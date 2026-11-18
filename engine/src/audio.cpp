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

#include "geiss/audio.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace geiss {
namespace {

constexpr float kSilenceLevel = 0.01f;
constexpr double kSilenceHoldSeconds = 0.5;

float downmix(const float* frame, int channels) {
    if (channels <= 1) {
        return frame[0];
    }
    return 0.5f * (frame[0] + frame[1]);
}

// Rotate so the first rising zero crossing sits at column 0.
// Intentional difference from GetWaveData: one rising edge, no slope match across frames.
void alignRisingEdge(std::vector<float>& samples) {
    const int n = static_cast<int>(samples.size());
    if (n < 2) {
        return;
    }
    int rising = -1;
    for (int i = 0; i < n; ++i) {
        const float a = samples[static_cast<std::size_t>(i)];
        const float b = samples[static_cast<std::size_t>((i + 1) % n)];
        if (a <= 0.f && b > 0.f) {
            rising = i + 1;
            break;
        }
    }
    if (rising <= 0 || rising >= n) {
        return;
    }
    std::rotate(samples.begin(), samples.begin() + rising, samples.end());
}

}  // namespace

AudioFrame Analyzer::analyze(const PcmBlock& block, int columns, float gain) {
    AudioFrame frame;
    frame.time = block.time;
    frame.beat = false;
    if (columns < 1) {
        return frame;
    }
    if (gain < 0.f) {
        gain = 0.f;
    }

    std::vector<float> mono;
    const bool hasAudio = block.samples != nullptr && block.frameCount > 0 && block.channels > 0;
    if (hasAudio) {
        mono.resize(static_cast<std::size_t>(block.frameCount));
        for (int i = 0; i < block.frameCount; ++i) {
            const float* sample = block.samples + static_cast<std::size_t>(i * block.channels);
            mono[static_cast<std::size_t>(i)] = downmix(sample, block.channels) * gain;
        }
    }

    double sum = 0;
    float peak = 0;
    for (float sample : mono) {
        sum += static_cast<double>(sample) * static_cast<double>(sample);
        peak = std::max(peak, std::fabs(sample));
    }
    frame.level = mono.empty() ? 0.f : static_cast<float>(std::sqrt(sum / static_cast<double>(mono.size())));
    frame.peak = peak;

    frame.samples.assign(static_cast<std::size_t>(columns), 0.f);
    if (!mono.empty()) {
        if (columns == 1 || mono.size() == 1) {
            const float value = mono.size() == 1 ? mono[0] : mono[mono.size() / 2];
            std::fill(frame.samples.begin(), frame.samples.end(), value);
        } else {
            const float span = static_cast<float>(mono.size() - 1);
            const float denom = static_cast<float>(columns - 1);
            for (int i = 0; i < columns; ++i) {
                const float pos = span * (static_cast<float>(i) / denom);
                const int i0 = static_cast<int>(pos);
                const int i1 = std::min(i0 + 1, static_cast<int>(mono.size()) - 1);
                const float fraction = pos - static_cast<float>(i0);
                const float a = mono[static_cast<std::size_t>(i0)];
                const float b = mono[static_cast<std::size_t>(i1)];
                frame.samples[static_cast<std::size_t>(i)] = a * (1.f - fraction) + b * fraction;
            }
        }
        alignRisingEdge(frame.samples);
    }

    double duration = 0;
    if (block.sampleRate > 0 && block.frameCount > 0) {
        duration = static_cast<double>(block.frameCount) / block.sampleRate;
    }
    if (frame.level < kSilenceLevel) {
        silenceHeld_ += duration;
    } else {
        silenceHeld_ = 0;
    }
    frame.silent = silenceHeld_ >= kSilenceHoldSeconds;
    return frame;
}

}  // namespace geiss
