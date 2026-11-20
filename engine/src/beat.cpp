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

#include "geiss/beat.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>

namespace geiss {
namespace {

// Below this smoothed bass level there is no beat. The original skipped beat mode under
// avg_vol_uniform < 10 on its 0...255 volume scale; this is the same idea on RMS.
constexpr float kBassGate = 0.01f;
// avg_vol_narrow = avg_vol_narrow*0.30 + vol*0.70 per frame at 30 fps.
constexpr double kNarrowRetain = 0.30;
// Beat mode turns on above 90 + 19 and off below 90 - 19.
constexpr float kBeatOn = 109.f;
constexpr float kBeatOff = 71.f;
// Steps smaller than this share of the average do not count toward beat strength.
constexpr float kStepAllowance = 0.15f;
// The big beat compares against the previous PAST_VOL_N / 3 samples.
constexpr int kRecent = BeatDetector::kHistory / 3;
constexpr double kMaxElapsed = 0.5;

}  // namespace

float bassLevel(const float* interleaved, int frameCount, int channels, double sampleRate, float gain) {
    if (interleaved == nullptr || frameCount < 1 || channels < 1 || sampleRate <= 0) {
        return 0.f;
    }
    const auto alpha = static_cast<float>(1.0 - std::exp(-2.0 * std::numbers::pi * kBassCutoffHz / sampleRate));
    const int window = std::clamp(static_cast<int>(sampleRate / BeatDetector::kHistoryRate), 1, frameCount);
    const int windowStart = frameCount - window;
    float first = 0;
    float second = 0;
    double sum = 0;
    for (int i = 0; i < frameCount; ++i) {
        const float* frame = interleaved + static_cast<std::size_t>(i) * static_cast<std::size_t>(channels);
        const float mono = channels > 1 ? 0.5f * (frame[0] + frame[1]) : frame[0];
        first += alpha * (mono - first);
        second += alpha * (first - second);
        if (i >= windowStart) {
            sum += static_cast<double>(second) * static_cast<double>(second);
        }
    }
    return static_cast<float>(std::sqrt(sum / static_cast<double>(window))) * std::max(gain, 0.f);
}

void BeatDetector::push(float value) {
    newest_ = (newest_ + 1) % kHistory;
    history_[static_cast<std::size_t>(newest_)] = value;
    filled_ = std::min(filled_ + 1, kHistory);
}

// `age` 0 is the newest sample.
float BeatDetector::at(int age) const {
    const int index = (newest_ - age + kHistory) % kHistory;
    return history_[static_cast<std::size_t>(index)];
}

BeatState BeatDetector::update(float bass, double elapsed) {
    elapsed = std::clamp(elapsed, 0.0, kMaxElapsed);
    const auto retain = static_cast<float>(std::pow(kNarrowRetain, elapsed * kHistoryRate));
    narrow_ = narrow_ * retain + bass * (1.f - retain);

    pending_ += elapsed;
    const double step = 1.0 / kHistoryRate;
    while (pending_ >= step) {
        push(narrow_);
        pending_ -= step;
    }

    BeatState state;
    state.bass = bass;
    if (filled_ < 2) {
        return state;
    }

    float average = 0;
    for (int age = 0; age < filled_; ++age) {
        average += at(age);
    }
    average /= static_cast<float>(filled_);

    // Beat mode: the smoothed bass tape is evenly bumpy with high amplitude.
    float strength = 0;
    if (average >= kBassGate) {
        for (int age = 1; age < filled_; ++age) {
            strength += std::max(0.f, std::fabs(at(age - 1) - at(age)) - average * kStepAllowance);
        }
        strength = strength / average * 10.f;
    }
    if (strength > kBeatOn) {
        beatMode_ = true;
    } else if (strength < kBeatOff) {
        beatMode_ = false;
    }
    state.beatMode = beatMode_;

    // Beat-mode brightness: 0 at the average, 1 at half a standard deviation above it.
    if (beatMode_) {
        float variance = 0;
        for (int age = 0; age < filled_; ++age) {
            const float delta = at(age) - average;
            variance += delta * delta;
        }
        variance /= static_cast<float>(filled_ - 1);
        const float deviation = std::sqrt(variance);
        state.brightness = deviation > 0.f ? std::clamp((bass - average) / (deviation * 0.5f), 0.f, 1.f) : 1.f;
    }

    // Big beat: the smoothed bass against the loudest of the previous samples. The original
    // scanned fixed array slots; this scans the newest samples before the current one.
    float recentMax = 0;
    for (int age = 1; age <= kRecent && age < filled_; ++age) {
        recentMax = std::max(recentMax, at(age));
    }
    state.bigBeatRatio = recentMax > 0.f ? narrow_ / recentMax : 0.f;
    return state;
}

}  // namespace geiss
