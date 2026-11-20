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

#include "geiss/effects.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace geiss {
namespace {

// floatframe advanced 1.6 per frame at 30 fps.
constexpr double kFloatFramesPerFrame = 1.6;
// The original's effect sizes are for a 640-wide frame.
constexpr float kReferenceWidth = 640.f;

struct BaseMode {
    // Chasers, Bar, Dots, Solar, Grid, Nuclide, Shade.
    std::array<int, kEffectCount> weights;
    int solarMax;
    float centerDwindle;
    int minEffects;
    int maxEffects;
};

// CModeInfo for modes 1...16, before the truecolor adjustment. Mode 8's shade weight is
// 60 because the original chained its assignment into solar_max.
constexpr std::array<BaseMode, 16> kLowModes = {{
    {{220, 150, 10, 680, 4, 170, 400}, 400, 1.f, 1, 2},
    {{750, 500, 750, 750, 0, 0, 0}, 35, 1.f, 1, 5},
    {{100, 100, 100, 500, 10, 0, 300}, 60, 0.99f, 1, 2},
    {{500, 100, 100, 100, 30, 0, 0}, 34, 0.98f, 1, 2},
    {{100, 350, 100, 500, 15, 180, 500}, 60, 0.99f, 1, 2},
    {{400, 120, 200, 0, 0, 0, 0}, 60, 1.f, 1, 2},
    {{50, 200, 0, 300, 0, 600, 350}, 65, 0.985f, 1, 2},
    {{150, 150, 150, 150, 25, 0, 60}, 60, 0.96f, 1, 2},
    {{450, 200, 50, 200, 0, 100, 200}, 50, 0.985f, 1, 2},
    {{150, 20, 80, 0, 0, 80, 0}, 0, 1.f, 0, 2},
    {{360, 200, 230, 550, 10, 330, 150}, 750, 1.f, 0, 4},
    {{360, 200, 230, 0, 0, 330, 0}, 500, 0.915f, 0, 2},
    {{500, 0, 100, 0, 30, 0, 0}, 34, 0.98f, 1, 2},
    {{500, 0, 100, 0, 30, 0, 0}, 34, 0.98f, 1, 2},
    {{0, 0, 0, 0, 0, 200, 0}, 60, 1.f, 0, 1},
    {{500, 100, 100, 100, 30, 0, 0}, 34, 0.98f, 1, 2},
}};

// Modes 17 and up share one setup. Modes 20...23 darken their center.
constexpr BaseMode kHighMode = {{150, 150, 150, 150, 12, 0, 50}, 600, 1.f, 1, 3};

std::size_t slot(Effect effect) { return static_cast<std::size_t>(effect); }

int capped(int value, int limit) { return std::clamp(value, 0, limit); }

std::uint8_t* pixel(std::uint8_t* indices, int width, int height, int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return nullptr;
    }
    return &indices[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
}

void add(std::uint8_t* indices, int width, int height, int x, int y, int amount) {
    if (std::uint8_t* p = pixel(indices, width, height, x, y)) {
        *p = static_cast<std::uint8_t>(std::min(255, *p + amount));
    }
}

// Adds only while the pixel is below `limit`, as ShadeBobs did.
void addBelow(std::uint8_t* indices, int width, int height, int x, int y, int amount, int limit) {
    if (std::uint8_t* p = pixel(indices, width, height, x, y); p != nullptr && *p < limit) {
        *p = static_cast<std::uint8_t>(std::min(255, *p + amount));
    }
}

}  // namespace

int EffectSet::count() const {
    return static_cast<int>(std::count(on.begin(), on.end(), true));
}

ModeEffects modeEffects(int mode) {
    const BaseMode& base =
        mode >= 1 && mode <= static_cast<int>(kLowModes.size()) ? kLowModes[static_cast<std::size_t>(mode - 1)] : kHighMode;
    ModeEffects info;
    info.weights = base.weights;
    info.solarMax = base.solarMax;
    info.centerDwindle = base.centerDwindle;
    if (mode >= 20 && mode <= 23) {
        info.centerDwindle = 0.98f;
    }
    info.minEffects = base.minEffects;
    info.maxEffects = base.maxEffects;
    // The original's truecolor adjustment: more nuclide, bar, dots, and shade, fewer chasers.
    auto& w = info.weights;
    w[slot(Effect::Nuclide)] = capped(static_cast<int>(static_cast<float>(w[slot(Effect::Nuclide)]) * 1.3f), 900);
    w[slot(Effect::Chasers)] = capped(w[slot(Effect::Chasers)] - 50, 900);
    w[slot(Effect::Dots)] = capped(w[slot(Effect::Dots)] + 220, 900);
    w[slot(Effect::Bar)] = capped(w[slot(Effect::Bar)] + 220, 900);
    w[slot(Effect::Shade)] = capped(w[slot(Effect::Shade)] + 150, 900);
    // And for every color depth, a little more grid.
    w[slot(Effect::Grid)] = capped(w[slot(Effect::Grid)] + 8, 1000);
    return info;
}

EffectSet randomEffects(int mode, bool soundActive, Random& random) {
    const ModeEffects info = modeEffects(mode);
    const auto& weights = info.weights;
    EffectSet effects;
    for (std::size_t i = 0; i < effects.on.size(); ++i) {
        int threshold = weights[i];
        if (soundActive) {
            threshold = static_cast<int>(static_cast<float>(threshold) * 0.7f);
        }
        effects.on[i] = random.below(1000) < threshold;
    }

    // Clip_Num_Effects. Without sound it adds effects until the mode's minimum is met. It
    // stops when no effect left can come up, where the original would have hung.
    int count = effects.count();
    auto canAdd = [&] {
        for (std::size_t i = 0; i < effects.on.size(); ++i) {
            if (!effects.on[i] && weights[i] > 0) {
                return true;
            }
        }
        return false;
    };
    if (!soundActive) {
        while (count < info.minEffects && canAdd()) {
            for (std::size_t i = 0; i < effects.on.size(); ++i) {
                if (!effects.on[i] && random.below(1000) < weights[i]) {
                    effects.on[i] = true;
                    ++count;
                    break;
                }
            }
        }
    }
    for (std::size_t i = 0; i < effects.on.size(); ++i) {
        if (weights[i] >= 1000 && !effects.on[i]) {
            effects.on[i] = true;
            ++count;
        }
    }
    auto canDrop = [&] {
        for (std::size_t i = 0; i < effects.on.size(); ++i) {
            if (effects.on[i] && weights[i] < 1000) {
                return true;
            }
        }
        return false;
    };
    while (count > info.maxEffects && canDrop()) {
        const auto i = static_cast<std::size_t>(random.below(kEffectCount));
        if (effects.on[i] && weights[i] < 1000) {
            effects.on[i] = false;
            --count;
        }
    }

    if (effects.has(Effect::Chasers)) {
        effects.chasers = 1 + random.below(2);
    }
    // A grid turns the heavy bar off.
    if (effects.has(Effect::Grid)) {
        effects.set(Effect::Bar, false);
    }
    effects.solarBurst = mode == 1 && random.below(2) == 0;
    effects.gridDirection = random.below(2) * 2 - 1;
    // The nuclide replaces the waveform 4 times in 7. The original set mode 10's waveform
    // again afterwards, so mode 10 keeps it.
    effects.hideWaveform = effects.has(Effect::Nuclide) && random.below(7) > 2 && mode != 10;
    return effects;
}

EffectLayer::EffectLayer(std::uint32_t seed) {
    random_.state = seed == 0 ? 1u : seed;
    chaserOffset_ = random_.below(40000);
    for (std::size_t i = 0; i < bobFrequency_.size(); ++i) {
        bobFrequency_[i] = 0.1f + 0.05f * random_.unit();
        bobRadius_[i] = 2.f + 2.8f * random_.unit();
    }
}

void EffectLayer::startLook(const EffectSet& effects) {
    trailFilled_ = 0;
    burstPending_ = effects.solarBurst;
}

void EffectLayer::draw(std::uint8_t* indices, int width, int height, const EffectSet& effects, int mode,
                       float centerX, float centerY, double frames, const AudioFrame& audio) {
    if (indices == nullptr || width < 2 || height < 2 || frames <= 0) {
        return;
    }
    const double previous = clock_;
    clock_ += frames;
    const int steps = static_cast<int>(std::floor(clock_) - std::floor(previous));
    const Canvas canvas{indices, width, height, centerX, centerY, static_cast<float>(width) / kReferenceWidth};
    const ModeEffects info = modeEffects(mode);

    // avg_vol_narrow follows the volume every frame, whether or not the nuclide is on.
    for (int i = 0; i < steps; ++i) {
        narrowVolume_ = narrowVolume_ * 0.3f + audio.volume * 0.7f;
    }

    if (burstPending_) {
        burstPending_ = false;
        // Drop_Solar_Particles(500), times 4 in 8-bit color.
        dropSolarParticles(canvas, static_cast<int>(2000.f * canvas.scale * canvas.scale));
    }
    if (effects.has(Effect::Shade)) {
        shadeBobs(canvas, frames);
    }
    if (effects.has(Effect::Chasers)) {
        chasers(canvas, effects.chasers, previous);
    }
    if (effects.has(Effect::Bar)) {
        bar(canvas, frames);
    }
    if (effects.has(Effect::Dots)) {
        dots(canvas, steps);
    }
    if (effects.has(Effect::Nuclide)) {
        nuclide(canvas, steps, audio);
    }
    if (effects.has(Effect::Grid)) {
        grid(canvas, effects.gridDirection);
    }
    if (effects.has(Effect::Solar)) {
        solar(canvas, info.solarMax, frames);
    }
    diminishCenter(canvas, mode, info.centerDwindle, steps);
}

// ShadeBobs: one bob wobbles a few pixels around the center and takes four jittered
// steps per frame, each adding 2 to its pixel and 1 to the four beside it.
void EffectLayer::shadeBobs(const Canvas& canvas, double frames) {
    bobSteps_ += 4.0 * frames;
    const auto t = static_cast<float>(clock_ * kFloatFramesPerFrame);
    const float s = canvas.scale;
    int a = static_cast<int>(canvas.centerX + s * (bobRadius_[0] * std::cos(t * bobFrequency_[0]) +
                                                    bobRadius_[2] * std::cos(t * bobFrequency_[1])));
    int b = static_cast<int>(canvas.centerY + s * (bobRadius_[1] * std::cos(t * bobFrequency_[2]) +
                                                    bobRadius_[3] * std::cos(t * bobFrequency_[3])));
    while (bobSteps_ >= 1.0) {
        bobSteps_ -= 1.0;
        b += random_.below(5) - 2;
        a += random_.below(5) - 2;
        addBelow(canvas.indices, canvas.width, canvas.height, a, b, 2, 250);
        addBelow(canvas.indices, canvas.width, canvas.height, a + 1, b, 1, 250);
        addBelow(canvas.indices, canvas.width, canvas.height, a - 1, b, 1, 250);
        addBelow(canvas.indices, canvas.width, canvas.height, a, b + 1, 1, 250);
        addBelow(canvas.indices, canvas.width, canvas.height, a, b - 1, 1, 250);
    }
}

// Two_Chasers: one or two points on two-cosine curves around the center. Each frame they
// move 1.6 floatframes in 20 steps (at 640 wide), and each step lifts its pixel 40% of the
// way to 255. The steps sit on a fixed grid of floatframes, so the path is the same at any
// frame rate.
void EffectLayer::chasers(const Canvas& canvas, int count, double previousClock) {
    const float s = canvas.scale;
    const int perFrame = std::max(1, static_cast<int>(20.f * s));
    const double spacing = kFloatFramesPerFrame / perFrame;
    const double start = previousClock * kFloatFramesPerFrame + chaserOffset_;
    const double end = clock_ * kFloatFramesPerFrame + chaserOffset_;
    // Steps are counted by index, so a step on a tick boundary is drawn exactly once.
    const auto last = static_cast<long long>(std::floor(end / spacing));
    for (auto n = static_cast<long long>(std::floor(start / spacing)) + 1; n <= last; ++n) {
        const double t = static_cast<double>(n) * spacing;
        for (int pass = 0; pass < count; ++pass) {
            int a = 0;
            int b = 0;
            if (pass == 0) {
                a = static_cast<int>(canvas.centerX + s * static_cast<float>(74.0 * std::cos(t * 0.1102 + 10.0) +
                                                                             65.0 * std::cos(t * 0.1312 + 20.0)));
                b = static_cast<int>(canvas.centerY + s * static_cast<float>(54.0 * std::cos(t * 0.1204 + 40.0) +
                                                                             55.0 * std::cos(t * 0.1715 + 30.0)));
            } else {
                a = static_cast<int>(canvas.centerX + s * static_cast<float>(64.0 * std::cos(t * 0.1213 + 33.0) +
                                                                             55.0 * std::cos(t * 0.1408 + 15.0)));
                b = static_cast<int>(canvas.centerY + s * static_cast<float>(52.0 * std::cos(t * 0.1304 + 12.0) +
                                                                             51.0 * std::cos(t * 0.1103 + 21.0)));
            }
            if (std::uint8_t* p = pixel(canvas.indices, canvas.width, canvas.height, a, b)) {
                *p = static_cast<std::uint8_t>(255.f - static_cast<float>(255 - *p) * 0.6f);
            }
        }
    }
}

// Solid_Line: a short line between two endpoints that wander near the center. Each frame
// adds 16 to every pixel on it that is below 223. Between frames it adds its share of 16.
void EffectLayer::bar(const Canvas& canvas, double frames) {
    barCarry_ += static_cast<float>(16.0 * frames);
    const int amount = static_cast<int>(barCarry_);
    if (amount < 1) {
        return;
    }
    barCarry_ -= static_cast<float>(amount);
    const float s = canvas.scale;
    // The bar uses 0.6 of the chaser offset, and 0.55 / 1.6 of floatframe.
    const double t = (clock_ * kFloatFramesPerFrame + chaserOffset_ * 0.6) * 0.55 / kFloatFramesPerFrame;
    const float x1 = canvas.centerX + s * static_cast<float>(16.0 * std::cos(t * 0.1102 + 10) + 15.0 * std::cos(t * 0.1312 + 20));
    const float y1 = canvas.centerY + s * static_cast<float>(15.0 * std::cos(t * 0.1204 + 40) + 10.0 * std::cos(t * 0.1715 + 30));
    const float x2 = canvas.centerX + s * static_cast<float>(14.0 * std::cos(t * 0.1213 + 33) + 13.0 * std::cos(t * 0.1408 + 15));
    const float y2 = canvas.centerY + s * static_cast<float>(13.0 * std::cos(t * 0.1304 + 12) + 11.0 * std::cos(t * 0.1103 + 21));
    const int points = std::max(1, static_cast<int>(s * 50.f));
    for (int k = 0; k < points; ++k) {
        const float u = static_cast<float>(k) / static_cast<float>(points);
        const int a = static_cast<int>(x1 * u + x2 * (1.f - u));
        const int b = static_cast<int>(y1 * u + y2 * (1.f - u));
        addBelow(canvas.indices, canvas.width, canvas.height, a, b, amount, 223);
    }
}

// One_Dotty_Chaser: a slow point leaves a trail of its last 20 positions. Each frame the
// trail is redrawn at a value that cycles with time, and every dot drifts one pixel right.
void EffectLayer::dots(const Canvas& canvas, int steps) {
    const float s = canvas.scale;
    for (int step = 0; step < std::min(steps, kTrail); ++step) {
        const double t = (clock_ - static_cast<double>(std::min(steps, kTrail) - 1 - step)) * kFloatFramesPerFrame;
        const int a = static_cast<int>(canvas.centerX + s * static_cast<float>(64.0 * std::cos(t * 0.0613 + 33) +
                                                                               55.0 * std::cos(t * 0.0708 + 15)));
        const int b = static_cast<int>(canvas.centerY + s * static_cast<float>(52.0 * std::cos(t * 0.0704 + 12) +
                                                                               51.0 * std::cos(t * 0.0503 + 21)));
        if (b < 0 || b >= canvas.height) {
            continue;
        }
        trailNewest_ = (trailNewest_ + 1) % kTrail;
        trailFilled_ = std::min(trailFilled_ + 1, kTrail);
        const auto newest = static_cast<std::size_t>(trailNewest_);
        trailX_[newest] = a;
        trailY_[newest] = b;
        trailValue_[newest] = static_cast<std::uint8_t>(127.0 + 126.0 * std::sin(t * 0.0613 + 33));
        for (int k = 0; k < trailFilled_; ++k) {
            const auto i = static_cast<std::size_t>((trailNewest_ - k + kTrail) % kTrail);
            if (std::uint8_t* p = pixel(canvas.indices, canvas.width, canvas.height, trailX_[i], trailY_[i])) {
                *p = trailValue_[i];
            }
            ++trailX_[i];
        }
    }
}

// The solar flare: Drop_Solar_Particles' count swings with time and the mode's strength.
// At 640 wide that is int(fx * 0.05) * 4 particles a frame. Wider frames get more, so the
// density stays the same.
void EffectLayer::solar(const Canvas& canvas, int solarMax, double frames) {
    const double f = clock_;
    const double max = solarMax;
    const double fx = 3.0 + max * 1.6 + max * 0.43 * std::sin(f * 0.05) + max * 0.43 * std::sin(f * 0.038 + 1);
    const int perFrame = static_cast<int>(fx * 0.05) * 4;
    solarParticles_ += static_cast<double>(perFrame) * frames * static_cast<double>(canvas.scale * canvas.scale);
    const int count = static_cast<int>(solarParticles_);
    solarParticles_ -= count;
    dropSolarParticles(canvas, count);
}

// Each particle lands within 35 pixels (at 640 wide) of the center and adds a 3x3 glow that
// is brighter near the center. A pixel already at 207 or more takes no particle.
void EffectLayer::dropSolarParticles(const Canvas& canvas, int count) {
    for (int k = 0; k < count; ++k) {
        int x = 0;
        int y = 0;
        float distance = 100.f;
        while (distance >= 35.f) {
            y = random_.below(72) - 36;
            x = random_.below(96) - 48;
            distance = std::sqrt(static_cast<float>(x * x + y * y));
        }
        const int a = static_cast<int>(canvas.centerX + static_cast<float>(x) * canvas.scale);
        const int b = static_cast<int>(canvas.centerY + static_cast<float>(y) * canvas.scale);
        const int i0 = 2 + random_.below(2) + static_cast<int>((35.f - distance) / 9.f);
        const int i1 = i0 - 1;
        const int i2 = i1 - 1;
        const std::uint8_t* center = pixel(canvas.indices, canvas.width, canvas.height, a, b);
        if (center == nullptr || *center >= 207 - i0) {
            continue;
        }
        add(canvas.indices, canvas.width, canvas.height, a, b, i0);
        add(canvas.indices, canvas.width, canvas.height, a + 1, b, i1);
        add(canvas.indices, canvas.width, canvas.height, a - 1, b, i1);
        add(canvas.indices, canvas.width, canvas.height, a, b + 1, i1);
        add(canvas.indices, canvas.width, canvas.height, a, b - 1, i1);
        add(canvas.indices, canvas.width, canvas.height, a - 1, b - 1, i2);
        add(canvas.indices, canvas.width, canvas.height, a + 1, b - 1, i2);
        add(canvas.indices, canvas.width, canvas.height, a - 1, b + 1, i2);
        add(canvas.indices, canvas.width, canvas.height, a + 1, b + 1, i2);
    }
}

// Grid: a dot every 1/30 of the width, in both directions. The dots scroll one pixel a
// frame, left or right, and their brightness pulses. A dot never darkens a pixel.
void EffectLayer::grid(const Canvas& canvas, int direction) {
    const int spacing = std::max(2, canvas.width / 30);
    const double f = clock_;
    const double level = 65.0 + 45.0 * std::sin(f * 0.06033) + 35.0 * std::cos(f * 0.04710 + 1) +
                         25.0 * std::cos(f * 0.00523 - 1);
    const int bright = std::clamp(static_cast<int>(level), 0, 255);
    if (bright == 0) {
        return;
    }
    int shift = static_cast<int>(static_cast<long long>(std::floor(f)) % spacing);
    if (direction == 1) {
        shift = -shift;
    }
    for (int y = 0; y < canvas.height; y += spacing) {
        for (int x = 0; x < canvas.width; x += spacing) {
            if (std::uint8_t* p = pixel(canvas.indices, canvas.width, canvas.height, x + shift, y); p != nullptr && *p < bright) {
                *p = static_cast<std::uint8_t>(bright);
            }
        }
    }
}

// Nuclide. With sound, a jump in volume (above 1.1 times avg_vol_narrow) drops a ring of
// discs whose size follows the jump. In silence, a ring of random size appears about one
// frame in 12.
void EffectLayer::nuclide(const Canvas& canvas, int steps, const AudioFrame& audio) {
    const bool sound = !audio.silent && !audio.samples.empty();
    for (int step = 0; step < steps; ++step) {
        if (!sound) {
            if (random_.below(12) == 0) {
                nuclideRing(canvas, static_cast<float>(3 + random_.below(8)));
            }
            continue;
        }
        if (audio.volume > narrowVolume_ * 1.1f) {
            const int radius = std::clamp(static_cast<int>(3.f + 40.f * (audio.volume / narrowVolume_ - 1.1f)), 1, 10);
            nuclideRing(canvas, static_cast<float>(radius));
        }
    }
}

// 3...7 soft discs evenly around a circle of 34...41 pixels (at 640 wide) about the center.
// Each adds (radius - distance) * 25 out to its radius.
void EffectLayer::nuclideRing(const Canvas& canvas, float discRadius) {
    const int nodes = 3 + random_.below(5);
    const auto phase = static_cast<float>(random_.below(1000));
    const float ring = static_cast<float>(34 + random_.below(8)) * canvas.scale;
    const int extent = static_cast<int>(std::ceil(10.f * canvas.scale));
    for (int n = 0; n < nodes; ++n) {
        const float angle = static_cast<float>(n) / static_cast<float>(nodes) * 6.28f + phase;
        const int cx = static_cast<int>(canvas.centerX + ring * std::cos(angle));
        const int cy = static_cast<int>(canvas.centerY + ring * std::sin(angle));
        for (int dy = -extent; dy <= extent; ++dy) {
            for (int dx = -extent; dx <= extent; ++dx) {
                const float distance = std::sqrt(static_cast<float>(dx * dx + dy * dy)) / canvas.scale;
                const int value = static_cast<int>((discRadius - distance) * 25.f);
                if (value > 0) {
                    add(canvas.indices, canvas.width, canvas.height, cx + dx, cy + dy, value);
                }
            }
        }
    }
}

// Diminish_Center: darkens the center pixel and its four neighbors each frame, so the
// zoom does not burn a hot spot there. Mode 12 splits sideways from a vertical line, so
// it darkens a 3-pixel column instead.
void EffectLayer::diminishCenter(const Canvas& canvas, int mode, float dwindle, int steps) {
    if (dwindle >= 0.999f || steps < 1) {
        return;
    }
    const auto factor = static_cast<float>(std::pow(static_cast<double>(dwindle), steps));
    const int cx = static_cast<int>(canvas.centerX);
    const int cy = static_cast<int>(canvas.centerY);
    auto darken = [&](int x, int y, int floor) {
        if (std::uint8_t* p = pixel(canvas.indices, canvas.width, canvas.height, x, y); p != nullptr && *p > floor) {
            *p = static_cast<std::uint8_t>(static_cast<float>(*p) * factor);
        }
    };
    if (mode == 12) {
        for (int y = 0; y < canvas.height; ++y) {
            darken(cx - 1, y, 0);
            darken(cx, y, 0);
            darken(cx + 1, y, 0);
        }
        return;
    }
    darken(cx, cy, 1);
    darken(cx - 1, cy, 1);
    darken(cx + 1, cy, 1);
    darken(cx, cy + 1, 1);
    darken(cx, cy - 1, 1);
}

}  // namespace geiss
