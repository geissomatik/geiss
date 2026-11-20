// This file is part of the Geiss macOS port.
//                           ___ ___ ___ ___ ___
//                          / __| __|_ _/ __/ __|
//                         | (_ | _| | |\__ \__ \
//                          \___|___|___|___/___/
// 3-Clause BSD License
//
// Copyright (c) 1998-2022 Ryan Geiss (@geissomatik)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "geiss/engine.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace geiss {
namespace {

constexpr double kReferenceFps = 30.0;
constexpr double kMaxDt = 0.1;
constexpr double kMapInterval = 20.0;
constexpr float kHuePeriod = 24.f;
// When a look is due during beat mode, it waits for a big beat, as the finished map did in
// the original. The threshold starts at 1.10 and relaxes by 0.2.
// Intentional difference: the original relaxed over its 550-frame (about 18 s) switch
// period. Here it relaxes over the 5 s wait, and the look changes when the wait ends.
// A steady beat peaks near 1.05, so it lands the change on a hit after about 1.3 s,
// and a louder hit lands it at once.
constexpr float kBigBeatThreshold = 1.10f;
constexpr float kBigBeatRelax = 0.2f;
constexpr double kMaxBeatWait = 5.0;
constexpr double kBigBeatRelaxSeconds = kMaxBeatWait;

bool lookMayChange(double overdue, const AudioFrame& audio) {
    if (!audio.beatMode || overdue >= kMaxBeatWait) {
        return true;
    }
    const auto threshold = kBigBeatThreshold - kBigBeatRelax * static_cast<float>(overdue / kBigBeatRelaxSeconds);
    return audio.bigBeatRatio > threshold;
}

// xorshift32 so the engine does not depend on a global rand().
std::uint32_t nextRandom(std::uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

float unit(std::uint32_t& state) {
    return static_cast<float>(nextRandom(state) >> 8) / static_cast<float>(1u << 24);
}

struct Look {
    int mode;
    int wave;
};

// Eleven pairs. GenerateChunkOfNewMap rejects mode 24 with waveform 6.
constexpr Look kLooks[] = {
    {17, 1}, {17, 2}, {17, 6},
    {19, 1}, {19, 2}, {19, 6},
    {24, 1}, {24, 2},
    {25, 1}, {25, 2}, {25, 6},
};

MapParameters parametersFor(std::uint32_t& state, int width, int height, int mode) {
    MapParameters map;
    map.mode = mode;
    const float centerX = (static_cast<float>(width) * 0.5f - 1.f) + (unit(state) * 60.f - 30.f);
    const float centerY = (static_cast<float>(height) * 0.5f - 1.f) + (unit(state) * 30.f - 15.f);
    map.centerXNorm = centerX / static_cast<float>(width);
    map.centerYNorm = centerY / static_cast<float>(height);
    if (mode == 24) {
        // The mode 24 pixel loop replaces the shared turn with this constant.
        map.turn = 0.05f;
    } else {
        // GenerateChunkOfNewMap for mode >= 17, then multiplied by 0.6, with a random sign.
        float turn = (0.007f + 0.02f * unit(state)) * 0.6f;
        if (unit(state) < 0.5f) {
            turn = -turn;
        }
        map.turn = turn;
    }
    if (mode == 25) {
        // The source uses the constant 3. The comment's useful range is about 2...6.
        map.zoom = 2.f + 4.f * unit(state);
    }
    return map;
}

void hsv(float hue, float value, std::uint8_t& r, std::uint8_t& g, std::uint8_t& b) {
    hue = hue - std::floor(hue);
    const float h = hue * 6.f;
    const int sector = static_cast<int>(h);
    const float fraction = h - static_cast<float>(sector);
    const float p = 0.f;
    const float q = 1.f - fraction;
    const float t = fraction;
    float rf = 0;
    float gf = 0;
    float bf = 0;
    switch (sector % 6) {
        case 0: rf = 1; gf = t; bf = p; break;
        case 1: rf = q; gf = 1; bf = p; break;
        case 2: rf = p; gf = 1; bf = t; break;
        case 3: rf = p; gf = q; bf = 1; break;
        case 4: rf = t; gf = p; bf = 1; break;
        default: rf = 1; gf = p; bf = q; break;
    }
    r = static_cast<std::uint8_t>(std::clamp(rf * value, 0.f, 1.f) * 255.f + 0.5f);
    g = static_cast<std::uint8_t>(std::clamp(gf * value, 0.f, 1.f) * 255.f + 0.5f);
    b = static_cast<std::uint8_t>(std::clamp(bf * value, 0.f, 1.f) * 255.f + 0.5f);
}

// A -1...1 sample covers about 35% of the height. The original scaled 16-bit capture
// by volscale. That constant waits until real tap levels are known.
constexpr float kWaveFraction = 0.35f;
// RenderWave waveform 6 uses sin(intframe * 0.01). At the 30 fps reference that is sin(t * 0.3).
constexpr float kScopeAngleRate = 0.3f;
constexpr int kScopePoints = 314;

float columnSample(const std::vector<float>& samples, int x, int width) {
    if (samples.empty() || width < 2) {
        return 0.f;
    }
    const int clamped = std::clamp(x, 0, width - 1);
    if (static_cast<int>(samples.size()) == 1) {
        return samples[0];
    }
    const std::size_t index = static_cast<std::size_t>(clamped) * (samples.size() - 1) / static_cast<std::size_t>(width - 1);
    return samples[index];
}

void plot(std::uint8_t* indices, int width, int height, int x, int y, int bright) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }
    std::uint8_t& pixel = indices[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
    if (pixel < static_cast<std::uint8_t>(bright)) {
        pixel = static_cast<std::uint8_t>(bright);
    }
}

// RenderWave waveform 1: one smoothed line, drawn with a max blend.
// It reads the mono columns and stays on the frame center.
void drawLine(std::uint8_t* indices, int width, int height, const AudioFrame& audio, int bright) {
    const float amplitude = static_cast<float>(height) * kWaveFraction;
    const float yCenter = static_cast<float>(height) * 0.5f;
    float previous = columnSample(audio.samples, 0, width) * amplitude + yCenter;
    for (int x = 0; x < width; ++x) {
        const float z = columnSample(audio.samples, x, width) * amplitude + yCenter;
        previous = previous * 0.9f + z * 0.1f;
        plot(indices, width, height, x, static_cast<int>(previous), bright);
    }
}

// RenderWave waveform 2: two smoothed lines. Left sits 12% of the height above the
// map center, right sits 12% below, and both use 0.7 of the line amplitude.
// Intentional difference: each column uses that column's channel. The original
// interleaved buffer repeated each stereo pair across two pixels.
void drawTwoLines(std::uint8_t* indices, int width, int height, const AudioFrame& audio,
                  int bright, float centerYNorm) {
    const float amplitude = static_cast<float>(height) * kWaveFraction * 0.7f;
    const float center = centerYNorm * static_cast<float>(height);
    const float upper = center - static_cast<float>(height) * 0.12f;
    const float lower = center + static_cast<float>(height) * 0.12f;
    const std::vector<float>& left = audio.left.empty() ? audio.samples : audio.left;
    const std::vector<float>& right = audio.right.empty() ? left : audio.right;
    float previousLeft = columnSample(left, 0, width) * amplitude + upper;
    float previousRight = columnSample(right, 0, width) * amplitude + lower;
    for (int x = 0; x < width; ++x) {
        previousLeft = previousLeft * 0.9f + (columnSample(left, x, width) * amplitude + upper) * 0.1f;
        previousRight = previousRight * 0.9f + (columnSample(right, x, width) * amplitude + lower) * 0.1f;
        plot(indices, width, height, x, static_cast<int>(previousLeft), bright);
        plot(indices, width, height, x, static_cast<int>(previousRight), bright);
    }
}

// RenderWave waveform 6: an X-Y oscilloscope. 314 smoothed stereo points, scaled by 1.2,
// rocked around the map center. The original seeds the smoother with the unscaled first pair.
// Intentional difference: the original walks the first 314 frames of the capture buffer.
// This frame is already one sample per column, so the 314 points cover that whole wave.
void drawScope(std::uint8_t* indices, int width, int height, const AudioFrame& audio,
               int bright, float centerXNorm, float centerYNorm, float motionTime) {
    const float amplitude = static_cast<float>(height) * kWaveFraction * 1.2f;
    const float centerX = centerXNorm * static_cast<float>(width);
    const float centerY = centerYNorm * static_cast<float>(height);
    const float angle = std::sin(motionTime * kScopeAngleRate);
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    const std::vector<float>& left = audio.left.empty() ? audio.samples : audio.left;
    const std::vector<float>& right = audio.right.empty() ? left : audio.right;
    float px2 = columnSample(left, 0, width);
    float py2 = columnSample(right, 0, width);
    for (int i = 0; i < kScopePoints; ++i) {
        const int column = (i * (width - 1)) / (kScopePoints - 1);
        px2 = px2 * 0.5f + 0.5f * columnSample(left, column, width) * amplitude;
        py2 = py2 * 0.5f + 0.5f * columnSample(right, column, width) * amplitude;
        const float px = px2 * cosine + py2 * sine;
        const float py = px2 * -sine + py2 * cosine;
        plot(indices, width, height, static_cast<int>(px + centerX), static_cast<int>(py + centerY), bright);
    }
}

void drawWaveform(std::uint8_t* indices, int width, int height, const AudioFrame& audio,
                  int waveform, float centerXNorm, float centerYNorm, float motionTime) {
    if (indices == nullptr || audio.silent || audio.samples.empty() || width < 2 || height < 2) {
        return;
    }
    // Beat-mode brightness follows each hit and goes dark between them. As in the
    // original, the X-Y oscilloscope keeps its steady brightness.
    const float beatScale = audio.beatMode && waveform != 6 ? std::clamp(audio.beatBrightness, 0.f, 1.f) : 1.f;
    const float steady = std::clamp(40.f + audio.level * 400.f, 40.f, 255.f);
    const int bright = static_cast<int>(steady * beatScale);
    if (bright < 1) {
        return;
    }
    if (waveform == 2) {
        drawTwoLines(indices, width, height, audio, bright, centerYNorm);
    } else if (waveform == 6) {
        drawScope(indices, width, height, audio, bright, centerXNorm, centerYNorm, motionTime);
    } else {
        drawLine(indices, width, height, audio, bright);
    }
}

}  // namespace

Engine::Engine(std::uint32_t seed) : rngState_(seed == 0 ? 1u : seed) {
    resize(960, 540);
}

void Engine::resize(int width, int height) {
    width = std::max(width, 2);
    height = std::max(height, 2);
    if (width == width_ && height == height_) {
        return;
    }
    const int oldWidth = width_;
    const int oldHeight = height_;
    width_ = width;
    height_ = height;
    const std::size_t count = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);

    // Carry the picture into the new size, nearest-neighbor, so a live resize does not go black.
    back_.assign(count, 0);
    if (oldWidth >= 2 && oldHeight >= 2 && !front_.empty()) {
        for (int y = 0; y < height_; ++y) {
            const int sy = y * oldHeight / height_;
            for (int x = 0; x < width_; ++x) {
                const int sx = x * oldWidth / width_;
                back_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) + static_cast<std::size_t>(x)] =
                    front_[static_cast<std::size_t>(sy) * static_cast<std::size_t>(oldWidth) + static_cast<std::size_t>(sx)];
            }
        }
    }
    front_.swap(back_);
    back_.assign(count, 0);
    rgba_.assign(count * 4, 0);
    expandPalette(front_.data());
    rebuildMap();
}

void Engine::setPaused(bool paused) { paused_ = paused; }

void Engine::setLocked(bool locked) { locked_ = locked; }

void Engine::pickLook() {
    if (width_ < 2 || height_ < 2) {
        return;
    }
    Look choices[sizeof(kLooks) / sizeof(kLooks[0])];
    int count = 0;
    for (const Look& look : kLooks) {
        if (look.mode == mapParameters_.mode && look.wave == waveform_) {
            continue;
        }
        choices[count] = look;
        ++count;
    }
    if (count < 1) {
        return;
    }
    const Look picked = choices[nextRandom(rngState_) % static_cast<std::uint32_t>(count)];
    mapParameters_ = parametersFor(rngState_, width_, height_, picked.mode);
    waveform_ = picked.wave;
}

void Engine::nextVisual() {
    pickLook();
    mapElapsed_ = 0;
    rebuildMap();
}

void Engine::rebuildMap() {
    buildMap(map_, width_, height_, mapParameters_);
}

void Engine::expandPalette(const std::uint8_t* indices) {
    std::uint8_t palette[256 * 3];
    palette[0] = 0;
    palette[1] = 0;
    palette[2] = 0;
    for (std::size_t i = 1; i < 256; ++i) {
        const float value = static_cast<float>(i) / 255.f;
        // One hue cycle. Low indices stay dark so the warp fades toward black.
        // Intentional difference: this is not the original palette generator.
        hsv(phase_ + 0.08f * value, value, palette[i * 3], palette[i * 3 + 1], palette[i * 3 + 2]);
    }
    const std::size_t count = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t index = indices[i];
        rgba_[i * 4] = palette[index * 3];
        rgba_[i * 4 + 1] = palette[index * 3 + 1];
        rgba_[i * 4 + 2] = palette[index * 3 + 2];
        rgba_[i * 4 + 3] = 255;
    }
}

FrameView Engine::tick(double dt, const AudioFrame& audio) {
    FrameView view;
    view.rgba = rgba_.data();
    view.width = width_;
    view.height = height_;
    if (width_ < 2 || height_ < 2) {
        return view;
    }
    if (paused_) {
        return view;
    }
    if (dt < 0) {
        dt = 0;
    }
    if (dt > kMaxDt) {
        dt = kMaxDt;
    }

    if (!locked_) {
        phase_ += static_cast<float>(dt) / kHuePeriod;
        phase_ -= std::floor(phase_);
        mapElapsed_ += dt;
        if (mapElapsed_ >= kMapInterval && lookMayChange(mapElapsed_ - kMapInterval, audio)) {
            pickLook();
            mapElapsed_ = 0;
            rebuildMap();
        }
    }

    motionTime_ += dt;
    const float step = static_cast<float>(std::min(dt * kReferenceFps, 1.0));
    const float perReferenceFrame = static_cast<float>(weightNumerator(width_, height_)) / 256.f;
    const float decay = static_cast<float>(std::pow(static_cast<double>(perReferenceFrame), dt * kReferenceFps));
    warpIndices(front_.data(), back_.data(), width_, height_, map_.data(), step, decay);
    drawWaveform(back_.data(), width_, height_, audio, waveform_, mapParameters_.centerXNorm,
                 mapParameters_.centerYNorm, static_cast<float>(motionTime_));
    expandPalette(back_.data());
    front_.swap(back_);
    return view;
}

}  // namespace geiss
