A window that draws one Geiss zoom and one waveform on Apple Silicon. This build listens to a generated tone. System audio comes later.

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

The window opens at 1280×720. The picture is a smooth zoom of the previous frame, with one line across the middle. The line follows a 220 Hz tone. Choose Silence from the Visual menu to let the picture fade. Pause freezes it. Lock holds the zoom and the color. Next visual picks another zoom. The controls hide after a few seconds and return when the pointer moves.

Sensitivity is remembered. Pause and lock are not.

## License

Copyright (c) 1998-2022 Ryan Geiss. The picture engine and this app are licensed under the 3-Clause BSD License. See [LICENSE](../LICENSE).

## Troubleshooting

| What you see | What to do |
|---|---|
| The window stays black | Wait a moment, choose Visual > Tone, or press Resume if the picture is paused. |
| The line does not move | The preview is set to Silence. Choose Visual > Tone. |
| `xcodebuild` cannot find Metal | Build on an Apple Silicon Mac with Xcode, for the `arm64` destination above. |
| The engine tests fail to configure | Use CMake 3.20 or newer and run the commands from the repository root. |
| The app is not the system-audio visualizer | This build does not capture the Mac's output yet. It plays its own tone. |
