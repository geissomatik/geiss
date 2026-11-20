A window that draws a few Geiss maps and waveforms on Apple Silicon. It listens to the sound coming out of this Mac.

[![macOS](https://img.shields.io/badge/macOS-14.2%2B-black)](https://developer.apple.com/documentation/macos-release-notes)
[![architecture](https://img.shields.io/badge/architecture-Apple%20Silicon-black)](https://developer.apple.com/documentation/apple-silicon)
[![license](https://img.shields.io/badge/license-BSD--3--Clause-blue)](https://github.com/janrothen/geiss/blob/main/LICENSE)

# Geiss picture window

## Build

The Windows CMake project at the repository root is unchanged. The picture uses its own builds.

Engine tests:

```console
cmake -S engine -B /tmp/geiss-engine-build
cmake --build /tmp/geiss-engine-build
/tmp/geiss-engine-build/geiss_engine_tests
```

App:

```console
xcodebuild -project macos/Geiss.xcodeproj -scheme Geiss -destination 'platform=macOS,arch=arm64' -derivedDataPath /tmp/geiss-dd build
open /tmp/geiss-dd/Build/Products/Debug/Geiss.app
```

## What you see

The window opens at 1280×720 and asks to record system audio. The picture starts as a smooth zoom with one line across the middle. About every 20 seconds, and whenever you press Next, it picks another look: a horizontal tunnel, a vortex, a fast swirl, or the zoom, with one line, two lines, or an X-Y oscilloscope. The fast swirl is drawn with one line or two. The waveform follows the Mac's output. When music has a steady bass beat, the line flashes on each hit and goes dark between them (the oscilloscope stays steady), and the next look waits a moment to land on a beat. Quiet output leaves the picture calm. Visual > Tone plays a steady note. Visual > Silence fades the waveform. Pause freezes the picture. Lock holds the current look and the color. The controls hide after a few seconds and return when the pointer moves.

The bar reads LIVE while the tap is running, WAIT while it is starting or blocked, TONE for the generated note, and QUIET for silence. Sensitivity is remembered. Pause and lock are not.

## Linting

A pre-commit hook lints the files you commit. clang-tidy checks the C++ in `engine/` and the Objective-C++ bridge, using [`.clang-tidy`](../.clang-tidy). Any finding blocks the commit. SwiftLint checks the Swift in `macos/`, using [`.swiftlint.yml`](../.swiftlint.yml). Only errors block the commit, and warnings are shown. The Windows sources in the repository root are not linted.

```bash
brew install llvm swiftlint pre-commit
pre-commit install
pre-commit run --all-files
```

## License

Copyright (c) 1998-2022 Ryan Geiss. The picture engine and this app are licensed under the 3-Clause BSD License. See [LICENSE](../LICENSE).

## Troubleshooting

| What you see | What to do |
|---|---|
| The window stays black | Wait a moment, or press Resume if the picture is paused. |
| The waveform does not move | The Mac is quiet, or Visual > Silence is selected. Play some sound, or choose Visual > Tone. |
| The line vanishes between beats | That is beat mode with a steady bass beat. It returns on each hit and stays steady in quieter or beatless music. |
| Geiss cannot hear this Mac | Choose Open System Settings and allow Geiss under System Audio Recording. Return to the window. A local rebuild can ask again. |
| Playback stops while Geiss is open | Quit Geiss. The tap is meant to leave playback running. |
| `xcodebuild` cannot find Metal | Build on an Apple Silicon Mac with Xcode, for the `arm64` destination above. |
| The engine tests fail to configure | Use CMake 3.20 or newer and run the commands from the repository root. |
| The commit stops with "clang-tidy not found" | Run `brew install llvm`. The hook finds Homebrew's clang-tidy on its own. |
| SwiftLint lists warnings but the commit goes through | That is expected. Only SwiftLint errors block a commit. |
