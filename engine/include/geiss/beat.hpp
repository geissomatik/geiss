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

#include <array>

namespace geiss {

// What the beat detector reports after one analyzed block.
struct BeatState {
    // RMS of the low-passed newest 1/30 s of the block, after gain.
    float bass = 0;
    // A steady beat is on. Follows the "BEATMODE" test in the original volume code.
    bool beatMode = false;
    // Waveform brightness scale, 0...1. It follows each hit in beat mode and is 1 otherwise.
    float brightness = 1;
    // Smoothed bass over the loudest smoothed bass of the previous 4/3 s. A ratio
    // above the big-beat threshold is a big beat (bBigBeat in the original).
    float bigBeatRatio = 0;
};

// Bass level of one interleaved block. Two one-pole low-pass stages at `kBassCutoffHz`
// run across the whole block, and the RMS covers its newest 1/30 s, the original frame.
// Intentional difference: the original measured the composite volume of each frame.
constexpr double kBassCutoffHz = 150.0;
float bassLevel(const float* interleaved, int frameCount, int channels, double sampleRate, float gain);

// Port of the beat logic in the original volume code (main.cpp: "BEATMODE", bBigBeat,
// and the beat-mode brightness), fed by `bassLevel` instead of the composite volume.
// The history keeps the original 120 smoothed samples at a fixed 30 per second of
// elapsed time, so the caller's frame rate and overlapping blocks do not change it.
class BeatDetector {
public:
    static constexpr int kHistory = 120;
    static constexpr double kHistoryRate = 30.0;

    // `elapsed` is seconds since the previous update. Gaps count as at most half a second.
    BeatState update(float bass, double elapsed);

private:
    void push(float value);
    float at(int age) const;

    std::array<float, kHistory> history_{};
    int newest_ = -1;
    int filled_ = 0;
    float narrow_ = 0;
    double pending_ = 0;
    bool beatMode_ = false;
};

}  // namespace geiss
