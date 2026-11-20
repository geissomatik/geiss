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
#include "geiss/engine.hpp"
#include "geiss/map.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        ++failures;
    }
}

void checkNear(float actual, float expected, float tolerance, const char* message) {
    if (std::fabs(actual - expected) > tolerance) {
        std::cerr << message << " actual=" << actual << " expected=" << expected << '\n';
        ++failures;
    }
}

void testMode25PullsTowardCenter() {
    geiss::MapParameters map;
    map.centerXNorm = 0.5f;
    map.centerYNorm = 0.5f;
    map.turn = 0;
    map.zoom = 3;
    const geiss::SourcePoint center = geiss::mode25Source(50, 40, 100, 80, map);
    checkNear(center.x, 50.f, 0.01f, "center x stays put");
    checkNear(center.y, 40.f, 0.01f, "center y stays put");

    const geiss::SourcePoint edge = geiss::mode25Source(70, 40, 100, 80, map);
    check(edge.x < 70.f && edge.x > 50.f, "mode 25 looks inward from the right");

    map.zoom = 2;
    const geiss::SourcePoint fast = geiss::mode25Source(70, 40, 100, 80, map);
    map.zoom = 6;
    const geiss::SourcePoint slow = geiss::mode25Source(70, 40, 100, 80, map);
    check(fast.x < slow.x, "a smaller zoom constant pulls harder");
}

void testWarpDecayAndSample() {
    constexpr int width = 4;
    constexpr int height = 2;
    std::uint8_t src[8] = {10, 20, 0, 0, 30, 40, 0, 0};
    std::uint8_t dst[8] = {};
    std::vector<geiss::SourcePoint> map(8);
    for (int i = 0; i < 8; ++i) {
        map[static_cast<std::size_t>(i)] = {0.f, 0.f};
    }
    geiss::warpIndices(src, dst, width, height, map.data(), 1.f, 0.5f);
    check(dst[0] == 5, "identity sample at 0,0 decays by half");
    check(dst[1] == 5, "a target of 0,0 copies the decayed corner");

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            map[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)] = {static_cast<float>(x), static_cast<float>(y)};
        }
    }
    src[0] = 100;
    geiss::warpIndices(src, dst, width, height, map.data(), 0.f, 0.5f);
    check(dst[0] == 50, "step 0 samples the pixel itself");
}

void testAnalysis() {
    geiss::Analyzer analyzer;
    const float stereo[] = {1.f, -1.f, 1.f, -1.f};
    geiss::PcmBlock block;
    block.samples = stereo;
    block.frameCount = 2;
    block.channels = 2;
    block.sampleRate = 2;
    block.time = 0;
    const geiss::AudioFrame downmix = analyzer.analyze(block, 3, 1.f);
    checkNear(downmix.level, 0.f, 0.0001f, "stereo downmix of opposites is silent");
    check(downmix.samples.size() == 3, "analysis writes one sample per column");

    geiss::Analyzer levels;
    const float constant[] = {0.5f, 0.5f, 0.5f, 0.5f};
    block.samples = constant;
    block.frameCount = 4;
    block.channels = 1;
    block.sampleRate = 4;
    const geiss::AudioFrame gained = levels.analyze(block, 2, 2.f);
    checkNear(gained.level, 1.f, 0.0001f, "gain scales the RMS");
    checkNear(gained.peak, 1.f, 0.0001f, "gain scales the peak");
    check(!gained.silent, "a loud block is not silence");
    check(!gained.beat, "the beat flag waits");

    const float wave[] = {-0.5f, -0.2f, 0.1f, 0.4f};
    block.samples = wave;
    block.frameCount = 4;
    block.sampleRate = 4;
    geiss::Analyzer aligned;
    const geiss::AudioFrame edge = aligned.analyze(block, 4, 1.f);
    checkNear(edge.samples[0], 0.1f, 0.0001f, "rising edge moves to column 0");
    checkNear(edge.samples[1], 0.4f, 0.0001f, "samples after the edge keep their order");
    checkNear(edge.samples[2], -0.5f, 0.0001f, "samples before the edge wrap to the end");

    const float quiet[] = {0.f};
    block.samples = quiet;
    block.frameCount = 1;
    block.sampleRate = 4;
    geiss::Analyzer silence;
    const geiss::AudioFrame early = silence.analyze(block, 2, 1.f);
    check(!early.silent, "a quarter second of silence is not the hold yet");
    const geiss::AudioFrame later = silence.analyze(block, 2, 1.f);
    check(later.silent, "silence holds for half a second");
}

void testEnginePauseDrawsALine() {
    geiss::Engine engine(1);
    engine.resize(32, 24);
    geiss::AudioFrame audio;
    audio.level = 0.5f;
    audio.silent = false;
    audio.samples.resize(32);
    for (int i = 0; i < 32; ++i) {
        audio.samples[static_cast<std::size_t>(i)] = std::sin(static_cast<float>(i) * 0.4f);
    }
    const geiss::FrameView frame = engine.tick(1.0 / 30.0, audio);
    check(frame.width == 32 && frame.height == 24, "engine reports the simulation size");
    unsigned color = 0;
    const std::size_t pixels = static_cast<std::size_t>(frame.width) * static_cast<std::size_t>(frame.height);
    for (std::size_t i = 0; i < pixels; ++i) {
        color += frame.rgba[i * 4];
        color += frame.rgba[i * 4 + 1];
        color += frame.rgba[i * 4 + 2];
    }
    check(color > 0, "a tone leaves color in the frame");

    std::vector<std::uint8_t> frozen(frame.rgba, frame.rgba + pixels * 4);
    engine.setPaused(true);
    const geiss::FrameView held = engine.tick(1.0, audio);
    check(frozen.size() == static_cast<std::size_t>(held.width) * static_cast<std::size_t>(held.height) * 4, "pause keeps the size");
    bool same = true;
    for (std::size_t i = 0; i < frozen.size(); ++i) {
        same = same && frozen[i] == held.rgba[i];
    }
    check(same, "pause returns the same picture");
}

}  // namespace

int main() {  // NOLINT(bugprone-exception-escape): a test may abort on a failed allocation
    testMode25PullsTowardCenter();
    testWarpDecayAndSample();
    testAnalysis();
    testEnginePauseDrawsALine();
    if (failures != 0) {
        std::cerr << failures << " failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "ok\n";
    return EXIT_SUCCESS;
}
