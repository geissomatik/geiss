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
#include "geiss/beat.hpp"
#include "geiss/engine.hpp"
#include "geiss/map.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
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
    check(!gained.beatMode, "one block is not a beat");

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

std::vector<float> stereoTone(double frequency, int frames, double sampleRate, float amplitude) {
    std::vector<float> out(static_cast<std::size_t>(frames) * 2);
    for (int i = 0; i < frames; ++i) {
        const auto value = static_cast<float>(amplitude * std::sin(2.0 * std::numbers::pi * frequency * i / sampleRate));
        out[static_cast<std::size_t>(i) * 2] = value;
        out[static_cast<std::size_t>(i) * 2 + 1] = value;
    }
    return out;
}

void testBassLevel() {
    constexpr double rate = 48000;
    constexpr int frames = 4800;
    const std::vector<float> low = stereoTone(50, frames, rate, 0.5f);
    const std::vector<float> high = stereoTone(2000, frames, rate, 0.5f);
    const float lowLevel = geiss::bassLevel(low.data(), frames, 2, rate, 1.f);
    const float highLevel = geiss::bassLevel(high.data(), frames, 2, rate, 1.f);
    check(lowLevel > 0.25f, "a 50 Hz tone passes the bass filter");
    check(highLevel < lowLevel * 0.02f, "a 2 kHz tone is filtered out");
    checkNear(geiss::bassLevel(low.data(), frames, 2, rate, 2.f), lowLevel * 2.f, 0.0001f, "gain scales the bass level");
    checkNear(geiss::bassLevel(nullptr, frames, 2, rate, 1.f), 0.f, 0.f, "no samples has no bass");
    checkNear(geiss::bassLevel(low.data(), frames, 2, 0, 1.f), 0.f, 0.f, "no sample rate has no bass");
}

// Bass that hits for 50 ms twice a second, over a quiet floor.
float kick(double time) {
    return std::fmod(time, 0.5) < 0.05 ? 0.5f : 0.02f;
}

struct BeatRun {
    bool beatModeAtEnd = false;
    float minBrightness = 1;
    float maxBrightness = 0;
};

BeatRun runDetector(float (*bass)(double), double updatesPerSecond, double seconds) {
    geiss::BeatDetector detector;
    BeatRun run;
    const double step = 1.0 / updatesPerSecond;
    const int updates = static_cast<int>(seconds * updatesPerSecond);
    for (int i = 0; i < updates; ++i) {
        const geiss::BeatState state = detector.update(bass(i * step), step);
        run.beatModeAtEnd = state.beatMode;
        if (state.beatMode) {
            run.minBrightness = std::min(run.minBrightness, state.brightness);
            run.maxBrightness = std::max(run.maxBrightness, state.brightness);
        } else {
            check(state.brightness == 1.f, "brightness is full outside beat mode");
        }
    }
    return run;
}

void testBeatDetector() {
    const BeatRun kicks = runDetector(kick, 60, 8);
    check(kicks.beatModeAtEnd, "a steady kick turns beat mode on");
    check(kicks.minBrightness < 0.1f && kicks.maxBrightness > 0.9f, "beat brightness follows the hits");

    check(runDetector(kick, 120, 8).beatModeAtEnd, "beat mode does not depend on the update rate");
    check(runDetector(kick, 24, 8).beatModeAtEnd, "beat mode holds at a slow update rate");

    check(!runDetector([](double) { return 0.3f; }, 60, 8).beatModeAtEnd, "steady bass is not a beat");
    check(!runDetector([](double t) { return kick(t) * 0.01f; }, 60, 8).beatModeAtEnd, "a beat under the gate is ignored");

    geiss::BeatDetector detector;
    for (int i = 0; i < 120; ++i) {
        detector.update(0.1f, 1.0 / 60.0);
    }
    const geiss::BeatState steady = detector.update(0.1f, 1.0 / 60.0);
    checkNear(steady.bigBeatRatio, 1.f, 0.01f, "steady bass has a big-beat ratio of 1");
    const geiss::BeatState jump = detector.update(0.3f, 1.0 / 60.0);
    check(jump.bigBeatRatio > 1.5f, "a jump in bass is a big beat");
    geiss::BeatState settled;
    for (int i = 0; i < 120; ++i) {
        settled = detector.update(0.3f, 1.0 / 60.0);
    }
    checkNear(settled.bigBeatRatio, 1.f, 0.01f, "a louder level that holds is no longer a big beat");
}

void testSilenceHoldUsesTime() {
    // Overlapping 2048-frame snapshots, one per 1/60 s, as the app sends them.
    const std::vector<float> quiet(std::size_t{2048} * 2, 0.f);
    geiss::PcmBlock block;
    block.samples = quiet.data();
    block.frameCount = 2048;
    block.channels = 2;
    block.sampleRate = 48000;
    geiss::Analyzer analyzer;
    bool silentEarly = false;
    for (int i = 0; i < 24; ++i) {
        block.time = 100.0 + i / 60.0;
        silentEarly = silentEarly || analyzer.analyze(block, 8, 1.f).silent;
    }
    check(!silentEarly, "0.4 s of overlapping quiet blocks is not silence yet");
    bool silentLater = false;
    for (int i = 24; i < 40; ++i) {
        block.time = 100.0 + i / 60.0;
        silentLater = analyzer.analyze(block, 8, 1.f).silent;
    }
    check(silentLater, "0.65 s of quiet is silence");
}

geiss::AudioFrame beatFrame(bool beatMode, float brightness, float ratio) {
    geiss::AudioFrame audio;
    audio.level = 0.5f;
    audio.samples.assign(32, 0.5f);
    audio.beatMode = beatMode;
    audio.beatBrightness = brightness;
    audio.bigBeatRatio = ratio;
    return audio;
}

unsigned colorSum(const geiss::FrameView& frame) {
    unsigned color = 0;
    const std::size_t pixels = static_cast<std::size_t>(frame.width) * static_cast<std::size_t>(frame.height);
    for (std::size_t i = 0; i < pixels; ++i) {
        color += frame.rgba[i * 4] + frame.rgba[i * 4 + 1] + frame.rgba[i * 4 + 2];
    }
    return color;
}

void testBeatBrightness() {
    geiss::Engine dark(1);
    dark.resize(32, 24);
    check(colorSum(dark.tick(1.0 / 30.0, beatFrame(true, 0.f, 0.f))) == 0, "a beat-mode gap draws no line");

    geiss::Engine lit(1);
    lit.resize(32, 24);
    check(colorSum(lit.tick(1.0 / 30.0, beatFrame(true, 1.f, 0.f))) > 0, "a beat-mode hit draws the line");
}

struct Look {
    int mode;
    int wave;
};

Look lookOf(const geiss::Engine& engine) {
    return {engine.mapMode(), engine.waveform()};
}

bool sameLook(Look a, Look b) {
    return a.mode == b.mode && a.wave == b.wave;
}

// Seconds of 0.1 s ticks until the look changes, or -1 within `limit`.
double secondsUntilLookChanges(geiss::Engine& engine, const geiss::AudioFrame& audio, double limit) {
    const Look start = lookOf(engine);
    for (int i = 1; i <= static_cast<int>(limit * 10); ++i) {
        engine.tick(0.1, audio);
        if (!sameLook(lookOf(engine), start)) {
            return i * 0.1;
        }
    }
    return -1;
}

void testLookWaitsForBigBeat() {
    geiss::Engine plain(1);
    plain.resize(16, 12);
    const double withoutBeat = secondsUntilLookChanges(plain, beatFrame(false, 1.f, 0.f), 30);
    check(withoutBeat > 19.9 && withoutBeat < 20.2, "without a beat the look changes at 20 s");

    geiss::Engine waiting(1);
    waiting.resize(16, 12);
    const double noBigBeat = secondsUntilLookChanges(waiting, beatFrame(true, 1.f, 0.5f), 30);
    check(noBigBeat > 24.9 && noBigBeat < 25.2, "with no big beat the look changes when the 5 s wait ends");

    geiss::Engine hit(1);
    hit.resize(16, 12);
    const double bigBeat = secondsUntilLookChanges(hit, beatFrame(true, 1.f, 1.2f), 30);
    check(bigBeat > 19.9 && bigBeat < 20.2, "a big beat changes the look as soon as it is due");

    geiss::Engine steady(1);
    steady.resize(16, 12);
    const double steadyBeat = secondsUntilLookChanges(steady, beatFrame(true, 1.f, 1.05f), 30);
    check(steadyBeat > 21.0 && steadyBeat < 22.0, "a steady beat lands the change after the threshold relaxes");

    geiss::Engine locked(1);
    locked.resize(16, 12);
    locked.setLocked(true);
    check(secondsUntilLookChanges(locked, beatFrame(true, 1.f, 2.f), 30) < 0, "lock holds the look through big beats");
}

}  // namespace

int main() {  // NOLINT(bugprone-exception-escape): a test may abort on a failed allocation
    testMode25PullsTowardCenter();
    testWarpDecayAndSample();
    testAnalysis();
    testEnginePauseDrawsALine();
    testBassLevel();
    testBeatDetector();
    testSilenceHoldUsesTime();
    testBeatBrightness();
    testLookWaitsForBigBeat();
    if (failures != 0) {
        std::cerr << failures << " failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "ok\n";
    return EXIT_SUCCESS;
}
