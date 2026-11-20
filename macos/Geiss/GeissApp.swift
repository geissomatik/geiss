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

@main
struct GeissApp: App {
    // Observed so the Visual menu titles follow pause and lock.
    @ObservedObject private var model = VisualizerModel.shared

    var body: some Scene {
        Window("Geiss", id: "geiss") {
            VisualizerView(model: model)
                .frame(minWidth: 640, minHeight: 360)
        }
        .defaultSize(width: 1280, height: 720)
        .commands {
            CommandGroup(replacing: .appInfo) {
                Button("About Geiss") {
                    model.showAbout = true
                }
            }
            CommandMenu("Visual") {
                Button(model.paused ? "Resume" : "Pause") {
                    model.togglePaused()
                }
                .keyboardShortcut(.space, modifiers: [])
                Button(model.locked ? "Unlock" : "Lock") {
                    model.toggleLocked()
                }
                .keyboardShortcut("l", modifiers: [])
                Button("Next Visual") {
                    model.nextVisual()
                }
                .keyboardShortcut("n", modifiers: [])
                Divider()
                Button("System Audio") {
                    model.preview = .system
                }
                Button("Tone") {
                    model.preview = .tone
                }
                .keyboardShortcut("1", modifiers: [.command])
                Button("Silence") {
                    model.preview = .silence
                }
                .keyboardShortcut("2", modifiers: [.command])
            }
        }
    }
}
