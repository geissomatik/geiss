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

import SwiftUI

struct VisualizerView: View {
    @ObservedObject var model: VisualizerModel
    @State private var controlsVisible = true
    @State private var hideTask: Task<Void, Never>?

    private let amber = Color(red: 0.90, green: 0.63, blue: 0.36)

    var body: some View {
        GeometryReader { geo in
            ZStack(alignment: .bottom) {
                MetalScreen(presenter: model.presenter)
                    .ignoresSafeArea()
                if let info = model.info {
                    infoOverlay(info)
                        .padding(.top, 16)
                        .padding(.leading, 16)
                        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .topLeading)
                        .allowsHitTesting(false)
                }
                if case .failed(let status) = model.capture, model.preview == .system {
                    captureOverlay(status: status)
                        .padding(.top, 28)
                        .frame(maxHeight: .infinity, alignment: .top)
                }
                controlBar
                    .padding(.horizontal, 18)
                    .padding(.bottom, 16)
                    .opacity(controlsVisible ? 1 : 0)
                    .allowsHitTesting(controlsVisible)
            }
            .background(Color.black)
            .contentShape(Rectangle())
            .onContinuousHover { phase in
                if case .active = phase {
                    revealControls()
                }
            }
            .onTapGesture {
                revealControls()
            }
            .onAppear {
                model.tick(to: .now, viewSize: geo.size)
                revealControls()
            }
            .onChange(of: geo.size) { _, _ in
                model.tick(to: .now, viewSize: geo.size)
            }
            .background {
                TimelineView(.animation(minimumInterval: 1.0 / 60.0)) { timeline in
                    Color.clear
                        .onChange(of: timeline.date) { _, date in
                            model.tick(to: date, viewSize: geo.size)
                        }
                }
            }
        }
        .focusable()
        // Space, L, N, and I are menu shortcuts in the Visual menu. The right arrow is extra.
        .onKeyPress(.rightArrow) {
            model.nextVisual()
            return .handled
        }
        .sheet(isPresented: $model.showAbout) {
            AboutView()
        }
        .preferredColorScheme(.dark)
    }

    private var controlBar: some View {
        HStack(spacing: 14) {
            Text(model.sourceLabel)
                .foregroundStyle(amber)
            Text("SENS")
            Slider(value: $model.sensitivity, in: 0.25...4)
                .frame(width: 120)
                .tint(amber)
            barButton(model.paused ? "Resume" : "Pause", action: model.togglePaused)
            barButton(model.locked ? "Unlock" : "Lock", action: model.toggleLocked)
            barButton("Next", action: model.nextVisual)
        }
        .font(.system(size: 12, weight: .medium, design: .monospaced))
        .foregroundStyle(.white.opacity(0.88))
        .padding(.horizontal, 14)
        .padding(.vertical, 9)
        .background(Color.black.opacity(0.62))
        .overlay(alignment: .top) {
            Rectangle().fill(amber.opacity(0.9)).frame(height: 1)
        }
    }

    private func infoOverlay(_ info: InfoSnapshot) -> some View {
        Grid(alignment: .leading, horizontalSpacing: 10, verticalSpacing: 3) {
            infoRow("FPS", "\(info.fps)")
            infoRow("MAP", info.map)
            infoRow("WAVE", info.wave)
            infoRow("FX", info.effects)
        }
        .font(.system(size: 12, weight: .medium, design: .monospaced))
        .foregroundStyle(.white.opacity(0.88))
        .padding(.horizontal, 12)
        .padding(.vertical, 8)
        .background(Color.black.opacity(0.62))
        .overlay(alignment: .leading) {
            Rectangle().fill(amber.opacity(0.9)).frame(width: 1)
        }
    }

    private func infoRow(_ label: String, _ value: String) -> some View {
        GridRow {
            Text(label).foregroundStyle(amber)
            Text(value)
        }
    }

    private func captureOverlay(status: OSStatus) -> some View {
        VStack(alignment: .leading, spacing: 8) {
            Text("Geiss cannot hear this Mac.")
            Text(SystemAudioTap.statusText(status))
                .foregroundStyle(amber)
            Button("Open System Settings", action: model.openCaptureSettings)
                .buttonStyle(.plain)
                .padding(.horizontal, 8)
                .padding(.vertical, 4)
                .contentShape(Rectangle())
        }
        .font(.system(size: 12, weight: .medium, design: .monospaced))
        .foregroundStyle(.white.opacity(0.88))
        .padding(.horizontal, 14)
        .padding(.vertical, 10)
        .background(Color.black.opacity(0.72))
        .overlay(alignment: .top) {
            Rectangle().fill(amber.opacity(0.9)).frame(height: 1)
        }
    }

    private func barButton(_ title: String, action: @escaping () -> Void) -> some View {
        Button(title, action: action)
            .buttonStyle(.plain)
            .padding(.horizontal, 8)
            .padding(.vertical, 4)
            .contentShape(Rectangle())
    }

    private func revealControls() {
        controlsVisible = true
        hideTask?.cancel()
        hideTask = Task {
            try? await Task.sleep(nanoseconds: 3_500_000_000)
            guard !Task.isCancelled else { return }
            controlsVisible = false
        }
    }
}

private struct AboutView: View {
    @Environment(\.dismiss) private var dismiss

    var body: some View {
        VStack(alignment: .leading, spacing: 14) {
            Text("Geiss")
                .font(.system(size: 28, weight: .semibold, design: .monospaced))
            Text("A sound-reactive picture. It follows the sound coming out of this Mac.")
                .fixedSize(horizontal: false, vertical: true)
            Text("Copyright © 1998-2022 Ryan Geiss. Licensed under the 3-Clause BSD License. The copyright notice and license terms ship with the source.")
                .font(.callout)
                .foregroundStyle(.secondary)
                .fixedSize(horizontal: false, vertical: true)
            HStack {
                Spacer()
                Button("Close") { dismiss() }
                    .keyboardShortcut(.cancelAction)
            }
        }
        .padding(24)
        .frame(width: 440)
    }
}
