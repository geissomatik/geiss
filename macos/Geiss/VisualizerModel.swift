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

import AppKit
import SwiftUI

enum PreviewSource: String {
    case system
    case tone
    case silence
}

@MainActor
final class VisualizerModel: ObservableObject {
    static let shared = VisualizerModel()

    let presenter = MetalPresenter()
    @Published var paused = false
    @Published var locked = false
    @Published var showAbout = false
    @Published var sensitivity: Double {
        didSet {
            let clamped = min(4, max(0.25, sensitivity))
            if clamped != sensitivity {
                sensitivity = clamped
                return
            }
            UserDefaults.standard.set(clamped, forKey: Self.sensitivityKey)
            bridge.setSensitivity(Float(clamped))
        }
    }
    @Published var preview: PreviewSource = .system
    @Published var capture: CaptureStatus = .starting

    private let bridge = GeissBridge(seed: 1)
    private let tap = SystemAudioTap()
    private var lastDate: Date?
    private var lastSequence: UInt64 = 0
    private var lastCaptureDate: Date?
    private var simWidth = 0
    private var simHeight = 0
    private var silenceBuffer: [Float] = []
    private var captureBuffer: [Float] = []

    private static let sensitivityKey = "sensitivity"

    var sourceLabel: String {
        switch preview {
        case .tone:
            return "TONE"
        case .silence:
            return "QUIET"
        case .system:
            if case .running = capture {
                return "LIVE"
            }
            return "WAIT"
        }
    }

    private init() {
        let stored = UserDefaults.standard.object(forKey: Self.sensitivityKey) as? Double ?? 1
        sensitivity = min(4, max(0.25, stored))
        bridge.setSensitivity(Float(sensitivity))
        // The tap delivers statuses on the main queue, in order.
        tap.onStatus = { [weak self] status in
            MainActor.assumeIsolated {
                self?.capture = status
            }
        }
        tap.activate()
    }

    func openCaptureSettings() {
        let candidates = [
            "x-apple.systempreferences:com.apple.settings.PrivacySecurity.extension?Privacy_AudioCapture",
            "x-apple.systempreferences:com.apple.preference.security?Privacy_AudioCapture"
        ]
        for candidate in candidates {
            guard let url = URL(string: candidate) else { continue }
            if NSWorkspace.shared.open(url) {
                return
            }
        }
    }

    func togglePaused() {
        paused.toggle()
        bridge.setPaused(paused)
    }

    func toggleLocked() {
        locked.toggle()
        bridge.setLocked(locked)
    }

    func nextVisual() {
        bridge.nextVisual()
    }

    func tick(to date: Date, viewSize: CGSize) {
        let size = Self.simulationSize(for: viewSize)
        if size.width != simWidth || size.height != simHeight {
            simWidth = size.width
            simHeight = size.height
            bridge.resizeWidth(Int32(size.width), height: Int32(size.height))
        }

        let dt = cappedDelta(to: date)
        let time = date.timeIntervalSinceReferenceDate
        switch preview {
        case .tone:
            let samples = PreviewTone.samples(at: time, duration: dt)
            samples.withUnsafeBufferPointer { buffer in
                bridge.ingestInterleaved(
                    buffer.baseAddress,
                    frameCount: Int32(samples.count / 2),
                    channels: 2,
                    sampleRate: PreviewTone.sampleRate,
                    time: time
                )
            }
        case .silence:
            ingestSilence(dt: dt, time: time)
        case .system:
            ingestSystem(dt: dt, time: time)
        }

        var width: Int32 = 0
        var height: Int32 = 0
        let pixels = bridge.tick(dt, width: &width, height: &height)
        presenter.upload(pixels: pixels, width: Int(width), height: Int(height))
    }

    static func simulationSize(for viewSize: CGSize) -> (width: Int, height: Int) {
        let longest = max(viewSize.width, viewSize.height, 1)
        let scale = min(1, 960 / longest)
        return (
            max(2, Int((viewSize.width * scale).rounded())),
            max(2, Int((viewSize.height * scale).rounded()))
        )
    }

    private func ingestSystem(dt: Double, time: Double) {
        let running: Bool
        if case .running = capture {
            running = true
        } else {
            running = false
        }
        if running, let snap = tap.latest(into: &captureBuffer), snap.sequence != lastSequence {
            lastSequence = snap.sequence
            lastCaptureDate = Date()
            captureBuffer.withUnsafeBufferPointer { buffer in
                bridge.ingestInterleaved(
                    buffer.baseAddress,
                    frameCount: Int32(snap.frames),
                    channels: 2,
                    sampleRate: snap.sampleRate,
                    time: time
                )
            }
            return
        }
        let age = lastCaptureDate.map { Date().timeIntervalSince($0) } ?? 1
        if !running || age > 0.2 {
            ingestSilence(dt: dt, time: time)
        }
    }

    private func ingestSilence(dt: Double, time: Double) {
        let frames = max(1, Int(PreviewTone.sampleRate * dt))
        if silenceBuffer.count != frames * 2 {
            silenceBuffer = [Float](repeating: 0, count: frames * 2)
        }
        silenceBuffer.withUnsafeBufferPointer { buffer in
            bridge.ingestInterleaved(
                buffer.baseAddress,
                frameCount: Int32(frames),
                channels: 2,
                sampleRate: PreviewTone.sampleRate,
                time: time
            )
        }
    }

    private func cappedDelta(to date: Date) -> Double {
        defer { lastDate = date }
        guard let lastDate else { return 1.0 / 60.0 }
        let gap = date.timeIntervalSince(lastDate)
        if gap <= 0 || gap > 0.5 {
            return 1.0 / 60.0
        }
        return min(gap, 0.1)
    }
}

enum PreviewTone {
    static let sampleRate = 48_000.0

    static func samples(at time: Double, duration: Double) -> [Float] {
        let frames = max(1, Int(sampleRate * duration))
        var samples = [Float](repeating: 0, count: frames * 2)
        let frequency = 220.0
        for index in 0..<frames {
            let sampleTime = time + Double(index) / sampleRate
            let value = Float(sin(sampleTime * frequency * 2 * .pi) * 0.65)
            samples[index * 2] = value
            samples[index * 2 + 1] = value
        }
        return samples
    }
}
