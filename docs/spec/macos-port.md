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
  - Capture, in Swift, copies PCM and returns from the audio callback. The callback converts the tap's 32-bit float stream to stereo straight into a preallocated ring. It does not allocate, wait on a lock, or log. If the reader is copying from the ring at that moment, the callback drops that block. Other tap formats are reported as a capture failure.
  - Analysis, in portable C++, turns that PCM into one immutable frame: samples, level, peak, volume and its average, silence, time, and the beat state (bass, beat mode, beat brightness, and big-beat ratio).
  - The renderer reads only that frame.
- A stalled consumer keeps the latest frame and drops older audio.
- The frame carries one mono sample per simulation column, and a left and a right sample on that same grid, resampled from the tap rate into about −1…1. A one-channel block copies its sample into both sides. Level is the RMS of the mono samples. Silence means that level has stayed low for about half a second of wall time, measured between analyzed blocks rather than from their length, because successive snapshots of the ring overlap. One rising-edge alignment rotates the mono, left, and right columns together so the waveform holds in place.
- The engine runs on the main thread. The capture callback only copies samples.
- Waveform brightness follows RenderWave: `base = volume*4 + averageVolume*0.4 - 10`, clamped to 0…155, where volume is the peak-to-peak of the newest 1/30 s on the original's 0…256 scale (after sensitivity) and the average smooths it like `avg_vol`. That range maps onto the full palette, so normal listening levels draw at full brightness and quiet passages dim. Beats scale it further when a beat is on (see [Beat response](#beat-response)).

### Beat response

- Detection is bass-focused. Two one-pole low-pass stages at 150 Hz run across each block, and the bass level is the RMS of its newest 1/30 s. Intentional difference: the original measured the composite volume.
- The detector ports the original's beat logic (`main.cpp`: beat mode, the beat-mode brightness, and `bBigBeat`). A smoothed bass level feeds a history of 120 samples written at a fixed 30 per second of elapsed time, so frame rate and overlapping blocks do not change it.
- Beat mode turns on when that history is evenly bumpy, using the original's strength measure and its 109/71 hysteresis. A quiet history (smoothed bass below 0.01 RMS) is never in beat mode. A steady tone, hi-hats alone, or a sustained bass note stay out of beat mode.
- In beat mode, the waveform brightness scales from 0 at the average bass to full at half a standard deviation above it, so the line flashes on hits and goes dark between them. A hit draws at the top of the palette, which is white-hot, so it stands out from the glowing trails. The X-Y oscilloscope keeps its steady brightness, as in the original.
- When the 20-second timer is due in beat mode, the new look waits for a big beat: the smoothed bass over the loudest of the previous 4/3 s, against a threshold that starts at 1.10. Intentional difference: the threshold relaxes by 0.2 over a 5-second wait instead of the original's 18 seconds, and the look changes when the wait ends. A steady beat therefore lands the change on a hit after about 1.3 s, and a louder hit lands it at once.
- Lock still holds the look. Next still switches at once. Pause freezes the timer and the wait.
- The horizontal slide shift on beats waits. There is no beat control.
- The exact OS signal for a denied tap is confirmed during the prototype and recorded here.

## Rendering

- Preserve the existing visualization algorithms and their visual intent in portable C++ where practical.
- In the first version, Metal uploads the RGBA8 buffer and scales it. A GPU warp can be considered later if CPU cost calls for it.
- Each tick draws the look's effects into the previous index buffer, warps it through the current map, darkens it, draws the waveform, and then expands the palette to RGBA8.
- The prototype palette is one 256-color hue cycle. Its phase turns with time. The top 15% of indices blends from the hue toward white, eased in, so a freshly drawn full-brightness line glows white and fades back into the hue within about 0.7 s at the original decay rate. Audio-linked palettes wait. That difference is noted beside the ported code.
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
- A running tap that delivers no callbacks for three seconds is restarted, and a restart that cannot create the tap shows the overlay. Whether a revoked tap stops its callbacks or keeps delivering silence has not been observed yet. If it delivers silence, the app cannot tell revocation from a quiet Mac, and the picture stays calm without the overlay.
- Returning to the app retries a failed tap. The overlay stays up during that retry.

## Prototype milestone

The picture has all 25 of the original's map modes and all 6 of its waveforms. The warp is unchanged: each frame samples the previous index buffer through the map, darkens it, and draws the waveform.

Maps, from `GenerateChunkOfNewMap`. `dx` and `dy` are the pixel offset from the map center. Most modes compute a scale and read from the center plus that offset, rotated by the map's turn and multiplied by the scale. `r640` is the offset's length on a 640-wide frame, as the original's `rmult` made it, so a mode keeps its look at any simulation size. Modes 17 and up use `nx` and `ny`, the offset times `2 / width`. The [glossary](../glossary.md) describes each map and effect in plain words.

- 1: constant zoom, a constant scale of 0.985 − 0.12·u². 2: outward zoom, a constant scale of 1.00…1.02. 3: terra-landing, `0.95 − dy·(480/H)·0.0005`. 4: a sphere, `0.9 + r640·0.0025·0.14`. 5: super-perspective, `f2 − f1·r640/200·1.7` with the nuclide effect, `f2 − f1·√(r640/200)` without.
- 6: a vortex field. Five push centers each push, or swirl one way or the other, weighted by `1 / (d² + 0.1)`, normalized to 1.9 pixels with a small drift. Distances and the move use the 640-wide reference.
- 7: fuzzy, `f1 − r640·f2` plus a fixed per-pixel jitter of 0…0.0495. 8: ripples, `0.85 + 0.1·sin(√r640·f1)`. 9: flower petals, `f1 − r640·f2` on even pixels and a constant `scale2` on odd pixels.
- 10: a scroll. Rows read from `y·1.04`, and columns stretch from the center by `1.03 + 0.03·y/H`. 11: two rotations, each with its own scale, dithered by pixel parity. 12: a sideways splitter. Columns read from `±√|dx|` near the centerline (on the 640 reference), and the picture fades faster (0.98 per frame).
- 13: a black hole, `(1.04 − r640^1.5·0.00025·0.14 − 1)·f1 + 1`. 14: a split world, `0.9 + 0.2·cos(dy·12/H)`. 15: radial petals, `f2 + f3·sin(angle·f1)` with 2…6 petals. 16: a crystal ball, `1.05 − r640²·0.00025·0.09`, at least −1.5, so far pixels read from the other side.
- 17: a horizontal tunnel, `0.97 − ny²·0.40`. 18: a vertical tunnel, `0.97 − nx²·0.40`. 19: a vortex, `1.04 − 0.25·length`. 20: terra, `1.15 − √(ny + 1.4)·0.20`. 21: diced cubes, `0.95 − ⌊|nx|·10⌋·0.03 − ⌊|ny|·10⌋·0.03`. 22: phonic rings, `0.95 − ⌊length·10⌋·0.04`. 23: fast rings, `0.95 − (⌊length·20⌋ mod 4)·0.12`. 24: a fast swirl, scale 0.96 and turn 0.05. 25: the smooth zoom, `zoom / (zoom + length)` with zoom 2…6.

Each new map picks its center, turn, and constants as `GenerateChunkOfNewMap` did: per-mode turns, a random rotation direction, and turns scaled by 0.6. Modes 1, 2, 4, 5, and 7…16 move half as far per frame, as `mode_motion_dampened` made them. The original also scaled that by `suggested_damping`, a measure of how much the music was changing; that stays at full here.

Intentional differences:
- The original shrank the motion of modes 5, 7, and 9 above 640 pixels wide (`protective_factor`). Here every mode keeps its 640-wide look at any size.
- Mode 7's jitter comes from a hash per pixel and map, not the original's shared random table.
- Mode 14 leaves out a jitter of under 1/1024 in the height.
- Mode 20 reads a negative root above the frame as zero.
- Mode 5 falls off with `r·1.7` when its look has the nuclide effect and with `√r` when it does not. The original read the nuclide state of the previous look, because it built the next map while the current one played. Here it follows the new look.
- Modes 7 and 9 stop their per-pixel scale at 0. On frames much taller than wide, `r640` reaches radii the 640×480 original never did, and `f1 − r640·f2` would turn negative and flip the corners through the center.

Kept as in the original:
- A source point past the left or right edge wraps around (`newx += FXW-1` while below 0), so maps that read beyond the sides, such as 2, 10, and 16, pull in the opposite edge. Rows clamp.
- Mode 24's pixel loop sets `turn1 = 0.05`, so the random rotation direction does not reach it and every fast swirl turns the same way.

Waveforms, from `RenderWave`. A −1…1 sample covers about 35% of the height before each waveform's own scale.

- 1: one smoothed line across the frame center. Over mode 10's scroll it sits at three quarters of the height, as in the original.
- 2: two smoothed lines, left 12% of the height above the map center and right 12% below, at 0.7 of the amplitude.
- 3: one smoothed vertical line through the map center. Row y reads the sample of column y.
- 4: two smoothed diagonal lines at 0.9 of the amplitude, left from the top-left corner and right from `width − height`.
- 5: a circle around the map center. Its radius is 60 pixels on a 640-wide frame plus the wave at 0.7 of the amplitude. The first 50 points blend toward the points after 314, so the circle closes.
- 6: the X-Y oscilloscope. It plots 314 smoothed stereo points around the map center at 1.2 of the amplitude, rocked by `sin(t * 0.3)`.

Intentional difference: the original walked the first frames of its capture buffer for waveforms 5 and 6. Here those points spread across the whole column wave.

Picking follows the original:
- A mode is chosen uniformly from 1…25, then has a 1-in-25 chance each of becoming fuzzy (7) or super-perspective (5).
- A waveform is chosen with 1…5 at 3/17 each and 6 at 2/17.
- The waveform is re-chosen while the pair is excluded: mode 6 with waveform 5, mode 12 with waveforms 4 or 6, mode 14 with waveforms 3 or 4, and modes 8, 23, or 24 with waveform 6.
- Next and the timer always change the pair.

### Layered effects

Seven of the original's overlays are ported from `Effects.h` and `main.cpp`, using the original's 8-bit drawing, because the simulation stores palette indices. They draw into the previous frame before the warp, as the original drew into `VS1`, so the map smears them into trails. Sizes and positions are for a 640-wide frame and scale with the width. Time is counted in reference frames at 30 fps, so speeds do not depend on the display's refresh rate. Effects draw only while there is sound. In silence, and while capture is denied or failed, they rest until sound returns, so the picture stays calm.

- **Chasers:** one or two points on two-cosine curves around the map center. Each frame they take 20 steps (at 640 wide), and each step lifts its pixel 40% of the way to the top. The steps sit on a fixed grid of time, so the path is the same at any frame rate.
- **Bar:** a short line between two endpoints that wander near the center. Each frame it adds 16 to every pixel on it below 223. Between frames it adds its share.
- **Dots:** a slow point leaves a trail of its last 20 positions at a value that cycles with time. Every dot drifts one pixel right each frame.
- **Solar:** particles land within 35 pixels of the center, each with a 3×3 glow that is brighter near the center. A pixel at 207 or above takes none. The count swings with time and with the mode's strength (`solar_max`, from the original's 8-bit table). Wider frames get more particles, so the density stays the same. A new mode 1 look opens with a burst of 2000 particles half the time, whether or not Solar is on. A look that opens in silence loses its burst.
- **Grid:** a dot every 1/30 of the width in both directions. It scrolls one pixel a frame, left or right, and its brightness pulses. It never darkens a pixel.
- **Nuclide:** a ring of 3…7 soft discs, 34…41 pixels from the center. With sound, a jump in volume (above 1.1 times the original's narrow average, which takes 0.7 of each new frame) drops a ring whose disc size follows the jump. In 4 looks out of 7 the nuclide replaces the waveform, except over mode 10, where the original set the waveform again afterwards.
- **Shade bobs:** one bob wobbles a few pixels around the center. It takes four jittered steps a frame, each adding 2 to its pixel and 1 to the four beside it, up to 250.

Each map also darkens its center every frame (`center_dwindle`), so the zoom does not burn a hot spot there: 0.99 for modes 3 and 5, 0.98 for 4, 13, 14, 16, and 20…23, 0.985 for 7 and 9, 0.96 for 8, and 0.915 for 12. Mode 12 darkens a three-pixel column, since it splits sideways from a vertical line. The others darken the center pixel and its four neighbors.

Picking follows `CModeInfo` and `Clip_Num_Effects`:
- Each mode gives each effect a weight out of 1000. The weights use the original's truecolor adjustment: nuclide × 1.3, chasers − 50, dots and bar + 220, shade + 150, each kept within 0…900, and grid + 8.
- Each effect gets one roll. While sound is playing, every weight is multiplied by 0.7.
- Without sound, effects are added until the mode's minimum is met. Then effects are dropped at random down to the mode's maximum. Most modes have a minimum of 1 and a maximum of 2. Mode 2 allows up to 5, and modes 17 and up 1…3. Modes 10, 11, 12, and 15 have no minimum, so their looks can have no effects; their maximums are 2, 4, 2, and 1. While sound is playing the minimum is not enforced, so a look on any mode can open with no effects.
- Chasers come as one or two. A grid turns the bar off. The grid's direction is random.
- Effects are part of the look. Lock holds them. Next and the 20-second timer pick a new set for the new map. Pause freezes them with the picture.

Intentional differences:
- The original's weight table had a ninth slot that no mode set. It defaulted to 111 and could take up a place without drawing anything. It is left out.
- Where no effect left could come up, the original's minimum loop would hang. Here it stops.
- The original drew the nuclide's colors and the chasers' per-channel tints only in truecolor. The index buffer keeps one value per pixel, so the palette colors them.
- The original turned its dots and grid points into 2×2 blocks above about 1000 pixels wide. The simulation stops at 960, so they stay one pixel.
- Effects rest in silence. The original kept drawing them, and the nuclide dropped a random ring about one frame in 12. The user chose a calm picture in silence and when capture is denied, so those rings are left out. The center darkening is part of the map and continues.
- Spectral dots (weight 0 everywhere) and the commented-out edge, neutron, and crystal effects are not ported.

The window opens on mode 25 and waveform 1, with its own effects. Another pair is chosen about every 20 seconds unless the visual is locked. Next chooses one immediately. Pause freezes the picture and that timer. The oscilloscope rock advances while the picture is moving, including while locked, and holds while paused. Chunked background generation waits.

The index buffer and hue-cycle palette are unchanged. The window listens to the default output mix. Visual > Tone and Visual > Silence override that source. The tap keeps running during an override.

## Project layout and code policy

- `engine/` holds the portable C++.
- `macos/` holds the Xcode app and the Objective-C++ bridge.
- The root `CMakeLists.txt` remains the Windows build and does not gain a macOS target.
- Once the project exists, build the app with `xcodebuild`. Test analysis and map math with a small C++ executable that uses assertions. The Xcode project is checked in.
- Ported algorithm files keep the Geiss copyright and 3-Clause BSD notice. The SwiftUI shell is separate new code.
- Add unit tests for all new portable visualization and audio-analysis code in the same change. Check system permission behavior and live rendering manually. Add an automated test when a behavior is stable and testable. Screenshot comparison is not the quality gate.
- Use the existing C++ style only inside preserved legacy files. Use current Swift conventions for new app code. New C++ is a modern portable subset.
- Keep platform APIs at the edges: capture, the bridge, and SwiftUI.
- Name the original function a ported routine follows. Note intentional visual differences beside that code.
- Handle resize, sleep and wake, output-device changes, and permission changes without leaking the tap or jumping the simulation clock.

## Distribution and rights

- Initial use is local. Public distribution and App Store plans are out of scope for now.
- The local app name can be Geiss. An About box carries the existing copyright and 3-Clause BSD notice.
- Review the repository license, attribution requirements, name, and any bundled assets before public distribution.

## Open questions

- Which existing configuration options, beyond the prototype slice, should be preserved, modernized, or left out?
- Which OS error indicates that system-audio recording permission was denied?
- Does a tap whose permission is revoked at runtime stop its callbacks, or keep delivering silence?
- Later decisions: a GPU warp, size and quality settings, presets, multi-display, a menu-bar extra, the screensaver host, sandboxing, and a public name.

## Existing experience inventory (discussion draft)

The Windows project has a substantial set of rendering behaviors and options. Some controls belong to obsolete display or audio hardware, or to Winamp and CD playback. The inventory below records that intent. The first macOS version exposes only the controls in [Controls and visual design](#controls-and-visual-design).

### Visual identity and animation

- **Procedural evolving backgrounds/maps:** Geiss creates a new visual map in chunks while the current one is displayed, then transitions to the new map. This evolving, fluid look is central to the identity. Preserve the feedback warp and automatic change of maps. The first build generates each map completely when the visual changes. Preparing the next map in slices waits.
- **Waveforms:** The project cycles through several waveform styles, including an X-Y oscilloscope mode. Wave shape and amplitude react to the audio. Preserve the recognizable waveform family and audio response. All six of the original's waveforms are ported.
- **Layered effects:** The renderer has optional effects such as chasers, bars, dots, solar-like overlays, grids, nuclide patterns, and shading. Some are selected or varied as part of modes. Preserve the signature effects and their combinations after assessing each for visual quality and cost on modern displays. All seven of the original's live effects are ported, with its per-map weights. See [Layered effects](#layered-effects).
- **Automatic variation:** Modes, waveforms, palettes, and effects can change over time. Preserve automatic variety. Pause, lock, and next visual are the first-version controls for that behavior.

### Existing configuration intent

- **Vertical visual size / letterboxing:** percentage of display height occupied by the visual. Deferred. The first version is full-bleed.
- **Wave scaling:** default audio waveform amplitude. This is the first-version sensitivity control.
- **Color behavior:** palette generation, gamma, coarse and solar palette frequency, and optional sound-synchronized color. The 8-bit palette-specific controls reflect old color modes. The prototype uses one cycling palette. Palette variety and optional audio-linked color remain later work.
- **Beat detection:** controls beat-responsive behavior. Beat brightness and look changes on a big beat follow the original, from a bass-focused detector. A user-facing beat control waits.
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
- Current picture: all 25 original map modes and all 6 waveforms, picked with the original's weights and excluded pairs. The window opens on mode 25 and waveform 1. Next and the 20-second timer pick a different pair. Each look also picks up to a few of the original's seven layered effects, with its per-map weights. Lock holds the pair, its effects, and the palette. Pause freezes the picture. One hue-cycle palette with a white-hot top. Each frame feeds back through the same warp. No new controls. Beats brighten the waveform and land look changes, from a bass-focused detector. The horizontal slide shift waits. Chunked map generation, a GPU warp, and the screen saver host wait.
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
