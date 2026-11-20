# Project Guidance

## Product direction

This project includes a modernized Geiss visualizer for Apple Silicon Macs, alongside the original Windows screensaver and Winamp plug-in. The macOS product is a standalone app. Keep the visualization engine independent of the app shell so it can potentially be reused by a macOS screen saver later.

Treat [`docs/spec/macos-port.md`](docs/spec/macos-port.md) as the current product brief. Update it when the user makes a design decision. When guidance here and the spec diverge, follow the latest user decision and reconcile the documents.

## Agreed product decisions

- Target Apple Silicon only.
- Start with macOS 14.2 as the minimum deployment target; raise it if required by the chosen rendering or other APIs. No fallback for older macOS versions is required.
- React to system output, including music, notifications, and other sounds. Music playback controls and player-specific integration are out of scope.
- Start as a standalone app and visualize on the main display. Keep a future screen saver path viable; multi-display behavior can be considered later.
- Use a modernized interpretation of Geiss: retain recognizable procedural maps, waveforms, rich colors, layered effects, and evolving visuals while refining rendering, motion, and visual balance.
- Visuals evolve automatically. Pause freezes the picture. Lock holds the current map, waveform, and palette while audio motion continues. Next visual advances the look.
- The first controls are sensitivity (persisted), pause, lock, and next visual. Size and the other legacy settings wait. Do not add visualizer controls without user direction.
- Layered effects draw only while there is sound. In silence, and when capture is denied or fails, they rest until sound returns.
- On launch, open the visualizer immediately. When audio permission is missing, denied, or the tap fails, keep a calm visual and show an overlay that opens System Settings. Silence with permission granted shows the calm visual and no error.
- Initial distribution is local. Before public distribution, review applicable license and attribution requirements, bundled assets, and use of the Geiss name.

## Current build

`macos/` is the SwiftUI app. `engine/` is the portable C++ picture. The root CMake project remains the Windows screensaver and Winamp plug-in. Build steps for the app are in [`macos/README.md`](macos/README.md).

The app opens straight into a window on the main display. The window starts at 1280×720, can shrink to 640×360, and uses the system fullscreen control. The bundle identifier is `com.janrothen.geiss`. Local builds are ad-hoc signed and are not sandboxed. The minimum system is macOS 14.2 on Apple Silicon.

The picture keeps the same warp and has all 25 of the original's map modes (`engine/src/map.cpp`) and all 6 waveforms. Looks are picked with the original's weights and excluded pairs; the spec's Prototype milestone section lists each formula and every intentional difference. Each look also picks the original's layered effects (`engine/src/effects.cpp`): chasers, bar, dots, solar, grid, nuclide, and shade bobs, with its per-map weights, plus each map's center darkening. The nuclide can replace the waveform, and mode 5's map follows it. Effects rest in silence. The window opens on mode 25 and waveform 1. Next, and the 20-second timer, pick a different pair and new effects. Lock holds the pair and its effects. Pause freezes the picture and that timer. The simulation stores 8-bit palette indices, expands one hue-cycle palette with a white-hot top to RGBA8, and Metal uploads that buffer. The longest simulation side is 960. Each frame warps the previous frame through the map and darkens it.

The app taps the default output mix. Playback continues. The audio callback only copies the 32-bit float tap stream into a preallocated ring. It never waits on a lock; it drops a block if the reader is copying. A tap with no callbacks for three seconds is restarted. The C++ analyzer turns those samples into one frame, and the engine reads that frame on the main thread. The tap is recreated when the default output device changes, stopped on sleep, and started again on wake. Visual > Tone and Visual > Silence override the live mix while the tap keeps running. A failed tap keeps a calm picture and shows an overlay that opens System Settings. The OSStatus for a denied tap has not been observed yet, so leave that line in the spec until a real code appears. Whether a revoked tap stops its callbacks or delivers silence is also still open.

The window controls are sensitivity (persisted, default 1.0), pause, lock, and next visual. The bar hides after a few seconds. Space pauses, L locks, and N or the right arrow advances the look.

Beats come from a bass-focused port of the original detector (`engine/src/beat.cpp`). In beat mode the waveform flashes white on hits, except the oscilloscope, and a due look waits up to 5 seconds for a big beat. Lock and Next are unchanged. The spec's Beat response section has the details.

Still ahead: the horizontal slide shift on beats, palette variety, chunked map generation, a GPU warp, size and quality settings, presets, multi-display, a screen saver host, sandboxing, and public distribution wait.

## Porting guidance

- Preserve the existing visualization algorithms and their intent where practical, in new portable C++. The simulation buffer is 8-bit palette indices; the engine expands a palette to RGBA8 and Metal uploads that. The current formulas are all 25 map modes, all 6 waveforms, and 7 layered effects. Each frame feeds the previous frame back through the map. A new pair is chosen about every 20 seconds unless locked. Treat the DirectDraw and software-pixel path, including the x86 assembly warp, as an implementation detail to replace. Keep that engine independent of the app shell for possible future screen saver reuse. The legacy Windows sources stay out of the macOS build.
- SwiftUI owns the window and controls. A small Objective-C++ bridge is the only file that includes both Cocoa and the engine.
- Replace Windows-specific platform, audio, and UI layers with suitable macOS components.
- Do not carry over Winamp integration, CD playback/control, Windows mixer/device selection, DirectDraw display-mode controls, or other Windows-only compatibility behavior into the macOS product.
- Avoid treating old frame counts, color-depth assumptions, or hardware workarounds as product requirements. Use frame-rate-independent timing and modern display capabilities where appropriate.
- Keep system-audio capture and visualization processing decoupled from the app UI. Make permission state and capture failures understandable to the user.
- Preserve existing source and assets unless the user asks to remove them or the port plan explicitly makes them obsolete.

## Working approach

- During brainstorming and specification work, do not implement application code unless the user explicitly asks to begin implementation.
- Keep changes focused on the requested task.
- Always add tests for new code in the same change. Engine code gets assertions in `engine/tests/`. App, bridge, and capture code without an automated test harness gets a manual check in the PR description instead, until such a harness exists.
- Before every commit, build and run the engine tests (see [`macos/README.md`](macos/README.md)) and commit only when they pass.
- After changing the app, build it and launch it so the user can test it by hand.
- Keep `engine/`, the Objective-C++ bridge, and `macos/` clean under the pre-commit hooks (`pre-commit run --all-files`): clang-tidy per [`.clang-tidy`](.clang-tidy) and SwiftLint per [`.swiftlint.yml`](.swiftlint.yml). Fix findings rather than adding `NOLINT`, unless the check is wrong for that line, and say why in the comment. The legacy Windows sources are not linted.
- Explain important design assumptions and unresolved product questions in the spec rather than silently deciding them.
