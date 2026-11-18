# macOS Port Design Brief

## Product direction

Build a modernized Geiss visualizer for Apple Silicon Macs. Preserve the recognizable visual style while updating the rendering and the macOS experience.

## Initial product shape

- Start as a standalone macOS app.
- Keep the visualization engine as a library with no SwiftUI and no permission code, so a future screen saver can be another host. The app owns audio capture. This prototype does not design the screen saver's permission model.
- On launch, open the visualizer immediately in a normal window on the main display. The picture fills that window. The window opens at 1280×720, is resizable, and has a minimum size of 640×360. Fullscreen uses the system fullscreen control. There is no separate start action.
- Multi-display support can be considered later.
- No music playback controls are needed.

## Platform target

- Apple Silicon only.
- Use macOS 14.2 as the initial minimum deployment target because Core Audio taps are available from macOS 14.2.
- Raise the minimum version if required by the chosen rendering or other APIs.
- No fallback support for older macOS versions is required.
- The app is not sandboxed. Local builds use automatic signing. The bundle identifier is `com.janrothen.geiss`.

## Language and module boundaries

- New visualization and audio-analysis code is portable C++ that follows the original formulas.
- Leave the legacy Windows sources in the tree, and keep them out of the macOS build. The warp in `proc_map.cpp` is MSVC x86 assembly, and the surrounding algorithms sit in Win32 and DirectDraw code.
- SwiftUI provides the window, overlay, and menus.
- Metal presents the frame.
- One Objective-C++ bridge is the only place that includes both Cocoa and the engine headers. Swift does not include C++ headers.
- The simulation buffer stores 8-bit palette indices. Warp, darkening, and the waveform write those indices. The engine expands a 256-color palette to an RGBA8 buffer at the boundary. Swift creates the Metal texture and uploads it. Metal scales that texture into the window.

## Audio behavior

- React to system audio output, including music, notifications, and other sounds.
- Capture with a non-exclusive stereo Core Audio tap of the default output mix (`CATapDescription` and a private aggregate device). Playback continues. Recreate the tap when the default output device changes. If that device tap cannot be created, try one global stereo tap. A failure of both keeps calm motion and shows the settings overlay.
- `NSAudioCaptureUsageDescription` supplies the system prompt. There is no public preflight API for this permission; the prompt appears when the tap starts.
- Split the work into three pieces:
  - Capture, in Swift, copies PCM and returns from the audio callback.
  - Analysis, in portable C++, turns that PCM into one immutable frame: samples, level, peak, beat, silence, and time.
  - The renderer reads only that frame.
- A stalled consumer keeps the latest frame and drops older audio.
- The frame carries one mono sample per simulation column, resampled from the tap rate into about −1…1. Level is the RMS of the captured block. Silence means that level has stayed low for about half a second. A simple rising-edge alignment holds the waveform in place.
- The engine runs on the main thread. The capture callback only copies samples.
- Waveform brightness follows level and sensitivity. The full beat detector waits. The frame may still include a beat flag for later use.
- The exact OS signal for a denied tap is confirmed during the prototype and recorded here.

## Rendering

- Preserve the existing visualization algorithms and their visual intent in portable C++ where practical.
- In the first version, Metal uploads the RGBA8 buffer and scales it. A GPU warp can be considered later if CPU cost calls for it.
- Each tick warps the previous index buffer through the current map, darkens it, draws the waveform, and then expands the palette to RGBA8.
- The prototype palette is one 256-color hue cycle. Its phase turns with time. Audio-linked palettes wait. That difference is noted beside the ported code.
- Presentation follows the display. Simulation time is elapsed seconds. A large gap after sleep or wake is capped so the picture does not jump.
- Simulation resolution is independent of the display. The longest side is capped at 960, and the aspect ratio follows the window.

## Controls and visual design

- Visual direction: modernized interpretation. Keep recognizable Geiss elements — procedural maps, waveforms, rich colors, and evolving layers — while refining rendering, motion, and visual balance.
- Visuals evolve automatically.
- The first control set is:
  - **Sensitivity.** A linear gain on the analyzed level. The default is 1.0, and the value persists. Tune the default once real tap levels are visible.
  - **Pause.** Freezes the picture. This state does not persist.
  - **Lock.** Keeps audio motion and holds the current map, waveform, and palette. This state does not persist.
  - **Next visual.**
- The picture is full-bleed. Vertical size, letterboxing, beat response, palette controls, variation pace, and horizontal shift stay internal until a later version asks for them.

## When audio permission is missing

- Waiting for permission, or granted with sound: the visual reacts to audio.
- Granted and silent: the same motion continues, calm, with no error.
- Denied, unavailable, or the tap fails: calm motion continues, and an overlay explains the state and opens System Settings. Revoking permission at runtime returns to that overlay.

## Prototype milestone

The first build proves the architecture with a small slice:

- Map mode 25, the smooth 1/r zoom in `GenerateChunkOfNewMap`.
- Waveform 1, one smoothed line across the frame.
- A complete map is built when a visual starts. Another is built about every 20 seconds unless the visual is locked. Next builds one immediately. Pause freezes that timer. Chunked background generation waits.
- The index buffer and hue-cycle palette described above, scaled by Metal.
- The window listens to the default output mix. Visual > Tone and Visual > Silence override that source. The tap keeps running during an override.

## Project layout and code policy

- `engine/` holds the portable C++.
- `macos/` holds the Xcode app and the Objective-C++ bridge.
- The root `CMakeLists.txt` remains the Windows build and does not gain a macOS target.
- Once the project exists, build the app with `xcodebuild`. Test analysis and map math with a small C++ executable that uses assertions. The Xcode project is checked in.
- Ported algorithm files keep the Geiss copyright and 3-Clause BSD notice. The SwiftUI shell is separate new code.
- Add unit tests for portable visualization and audio-analysis logic. Check system permission behavior and live rendering manually. Add an automated test when a behavior is stable and testable. Screenshot comparison is not the quality gate.
- Use the existing C++ style only inside preserved legacy files. Use current Swift conventions for new app code. New C++ is a modern portable subset.
- Keep platform APIs at the edges: capture, the bridge, and SwiftUI.
- Name the original function a ported routine follows. Note intentional visual differences beside that code.
- Handle resize, sleep and wake, output-device changes, and permission changes without leaking the tap or jumping the simulation clock.

## Distribution and rights

- Initial use is local. Public distribution and App Store plans are out of scope for now.
- The local app name can be Geiss. An About box carries the existing copyright and 3-Clause BSD notice.
- Review the repository license, attribution requirements, name, and any bundled assets before public distribution.

## Open questions

- Which existing effects and configuration options, beyond the prototype slice, should be preserved, modernized, or left out?
- Which OS error indicates that system-audio recording permission was denied?
- Later decisions: a GPU warp, size and quality settings, presets, multi-display, a menu-bar extra, the screensaver host, sandboxing, and a public name.

## Existing experience inventory (discussion draft)

The Windows project has a substantial set of rendering behaviors and options. Some controls belong to obsolete display or audio hardware, or to Winamp and CD playback. The inventory below records that intent. The first macOS version exposes only the controls in [Controls and visual design](#controls-and-visual-design).

### Visual identity and animation

- **Procedural evolving backgrounds/maps:** Geiss creates a new visual map in chunks while the current one is displayed, then transitions to the new map. This evolving, fluid look is central to the identity. Preserve the feedback warp and automatic change of maps. The first build generates each map completely when the visual changes. Preparing the next map in slices waits.
- **Waveforms:** The project cycles through several waveform styles, including an X-Y oscilloscope mode. Wave shape and amplitude react to the audio. Preserve the recognizable waveform family and audio response. The prototype uses waveform 1, one smoothed line across the frame.
- **Layered effects:** The renderer has optional effects such as chasers, bars, dots, solar-like overlays, grids, nuclide patterns, and shading. Some are selected or varied as part of modes. Preserve the signature effects and their combinations after assessing each for visual quality and cost on modern displays. The prototype leaves these for a later slice.
- **Automatic variation:** Modes, waveforms, palettes, and effects can change over time. Preserve automatic variety. Pause, lock, and next visual are the first-version controls for that behavior.

### Existing configuration intent

- **Vertical visual size / letterboxing:** percentage of display height occupied by the visual. Deferred. The first version is full-bleed.
- **Wave scaling:** default audio waveform amplitude. This is the first-version sensitivity control.
- **Color behavior:** palette generation, gamma, coarse and solar palette frequency, and optional sound-synchronized color. The 8-bit palette-specific controls reflect old color modes. The prototype uses one cycling palette. Palette variety and optional audio-linked color remain later work.
- **Beat detection:** controls beat-responsive behavior. Analysis may still produce a beat flag. A user-facing beat control waits.
- **Automatic map changes:** controls how long maps are generated and shown. The first version evolves automatically. A pace setting waits, and any later control uses seconds.
- **Automatic horizontal shifting:** percentage and frequency, plus a minimum interval between shifts. Keep this only if it remains visually valuable. Any later timing is frame-rate independent.
- **Rendering resolution, color depth, adapter selection, frame idle delay:** Windows and DirectDraw performance controls. The first version uses the capped simulation size in [Rendering](#rendering). A quality setting waits.
- **Help messages, debug info, suppress all messages:** help overlay and diagnostics. Keep diagnostics developer-only. On-screen help in the visualization waits.

### Remove from the macOS product scope

- Winamp integration, track-title overlays, playback controls, and Winamp-specific minimize and flicker options.
- CD playback, CD controls, and CD auto-start.
- Windows sound input device selection and mixer instructions. The target is system output capture with macOS permission handling.
- Windows display adapter selection, video-mode selection, and DirectDraw-specific compatibility options.

### Decisions

- Visual fidelity: modernized interpretation, preserving recognizable Geiss elements while refining presentation.
- Automatic variation: visuals evolve on their own. Pause freezes the picture. Lock holds the current map, waveform, and palette while audio motion continues. Next visual advances the look.
- Initial controls: sensitivity, pause, lock, and next visual.
- Pixels: portable C++ simulates 8-bit palette indices, then expands them to RGBA8. Metal uploads and scales that buffer.
- First slice: map mode 25 (smooth 1/r zoom), waveform 1 (one centered line), and one hue-cycle palette. Each frame feeds back through the map. A new map is built about every 20 seconds unless locked.
- Launch: the visualizer opens immediately. Denied or failed capture keeps calm motion and shows an overlay that opens System Settings.

### Choices considered

1. **How faithful should the visuals be?**
   - *Close recreation:* retain the original compositions, palettes, and motion as closely as the new renderer allows.
   - *Modernized interpretation (chosen):* keep recognizable waveforms, procedural maps, palette richness, and evolving layers, while improving smoothness, resolution handling, and visual balance.
   - *Selective inspiration:* retain only a small set of recognizable motifs and redesign the rest.
2. **How much control should the first app expose?**
   - *Translate useful controls:* keep size, sensitivity, beat response, sound-linked color, and variation pace, and fold obsolete palette-era controls into defaults.
   - *Minimal controls (chosen):* sensitivity, pause, lock, and next visual. Size waits with the other legacy settings.
   - *Parity-oriented controls:* expose nearly every visualization setting that still has a meaningful modern equivalent.
3. **How should automatic variation work?**
   - *Chosen:* visuals evolve automatically. Pause and lock are separate actions, and next visual is available.
   - A calmer default that stays on one look until the user asks it to change.
