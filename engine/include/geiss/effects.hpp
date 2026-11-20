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

#pragma once

#include "geiss/audio.hpp"
#include "geiss/map.hpp"

#include <array>
#include <cstdint>

namespace geiss {

// The original's layered effects (Effects.h), in its effect[] order.
enum class Effect : std::uint8_t { Chasers, Bar, Dots, Solar, Grid, Nuclide, Shade };
constexpr int kEffectCount = 7;

// One map mode's effect setup, from CModeInfo. Weights are out of 1000.
struct ModeEffects {
    std::array<int, kEffectCount> weights{};
    // Solar particle strength, from the original's 8-bit table.
    int solarMax = 60;
    // Per-frame darkening of the map center at 30 fps. 1 leaves it alone.
    float centerDwindle = 1.f;
    int minEffects = 1;
    int maxEffects = 2;
};

// The effects of one look.
struct EffectSet {
    std::array<bool, kEffectCount> on{};
    // Two_Chasers draws one chaser or two.
    int chasers = 1;
    // Grid scroll direction, 1 or -1.
    int gridDirection = 1;
    // The nuclide replaces the waveform in 4 looks out of 7.
    bool hideWaveform = false;
    // Map 1 sometimes opens with a burst of solar particles, whether or not Solar is on.
    bool solarBurst = false;

    bool has(Effect effect) const { return on[static_cast<std::size_t>(effect)]; }
    void set(Effect effect, bool value) { on[static_cast<std::size_t>(effect)] = value; }
    int count() const;
};

// CModeInfo for `mode`, with the original's truecolor adjustment applied to the weights.
ModeEffects modeEffects(int mode);

// The original's pick for a new look. `soundActive` lowers every weight to 0.7, and the
// minimum count is only enforced while there is no sound.
EffectSet randomEffects(int mode, bool soundActive, Random& random);

// Draws the effects into the index buffer before the warp, as the original drew into VS1.
// Time is counted in reference frames at 30 fps, so speeds do not depend on the frame rate.
class EffectLayer {
public:
    explicit EffectLayer(std::uint32_t seed);

    // Starts a new look's effects: clears the dot trail and arms the solar burst.
    void startLook(const EffectSet& effects);

    // `frames` is the elapsed time in reference frames (dt * 30). `centerX` and `centerY`
    // are the map center in pixels, and `mode` is the map mode.
    void draw(std::uint8_t* indices, int width, int height, const EffectSet& effects, int mode,
              float centerX, float centerY, double frames, const AudioFrame& audio);

private:
    struct Canvas {
        std::uint8_t* indices;
        int width;
        int height;
        float centerX;
        float centerY;
        float scale;
    };

    void shadeBobs(const Canvas& canvas, double frames);
    void chasers(const Canvas& canvas, int count, double previousClock);
    void bar(const Canvas& canvas, double frames);
    void dots(const Canvas& canvas, int steps);
    void solar(const Canvas& canvas, int solarMax, double frames);
    void dropSolarParticles(const Canvas& canvas, int count);
    void grid(const Canvas& canvas, int direction);
    void nuclide(const Canvas& canvas, int steps, const AudioFrame& audio);
    void nuclideRing(const Canvas& canvas, float discRadius);
    void diminishCenter(const Canvas& canvas, int mode, float dwindle, int steps);

    static constexpr int kTrail = 20;

    Random random_;
    // Reference frames since the start. floatframe ran at 1.6 of these.
    double clock_ = 0;
    // chaser_offset, a random phase for the chasers and the bar.
    double chaserOffset_ = 0;
    // ShadeBobs' micro_f1...f4 and micro_rad, picked once per run.
    std::array<float, 4> bobFrequency_{};
    std::array<float, 4> bobRadius_{};
    double bobSteps_ = 0;
    double solarParticles_ = 0;
    float barCarry_ = 0;
    std::array<int, kTrail> trailX_{};
    std::array<int, kTrail> trailY_{};
    std::array<std::uint8_t, kTrail> trailValue_{};
    int trailNewest_ = 0;
    int trailFilled_ = 0;
    bool burstPending_ = false;
    // avg_vol_narrow: 0.3 of the previous value and 0.7 of the new volume per frame.
    float narrowVolume_ = 1.f;
};

}  // namespace geiss
