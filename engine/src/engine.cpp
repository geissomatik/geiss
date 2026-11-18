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

MapParameters randomMode25(std::uint32_t& state, int width, int height) {
    MapParameters map;
    const float centerX = (static_cast<float>(width) * 0.5f - 1.f) + (unit(state) * 60.f - 30.f);
    const float centerY = (static_cast<float>(height) * 0.5f - 1.f) + (unit(state) * 30.f - 15.f);
    map.centerXNorm = centerX / static_cast<float>(width);
    map.centerYNorm = centerY / static_cast<float>(height);
    // GenerateChunkOfNewMap for mode >= 17, then multiplied by 0.6, with a random sign.
    float turn = (0.007f + 0.02f * unit(state)) * 0.6f;
    if (unit(state) < 0.5f) {
        turn = -turn;
    }
    map.turn = turn;
    // The source uses the constant 3. The comment's useful range is about 2...6.
    map.zoom = 2.f + 4.f * unit(state);
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

// RenderWave waveform 1: a smoothed line across the frame, drawn with a max blend.
// Amplitude maps a -1...1 sample onto about 35% of the height. The original scaled
// 16-bit capture by volscale; that constant waits until real tap levels are known.
void drawWaveform(std::uint8_t* indices, int width, int height, const AudioFrame& audio) {
    if (indices == nullptr || audio.silent || audio.samples.empty() || width < 2 || height < 2) {
        return;
    }
    const float amplitude = static_cast<float>(height) * 0.35f;
    const float yCenter = static_cast<float>(height) * 0.5f;
    const int bright = std::clamp(static_cast<int>(40.f + audio.level * 400.f), 40, 255);
    float previous = audio.samples[0] * amplitude + yCenter;
    for (int x = 0; x < width; ++x) {
        const std::size_t sampleIndex = static_cast<std::size_t>(
            audio.samples.size() == 1 ? 0 : (static_cast<std::size_t>(x) * (audio.samples.size() - 1)) / static_cast<std::size_t>(width - 1));
        const float sample = audio.samples[sampleIndex];
        const float z = sample * amplitude + yCenter;
        previous = previous * 0.9f + z * 0.1f;
        const int y = static_cast<int>(previous);
        if (y < 0 || y >= height) {
            continue;
        }
        std::uint8_t& pixel = indices[static_cast<std::size_t>(y * width + x)];
        if (pixel < static_cast<std::uint8_t>(bright)) {
            pixel = static_cast<std::uint8_t>(bright);
        }
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
    width_ = width;
    height_ = height;
    const std::size_t count = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
    front_.assign(count, 0);
    back_.assign(count, 0);
    rgba_.assign(count * 4, 0);
    for (std::size_t i = 0; i < count; ++i) {
        rgba_[i * 4 + 3] = 255;
    }
    if (mapParameters_.zoom <= 0.f) {
        mapParameters_.zoom = 3.f;
        mapParameters_.centerXNorm = 0.5f;
        mapParameters_.centerYNorm = 0.5f;
    }
    rebuildMap();
}

void Engine::setPaused(bool paused) { paused_ = paused; }

void Engine::setLocked(bool locked) { locked_ = locked; }

void Engine::nextVisual() {
    mapParameters_ = randomMode25(rngState_, width_, height_);
    mapElapsed_ = 0;
    rebuildMap();
}

void Engine::rebuildMap() {
    if (mapParameters_.zoom <= 0.f) {
        mapParameters_ = randomMode25(rngState_, width_, height_);
    }
    buildMode25Map(map_, width_, height_, mapParameters_);
}

void Engine::expandPalette(const std::uint8_t* indices) {
    std::uint8_t palette[256 * 3];
    palette[0] = 0;
    palette[1] = 0;
    palette[2] = 0;
    for (int i = 1; i < 256; ++i) {
        const float value = static_cast<float>(i) / 255.f;
        // One hue cycle. Low indices stay dark so the warp fades toward black.
        // Intentional difference: this is not the original palette generator.
        hsv(phase_ + 0.08f * value, value, palette[i * 3], palette[i * 3 + 1], palette[i * 3 + 2]);
    }
    const std::size_t count = static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_);
    for (std::size_t i = 0; i < count; ++i) {
        const int index = indices[i];
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
        if (mapElapsed_ >= kMapInterval) {
            mapParameters_ = randomMode25(rngState_, width_, height_);
            mapElapsed_ = 0;
            rebuildMap();
        }
    }

    const float step = static_cast<float>(std::min(dt * kReferenceFps, 1.0));
    const float perReferenceFrame = static_cast<float>(weightNumerator(width_, height_)) / 256.f;
    const float decay = static_cast<float>(std::pow(static_cast<double>(perReferenceFrame), dt * kReferenceFps));
    warpIndices(front_.data(), back_.data(), width_, height_, map_.data(), step, decay);
    drawWaveform(back_.data(), width_, height_, audio);
    expandPalette(back_.data());
    front_.swap(back_);
    return view;
}

}  // namespace geiss
