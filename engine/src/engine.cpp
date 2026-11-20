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

// The top `kWhiteHotSpan` of the palette blends from the hue toward white, eased in.
// At the original decay rate (about 0.992 per 1/30 s), a trail fades out of it in about 0.7 s.
constexpr float kWhiteHotSpan = 0.15f;

float whiteHot(float value) {
    const float t = std::clamp((value - (1.f - kWhiteHotSpan)) / kWhiteHotSpan, 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

// A -1...1 sample covers about 35% of the height. The original scaled 16-bit capture
// by volscale. That constant waits until real tap levels are known.
constexpr float kWaveFraction = 0.35f;
// RenderWave waveform 6 uses sin(intframe * 0.01). At the 30 fps reference that is sin(t * 0.3).
constexpr float kScopeAngleRate = 0.3f;
constexpr int kScopePoints = 314;
// RenderWave waveform 5 blends its first 50 points toward the points after 314, so the circle closes.
constexpr int kCirclePoints = 314;
constexpr int kCircleBlend = 50;

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

// RenderWave's plugin brightness: base = current_vol*4 + avg_vol*0.4 - 10, clamped to
// 0...155. The original then tinted that RGB base up to about 255; here 0...155 maps
// onto the palette's 0...255 index range, so normal listening levels draw at full brightness.
constexpr float kBaseCeiling = 155.f;

float waveBrightness(float volume, float averageVolume) {
    const float base = std::clamp(volume * 4.f + averageVolume * 0.4f - 10.f, 0.f, kBaseCeiling);
    return base * 255.f / kBaseCeiling;
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
// It reads the mono columns and stays on the frame center. Over mode 10's scroll the
// original lowered it to three quarters of the height.
void drawLine(std::uint8_t* indices, int width, int height, const AudioFrame& audio, int bright, int mode) {
    const float amplitude = static_cast<float>(height) * kWaveFraction;
    const float yCenter = static_cast<float>(height) * (mode == 10 ? 0.75f : 0.5f);
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

// RenderWave waveform 3: one smoothed vertical line through the map center. Row y reads
// the sample of column y, as the original read its buffer by row.
void drawVerticalLine(std::uint8_t* indices, int width, int height, const AudioFrame& audio,
                      int bright, float centerXNorm) {
    const float amplitude = static_cast<float>(height) * kWaveFraction;
    const float xCenter = centerXNorm * static_cast<float>(width);
    float previous = columnSample(audio.samples, 0, width) * amplitude + xCenter;
    for (int y = 0; y < height; ++y) {
        previous = previous * 0.9f + (columnSample(audio.samples, y, width) * amplitude + xCenter) * 0.1f;
        plot(indices, width, height, static_cast<int>(previous), y, bright);
    }
}

// RenderWave waveform 4: two smoothed diagonal lines at 0.9 of the line amplitude. Left
// runs down from the top-left corner and right from width - height, both one pixel
// across per row.
void drawDiagonals(std::uint8_t* indices, int width, int height, const AudioFrame& audio, int bright) {
    const float amplitude = static_cast<float>(height) * kWaveFraction * 0.9f;
    const std::vector<float>& left = audio.left.empty() ? audio.samples : audio.left;
    const std::vector<float>& right = audio.right.empty() ? left : audio.right;
    float previousLeft = columnSample(left, 0, width) * amplitude;
    float previousRight = columnSample(right, 0, width) * amplitude;
    for (int y = 0; y < height; ++y) {
        previousLeft = previousLeft * 0.9f + columnSample(left, y, width) * amplitude * 0.1f;
        previousRight = previousRight * 0.9f + columnSample(right, y, width) * amplitude * 0.1f;
        plot(indices, width, height, static_cast<int>(previousLeft) + y, y, bright);
        plot(indices, width, height, static_cast<int>(previousRight) + y + (width - height), y, bright);
    }
}

// RenderWave waveform 5: a circle around the map center whose radius follows the mono
// wave at 0.7 of the line amplitude. The base radius is 60 pixels on a 640-wide frame,
// and points closer than 5 (at 640) to the center are skipped.
// Intentional difference: like waveform 6, the 364 points cover the whole column wave
// instead of the first 364 frames of the capture buffer.
void drawCircle(std::uint8_t* indices, int width, int height, const AudioFrame& audio,
                int bright, float centerXNorm, float centerYNorm) {
    const float amplitude = static_cast<float>(height) * kWaveFraction * 0.7f;
    const float toPixels = static_cast<float>(width) / 640.f;
    const float baseRadius = 60.f * toPixels;
    const float centerX = centerXNorm * static_cast<float>(width);
    const float centerY = centerYNorm * static_cast<float>(height);
    constexpr int span = kCirclePoints + kCircleBlend;
    auto sampleAt = [&](int point) {
        return columnSample(audio.samples, point * (width - 1) / (span - 1), width);
    };
    float radius = baseRadius + sampleAt(0) * amplitude;
    for (int i = 0; i < kCirclePoints; ++i) {
        float sample = sampleAt(i);
        if (i < kCircleBlend) {
            const float blend = static_cast<float>(i) / static_cast<float>(kCircleBlend);
            sample = sample * blend + sampleAt(i + kCirclePoints) * (1.f - blend);
        }
        radius = radius * 0.5f + 0.5f * (baseRadius + sample * amplitude);
        if (radius < 5.f * toPixels) {
            continue;
        }
        const float angle = static_cast<float>(i) * 0.02f;
        plot(indices, width, height, static_cast<int>(centerX + radius * std::cos(angle)),
             static_cast<int>(centerY + radius * std::sin(angle)), bright);
    }
}

void drawWaveform(std::uint8_t* indices, int width, int height, const AudioFrame& audio,
                  int waveform, int mode, float centerXNorm, float centerYNorm, float motionTime) {
    if (indices == nullptr || audio.silent || audio.samples.empty() || width < 2 || height < 2) {
        return;
    }
    // Beat-mode brightness follows each hit and goes dark between them. As in the
    // original, the X-Y oscilloscope keeps its steady brightness.
    const float beatScale = audio.beatMode && waveform != 6 ? std::clamp(audio.beatBrightness, 0.f, 1.f) : 1.f;
    const int bright = static_cast<int>(waveBrightness(audio.volume, audio.averageVolume) * beatScale);
    if (bright < 1) {
        return;
    }
    switch (waveform) {
        case 2:
            drawTwoLines(indices, width, height, audio, bright, centerYNorm);
            break;
        case 3:
            drawVerticalLine(indices, width, height, audio, bright, centerXNorm);
            break;
        case 4:
            drawDiagonals(indices, width, height, audio, bright);
            break;
        case 5:
            drawCircle(indices, width, height, audio, bright, centerXNorm, centerYNorm);
            break;
        case 6:
            drawScope(indices, width, height, audio, bright, centerXNorm, centerYNorm, motionTime);
            break;
        default:
            drawLine(indices, width, height, audio, bright, mode);
            break;
    }
}

}  // namespace

bool lookAllowed(int mode, int waveform) {
    if (mode < kFirstMapMode || mode > kLastMapMode || waveform < kFirstWaveform || waveform > kLastWaveform) {
        return false;
    }
    // The pairs GenerateChunkOfNewMap re-rolls.
    return !((mode == 6 && waveform == 5) ||
             (mode == 12 && (waveform == 4 || waveform == 6)) ||
             (mode == 14 && (waveform == 3 || waveform == 4)) ||
             ((mode == 8 || mode == 23 || mode == 24) && waveform == 6));
}

Look randomLook(Random& random) {
    // FX_Pick_Random_Mode: uniform, then a 1-in-25 chance each of fuzzy (7) and super-perspective (5).
    Look look;
    look.mode = kFirstMapMode + random.below(kLastMapMode);
    if (random.below(25) == 0) {
        look.mode = 7;
    }
    if (random.below(25) == 0) {
        look.mode = 5;
    }
    // (rand() % (NUM_WAVES*3 - 1)) / 3 + 1: waveforms 1...5 at 3/17 each and 6 at 2/17.
    do {
        look.waveform = random.below(kLastWaveform * 3 - 1) / 3 + 1;
    } while (!lookAllowed(look.mode, look.waveform));
    return look;
}

// The effect layer draws its particles from its own generator, so the frame rate does not
// change which looks come up.
Engine::Engine(std::uint32_t seed) : effectLayer_(seed ^ 0x5BD1E995u) {
    random_.state = seed == 0 ? 1u : seed;
    pickEffects();
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
    effectLayer_.clearTrail();
    rebuildMap();
}

void Engine::setPaused(bool paused) { paused_ = paused; }

void Engine::setLocked(bool locked) { locked_ = locked; }

void Engine::pickLook() {
    if (width_ < 2 || height_ < 2) {
        return;
    }
    // Next and the timer always change the look, so the current pair is drawn again.
    Look picked = randomLook(random_);
    while (picked.mode == mapParameters_.mode && picked.waveform == waveform_) {
        picked = randomLook(random_);
    }
    mapParameters_ = randomMapParameters(picked.mode, width_, height_, random_);
    waveform_ = picked.waveform;
    pickEffects();
}

void Engine::pickEffects() {
    effects_ = randomEffects(mapParameters_.mode, soundActive_, random_);
    mapParameters_.nuclide = effects_.has(Effect::Nuclide);
    effectLayer_.startLook(effects_);
}

void Engine::setEffects(const EffectSet& effects) {
    effects_ = effects;
    effectLayer_.startLook(effects_);
    const bool nuclide = effects_.has(Effect::Nuclide);
    if (nuclide != mapParameters_.nuclide) {
        mapParameters_.nuclide = nuclide;
        if (mapParameters_.mode == 5) {
            rebuildMap();
        }
    }
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
        std::uint8_t* color = &palette[i * 3];
        hsv(phase_ + 0.08f * value, value, color[0], color[1], color[2]);
        // The top of the palette turns white-hot, so a freshly drawn full-brightness line,
        // such as a beat hit, stands out from the decaying trails around it.
        const float white = whiteHot(value);
        for (std::size_t channel = 0; channel < 3; ++channel) {
            const float hue = static_cast<float>(color[channel]);
            color[channel] = static_cast<std::uint8_t>(hue + (255.f * value - hue) * white + 0.5f);
        }
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

    soundActive_ = !audio.silent && !audio.samples.empty();
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
    // Dampened modes move half as far per frame, as mode_motion_dampened did.
    const float step = static_cast<float>(std::min(dt * kReferenceFps, 1.0)) * mapParameters_.damping;
    const float perReferenceFrame =
        static_cast<float>(weightNumerator(width_, height_)) / 256.f * mapParameters_.decay;
    const float decay = static_cast<float>(std::pow(static_cast<double>(perReferenceFrame), dt * kReferenceFps));
    // The effects draw into the previous frame before the warp, as the original drew into VS1.
    effectLayer_.draw(front_.data(), width_, height_, effects_, mapParameters_.mode,
                      mapParameters_.centerXNorm * static_cast<float>(width_),
                      mapParameters_.centerYNorm * static_cast<float>(height_), dt * kReferenceFps, audio);
    warpIndices(front_.data(), back_.data(), width_, height_, map_.data(), step, decay);
    if (!effects_.hideWaveform) {
        drawWaveform(back_.data(), width_, height_, audio, waveform_, mapParameters_.mode, mapParameters_.centerXNorm,
                     mapParameters_.centerYNorm, static_cast<float>(motionTime_));
    }
    expandPalette(back_.data());
    front_.swap(back_);
    return view;
}

}  // namespace geiss
