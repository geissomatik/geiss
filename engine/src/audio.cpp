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

std::vector<float> channel(const PcmBlock& block, int which, float gain) {
    std::vector<float> out(static_cast<std::size_t>(block.frameCount));
    for (int i = 0; i < block.frameCount; ++i) {
        const float* sample = block.samples + static_cast<std::size_t>(i * block.channels);
        float value = sample[0];
        if (which < 0) {
            value = downmix(sample, block.channels);
        } else if (which == 1 && block.channels > 1) {
            value = sample[1];
        }
        out[static_cast<std::size_t>(i)] = value * gain;
    }
    return out;
}

void resample(const std::vector<float>& source, std::vector<float>& dest) {
    const int columns = static_cast<int>(dest.size());
    if (source.empty() || columns < 1) {
        return;
    }
    if (columns == 1 || source.size() == 1) {
        const float value = source.size() == 1 ? source[0] : source[source.size() / 2];
        std::fill(dest.begin(), dest.end(), value);
        return;
    }
    const float span = static_cast<float>(source.size() - 1);
    const float denom = static_cast<float>(columns - 1);
    for (int i = 0; i < columns; ++i) {
        const float pos = span * (static_cast<float>(i) / denom);
        const int i0 = static_cast<int>(pos);
        const int i1 = std::min(i0 + 1, static_cast<int>(source.size()) - 1);
        const float fraction = pos - static_cast<float>(i0);
        const float a = source[static_cast<std::size_t>(i0)];
        const float b = source[static_cast<std::size_t>(i1)];
        dest[static_cast<std::size_t>(i)] = a * (1.f - fraction) + b * fraction;
    }
}

// Rotate so the first rising zero crossing sits at column 0.
// Intentional difference from GetWaveData: one rising edge, no slope match across frames.
// Left and right take the same rotation so a stereo pair stays together.
int risingColumn(const std::vector<float>& samples) {
    const int n = static_cast<int>(samples.size());
    if (n < 2) {
        return 0;
    }
    for (int i = 0; i < n; ++i) {
        const float a = samples[static_cast<std::size_t>(i)];
        const float b = samples[static_cast<std::size_t>((i + 1) % n)];
        if (a <= 0.f && b > 0.f) {
            const int rising = i + 1;
            return rising >= n ? 0 : rising;
        }
    }
    return 0;
}

void rotateFrom(std::vector<float>& samples, int rising) {
    const int n = static_cast<int>(samples.size());
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
    std::vector<float> left;
    std::vector<float> right;
    const bool hasAudio = block.samples != nullptr && block.frameCount > 0 && block.channels > 0;
    if (hasAudio) {
        mono = channel(block, -1, gain);
        if (block.channels == 1) {
            left = mono;
            right = mono;
        } else {
            left = channel(block, 0, gain);
            right = channel(block, 1, gain);
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
    frame.left.assign(static_cast<std::size_t>(columns), 0.f);
    frame.right.assign(static_cast<std::size_t>(columns), 0.f);
    if (!mono.empty()) {
        resample(mono, frame.samples);
        resample(left, frame.left);
        resample(right, frame.right);
        const int rising = risingColumn(frame.samples);
        rotateFrom(frame.samples, rising);
        rotateFrom(frame.left, rising);
        rotateFrom(frame.right, rising);
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
