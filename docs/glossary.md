# Glossary

Names and short descriptions of the macOS visualizer's 25 maps and 7 layered effects. Each description follows the engine code (`engine/src/map.cpp` and `engine/src/effects.cpp`). The exact formulas and every intentional difference from the original are in the [spec](spec/macos-port.md#prototype-milestone).

## Reading the map descriptions

Every frame, each pixel reads the previous frame at a source point that the map gives it. Then the whole picture darkens a little. Most maps use a *scale*. Below 1, a pixel reads from closer to the map center, so the picture flows outward, which looks like zooming in. Above 1, the picture flows inward toward the center. A *turn* rotates the source point around the center.

Some details apply to every map:
- The map center sits within 30 simulation pixels of the frame center horizontally and 15 vertically.
- Each new map picks a random rotation direction, except mode 24.
- Modes 1, 2, 4, 5, and 7…16 move half as far per frame as their numbers say.
- Distances are given for a 640-wide frame and scale with the width.

## Maps

| # | Name | What it does |
|---|---|---|
| 1 | Constant zoom | One scale of 0.865…0.985 everywhere, so the whole picture flows outward at the same rate and turns slowly. A new look on this map opens with a burst of solar particles half the time. |
| 2 | Outward zoom | One scale of 1.00…1.02 everywhere, so the picture zooms out: it shrinks slowly toward the center. It turns faster than mode 1. It allows up to 5 effects at once, more than any other map. |
| 3 | Terra-landing | The scale depends on the row: 0.95 at the center row, smaller below it, larger above it. The lower part rushes outward while the upper part drifts slowly in, like flying low over the ground. It moves at full speed. |
| 4 | Sphere | The scale is 0.9 at the center and rises with distance, passing 1 at about 286 pixels. Inside that radius the picture swells outward. Outside it, the picture pulls in. The picture gathers into a bulging ball. |
| 5 | Super-perspective | The scale starts just under 1 at the center and falls with the square root of the distance. The center barely moves while the edges rush outward, which gives a deep sense of perspective. When the look has the nuclide effect, the scale falls in a straight line with distance instead, so the edges rush out even faster. |
| 6 | Vortex field | Five randomly placed push centers each either push in a fixed direction or swirl one way or the other. Each pixel follows the centers nearest to it most strongly. The total move is normalized to 1.9 pixels per frame, plus a small constant drift. It has no overall zoom. It never pairs with the circle waveform. |
| 7 | Fuzzy | An outward zoom that is faster toward the edges, with a fixed random jitter of up to 0.05 added to each pixel's scale. The jitter smears the picture into a grainy, fuzzy texture. |
| 8 | Ripples | The scale swings between 0.75 and 0.95 with the sine of the square root of the distance. The result is concentric bands that zoom out at different speeds, so the picture ripples. The ripple frequency is random and usually low. |
| 9 | Flower petals | The pixels form a checkerboard. Half of them zoom out gently, faster toward the edges. The other half use one fixed scale of 0.8…1.05. Both turn the same way. The two interleaved zooms feed back into each other and grow petal and flower shapes. |
| 10 | Scroll | Each row reads from 4% further down, so the picture scrolls upward, fastest near the bottom. Each column reads from 3…6% further from the center, so the picture also narrows as it rises. It has no rotation. The horizontal-line waveform sits at three quarters of the height over this map. |
| 11 | Two rotations | The pixels form a checkerboard of two slightly inward zooms, about 1.00 and 1.02. They turn in opposite directions, one fast and one slow. The interleaved motions twist the picture into a shimmering interference pattern. |
| 12 | Sideways splitter | Columns read from much closer to a vertical line through the center (the square root of their distance), and rows do not move. The picture streaks sideways out of that line in both directions. It has no rotation. It fades faster than the other maps, and its center darkening covers a 3-pixel column instead of a point. |
| 13 | Black hole | The scale is above 1 near the center and below 1 beyond about 110 pixels. The center sucks the picture in while the outer area pushes it out toward the edges. The strength varies a little from map to map. |
| 14 | Split world | The scale follows a cosine of the row, between 0.7 and 1.1, mirrored above and below the center row. Horizontal bands alternately pull in and push out, so the picture splits into upper and lower worlds. |
| 15 | Radial petals | The scale follows a sine of the angle around the center, with 2…6 lobes. Some directions zoom out faster than the ones between them, and strong lobes pull the picture in. The picture grows rotating petals. |
| 16 | Crystal ball | The scale is 1.05 at the center and falls with the square of the distance. It reaches 0 at about 216 pixels and stops at −1.5. Beyond that radius, pixels read from the opposite side of the center, so the outside shows a flipped image around a ball. |
| 17 | Horizontal tunnel | The scale is 0.97 on the center row and falls toward the top and bottom. The top and bottom rush outward faster than the middle, like flying down a tunnel whose walls are above and below. |
| 18 | Vertical tunnel | The same as mode 17 turned on its side. The left and right sides rush outward faster than the middle. |
| 19 | Vortex | The scale is 1.04 at the center and falls with distance, passing 1 at about 51 pixels. The center pulls in and the rest pushes out. Together with the turn, this gives a swirling vortex. |
| 20 | Terra | The scale depends on the row along a square-root curve, from about 0.97 at the top of a 16:9 frame to about 0.87 at the bottom. The whole picture flows outward, slowly at the top and fast at the bottom, like a ground plane under a sky. It is a later variant of terra-landing. |
| 21 | Diced cubes | The frame is cut into a grid of cells, 20 across. Each cell zooms with its own scale, 0.03 smaller for each step away from the center in either direction. The cells move at different speeds, so the picture breaks into blocks. |
| 22 | Phonic rings | Concentric rings, each 1/20 of the width wide. Each ring zooms outward 0.04 faster than the ring inside it, so the picture moves in stepped rings. |
| 23 | Fast rings | Rings half as wide as in mode 22 cycle through four scales: 0.95, 0.83, 0.71, and 0.59. Neighboring rings move at very different speeds, so the rings cut sharply and change quickly. |
| 24 | Fast swirl | A constant scale of 0.96 with a fixed turn of 0.05 radians per frame. It always turns the same way, unlike every other map. The picture spirals steadily and quickly outward. |
| 25 | Smooth zoom | The scale is `zoom / (zoom + distance)`, with a random zoom of 2…6. It is 1 at the center and falls smoothly, so the zoom speeds up toward the edges without any curl. It turns slowly. The window opens on this map. |

## Layered effects

Effects draw into the previous frame before the warp, so the map smears them into trails. They draw only while there is sound and rest in silence. Each look picks its effects with the map's weights. Most maps allow one or two at a time. Positions are around the map center, and sizes are for a 640-wide frame.

| Effect | What it does |
|---|---|
| Chasers | One or two points race along looping two-cosine paths, up to about 140 pixels from the center. They take 20 steps per frame, and each step lifts its pixel 40% of the way to white. The result is a bright continuous line that the map trails behind. |
| Bar | A short line between two endpoints that wander within about 30 pixels of the center. Each frame it adds 16 to every pixel on it that is below 223. The map smears it into a glowing band. A look with Grid turns Bar off. |
| Dots | A slow point keeps its last 20 positions as a trail of dots. Each dot keeps the brightness it had when it was placed, which cycles with time. Each frame the trail is redrawn and every dot shifts one pixel to the right. |
| Solar | Particles land within 35 pixels of the center. Each adds a 3×3 glow that is brighter the closer it lands to the center. Pixels that are already bright, about 200 or more, take none. The number of particles swings with time and with the map's solar strength. |
| Grid | A dot every 1/30 of the width, in both directions. The dots scroll one pixel per frame, left or right, and their brightness pulses slowly. A dot only brightens a pixel, never darkens it. |
| Nuclide | When the volume jumps above 1.1 times its recent average, a ring of 3…7 soft discs drops 34…41 pixels from the center. The bigger the jump, the bigger the discs, up to 10 pixels. In 4 looks out of 7 it replaces the waveform, except over mode 10. |
| Shade bobs | One bob wobbles a few pixels around the center and takes four random steps of up to two pixels per frame. Each step adds 2 to its pixel and 1 to the four pixels beside it, up to just below white. The bob builds a soft, shifting glow at the center. The original's preset files call it "shadebobs". |

## Other terms

- **Look:** a map, a waveform, and that map's effects. Next and the 20-second timer pick a new look. Lock holds the current one.
- **Center darkening:** 13 of the maps darken the pixels at their center every frame, so the zoom does not burn a hot spot there. It belongs to the map, so it continues in silence.
