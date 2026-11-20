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

#include "geiss/map.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace geiss {

std::uint32_t Random::next() {
    if (state == 0) {
        state = 0x9E3779B9u;
    }
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

float Random::unit() {
    return static_cast<float>(next() >> 8) / static_cast<float>(1u << 24);
}

int Random::below(int count) {
    return count > 0 ? static_cast<int>(next() % static_cast<std::uint32_t>(count)) : 0;
}

namespace {

// The original tuned modes 1...16 in pixels on a 640-wide frame (rmult = 640 / FXW).
constexpr float kReferenceWidth = 640.f;

float wrapX(float value, int width) {
    const float span = static_cast<float>(width - 1);
    if (span <= 0.f) {
        return 0.f;
    }
    float wrapped = std::fmod(value, span);
    if (wrapped < 0.f) {
        wrapped += span;
    }
    return wrapped;
}

// cos and sin of a turn. The turns are fixed for a whole map, so buildMap computes them once.
struct Rotation {
    float cosine = 1;
    float sine = 0;
};

Rotation rotationOf(float turn) {
    return Rotation{std::cos(turn), std::sin(turn)};
}

// Mode 24's pixel loop replaces the shared turn with 0.05, so the original's random
// rotation direction does not reach it and every fast swirl turns the same way.
float effectiveTurn(const MapParameters& map) {
    return map.mode == 24 ? 0.05f : map.turn;
}

struct Rotations {
    Rotation first;
    Rotation second;
};

Rotations rotationsFor(const MapParameters& map) {
    return Rotations{rotationOf(effectiveTurn(map)), rotationOf(map.turn2)};
}

// One pixel relative to the map center. The original's newx2 and newy2.
struct Offset {
    float centerX = 0;
    float centerY = 0;
    float dx = 0;
    float dy = 0;
    // Length on the 640-wide reference.
    float r640 = 0;
    // dx and dy times 2 / width, for modes 17 and up.
    float nx = 0;
    float ny = 0;
};

Offset offsetAt(int x, int y, int width, int height, const MapParameters& map) {
    Offset offset;
    offset.centerX = map.centerXNorm * static_cast<float>(width);
    offset.centerY = map.centerYNorm * static_cast<float>(height);
    offset.dx = static_cast<float>(x) - offset.centerX;
    offset.dy = static_cast<float>(y) - offset.centerY;
    const float toReference = kReferenceWidth / static_cast<float>(width);
    offset.r640 = std::sqrt(offset.dx * offset.dx + offset.dy * offset.dy) * toReference;
    const float toUnit = 2.f / static_cast<float>(width);
    offset.nx = offset.dx * toUnit;
    offset.ny = offset.dy * toUnit;
    return offset;
}

SourcePoint place(const Offset& offset, float scale, const Rotation& rotation) {
    const float rx = offset.dx * rotation.cosine - offset.dy * rotation.sine;
    const float ry = offset.dx * rotation.sine + offset.dy * rotation.cosine;
    return SourcePoint{rx * scale + offset.centerX, ry * scale + offset.centerY};
}

// Mode 7's jitter: 0...0.0495 in steps of 0.0005, like rand_array's (rand()%100)*0.0005.
float jitter(int x, int y, std::uint32_t seed) {
    std::uint32_t hash = seed ^ (static_cast<std::uint32_t>(x) * 0x9E3779B1u) ^ (static_cast<std::uint32_t>(y) * 0x85EBCA77u);
    hash ^= hash >> 15;
    hash *= 0x2C1B3C6Du;
    hash ^= hash >> 12;
    return static_cast<float>(hash % 100u) * 0.0005f;
}

// Mode 6: each push center adds a push or a swirl weighted by 1 / (distance^2 + 0.1),
// and the sum is normalized to 1.9 pixels. Distances and the move use the 640-wide
// reference, so the field looks the same at any simulation size.
SourcePoint vortexSource(int x, int y, int width, int height, const MapParameters& map) {
    const float toReference = kReferenceWidth / static_cast<float>(width);
    float tx = 0;
    float ty = 0;
    float total = 0;
    for (const VortexPush& push : map.pushes) {
        const float px = (push.xNorm * static_cast<float>(width) - static_cast<float>(x)) * toReference;
        const float py = (push.yNorm * static_cast<float>(height) - static_cast<float>(y)) * toReference;
        const float squared = px * px + py * py;
        float weight = 1.f / (squared + 0.1f);
        total += weight;
        if (push.type == 0) {
            tx += push.dirX * weight;
            ty += push.dirY * weight;
            continue;
        }
        const float inverseLength = 1.f / (std::sqrt(squared) + 0.01f);
        weight += weight;
        const float sign = push.type == 1 ? 1.f : -1.f;
        tx += weight * -py * inverseLength * sign;
        ty += weight * px * inverseLength * sign;
    }
    if (total > 0.000001f) {
        const float normalize = 1.9f / total;
        tx *= normalize;
        ty *= normalize;
    } else {
        tx = 0;
        ty = 0;
    }
    const float fromReference = 1.f / toReference;
    return SourcePoint{static_cast<float>(x) + (tx - 0.1f) * fromReference,
                       static_cast<float>(y) + (ty + 0.6f) * fromReference};
}

// Mode 12: columns away from the centerline read from much closer to it, so the picture
// streaks sideways out of the middle. Offsets use the 640-wide reference.
SourcePoint splitterSource(const Offset& offset, int width) {
    const float toReference = kReferenceWidth / static_cast<float>(width);
    const float dx = offset.dx * toReference;
    float source = 0;
    if (dx < -0.5f) {
        source = -std::sqrt(-dx) + 0.9f;
    } else if (dx > 0.5f) {
        source = std::sqrt(dx) - 0.9f;
    }
    return SourcePoint{offset.centerX + source / toReference, offset.centerY + offset.dy};
}

// Modes 1...16 that compute one scale per pixel. Dither modes pick a rotation by parity.
float lowModeScale(const Offset& offset, int height, int x, int y, const MapParameters& map) {
    const float r = offset.r640;
    switch (map.mode) {
        case 3:
            return 0.95f - offset.dy * (480.f / static_cast<float>(height)) * 0.0005f;
        case 4:
            return 0.9f + r * 0.0025f * 0.14f;
        case 5:
            // The nuclide effect makes the fall-off steeper.
            if (!map.nuclide) {
                return map.f2 - map.f1 * std::sqrt(r / 200.f);
            }
            return map.f2 - map.f1 * (r / 200.f * 1.7f);
        case 7:
            // Tall frames reach radii the 640x480 original never did, where f1 - r*f2
            // would turn negative and flip the corners. The scale stops at 0.
            return std::max(map.f1 - r * map.f2, 0.f) + jitter(x, y, map.noiseSeed);
        case 8:
            return 0.85f + 0.1f * std::sin(std::sqrt(r) * map.f1);
        case 9:
            // Stops at 0 on tall frames, as mode 7 does.
            return std::max(map.f1 - r * map.f2, 0.f);
        case 13: {
            const float scale = 1.04f - r * std::sqrt(r) * 0.00025f * 0.14f;
            return (scale - 1.f) * map.f1 + 1.f;
        }
        case 14:
            // The original added under 1/1024 of jitter to the height. It is left out.
            return 0.9f + 0.2f * std::cos(offset.dy * 12.f / static_cast<float>(height));
        case 15:
            return map.f2 + map.f3 * std::sin(std::atan2(offset.dy, offset.dx) * map.f1);
        case 16:
            return std::max(1.05f - r * r * 0.00025f * 0.09f, -1.5f);
        default:
            // Modes 1, 2, and 11 use the map's constant scale.
            return map.scale;
    }
}

float highModeScale(const Offset& offset, const MapParameters& map) {
    const float nx = offset.nx;
    const float ny = offset.ny;
    const float length = std::sqrt(nx * nx + ny * ny);
    switch (map.mode) {
        case 17:
            // Horizontal tunnel. The source comment's ranges are 0.96...0.98 and 0.30...0.45.
            return 0.97f - ny * ny * 0.40f;
        case 18:
            // Vertical tunnel.
            return 0.97f - nx * nx * 0.40f;
        case 19:
            // Vortex. The source comment's ranges are 1.03...1.05 and 0.15...0.30.
            return 1.04f - 0.25f * length;
        case 20:
            // Terra. Above the frame (ny < -1.4) the original took the root of a negative
            // number; this reads it as zero.
            return 1.15f - std::sqrt(std::max(ny + 1.4f, 0.f)) * 0.20f;
        case 21:
            // Diced cube.
            return 0.95f - std::floor(std::fabs(nx) * 10.f) * 0.03f - std::floor(std::fabs(ny) * 10.f) * 0.03f;
        case 22:
            // Phonic rings.
            return 0.95f - std::floor(length * 10.f) * 0.04f;
        case 23:
            // Fast rings.
            return 0.95f - static_cast<float>(static_cast<int>(length * 20.f) % 4) * 0.12f;
        case 24:
            // Fast swirl. The comment's ranges are 0.95...0.97 and 0.04...0.06.
            return 0.96f;
        default: {
            // Mode 25, the smooth 1/r zoom.
            const float zoom = std::max(map.zoom, 0.001f);
            return zoom / (zoom + length);
        }
    }
}

bool dithers(int mode) {
    return mode == 1 || mode == 9 || mode == 11;
}

SourcePoint sourceAt(int x, int y, int width, int height, const MapParameters& map, const Rotations& rotations) {
    if (width < 2 || height < 2) {
        return SourcePoint{static_cast<float>(x), static_cast<float>(y)};
    }
    if (map.mode == 6) {
        return vortexSource(x, y, width, height, map);
    }
    const Offset offset = offsetAt(x, y, width, height, map);
    if (map.mode == 10) {
        // Rows read from 4% further down, and columns stretch away from the center.
        const float stretch = 1.03f + 0.03f * static_cast<float>(y) / static_cast<float>(height);
        return SourcePoint{offset.dx * stretch + offset.centerX, static_cast<float>(y) * 1.04f};
    }
    if (map.mode == 12) {
        return splitterSource(offset, width);
    }
    if (map.mode >= 17) {
        return place(offset, highModeScale(offset, map), rotations.first);
    }
    if (dithers(map.mode) && (x % 2) != (y % 2)) {
        return place(offset, map.scale2, rotations.second);
    }
    return place(offset, lowModeScale(offset, height, x, y, map), rotations.first);
}

// Original turn for modes 4, 13, 14, 16, and 17 up: 0.007...0.027.
float normalTurn(Random& random) {
    return 0.007f + 0.02f * random.unit();
}

}  // namespace

const char* mapName(int mode) {
    static constexpr std::array<const char*, kLastMapMode> kNames = {
        "Constant zoom", "Outward zoom", "Terra-landing", "Sphere", "Super-perspective",
        "Vortex field", "Fuzzy", "Ripples", "Flower petals", "Scroll",
        "Two rotations", "Sideways splitter", "Black hole", "Split world", "Radial petals",
        "Crystal ball", "Horizontal tunnel", "Vertical tunnel", "Vortex", "Terra",
        "Diced cubes", "Phonic rings", "Fast rings", "Fast swirl", "Smooth zoom",
    };
    if (mode < kFirstMapMode || mode > kLastMapMode) {
        return "?";
    }
    return kNames.at(static_cast<std::size_t>(mode - kFirstMapMode));
}

MapParameters randomMapParameters(int mode, int width, int height, Random& random) {
    MapParameters map;
    map.mode = std::clamp(mode, kFirstMapMode, kLastMapMode);
    width = std::max(width, 2);
    height = std::max(height, 2);
    const float centerX = (static_cast<float>(width) * 0.5f - 1.f) + (random.unit() * 60.f - 30.f);
    const float centerY = (static_cast<float>(height) * 0.5f - 1.f) + (random.unit() * 30.f - 15.f);
    map.centerXNorm = centerX / static_cast<float>(width);
    map.centerYNorm = centerY / static_cast<float>(height);

    // Defaults the original set for every mode before the per-mode overrides.
    map.f1 = 0.92f + 0.05f * random.unit();
    map.f2 = 0.0009f + 0.0012f * random.unit();
    float turn = 0;
    float turn2 = 0;
    bool separateTurn2 = false;

    switch (map.mode) {
        case 1:
            map.scale = 0.985f - 0.12f * std::pow(random.unit(), 2.f);
            map.scale2 = map.scale;
            turn = 0.01f + 0.01f * random.unit();
            if (map.scale > 0.97f && random.below(3) == 1) {
                turn = -turn;
            }
            break;
        case 2:
            map.scale = 1.f + 0.02f * random.unit();
            turn = 0.02f + 0.07f * random.unit();
            break;
        case 3:
            turn = 0.01f + 0.015f * random.unit();
            break;
        case 5:
            turn = 0.01f + 0.03f * random.unit();
            map.f1 = 0.05f + 0.05f * random.unit() + 0.07f * random.unit();
            map.f2 = 0.99f - 0.01f * random.unit() - 0.02f * random.unit();
            break;
        case 6:
            for (VortexPush& push : map.pushes) {
                push.xNorm = random.unit();
                push.yNorm = random.unit();
                const float angle = random.unit() * 6.28f;
                const float magnitude = 1.f + random.unit() * 0.8f;
                push.dirX = std::cos(angle) * magnitude;
                push.dirY = std::sin(angle) * magnitude;
                push.type = random.below(3);
            }
            break;
        case 7:
            turn = 0.01f + 0.01f * random.unit();
            map.f1 = 0.92f + 0.01f * random.unit();
            map.f2 = 0.0006f + 0.0005f * random.unit();
            map.noiseSeed = random.next() | 1u;
            break;
        case 8: {
            turn = 0.05f * random.unit();
            float ripple = random.unit();
            ripple *= ripple;
            ripple *= ripple;
            map.f1 = ripple * 8.f + 1.5f;
            break;
        }
        case 9:
            // The dithered half keeps scale1's starting value. The original also shrank
            // it toward 1 above 640 pixels wide; this keeps the 640-wide look.
            map.scale2 = 0.8f + 0.25f * random.unit();
            turn = 0.01f + 0.03f * random.unit();
            map.f1 = 0.98f + 0.01f * random.unit();
            map.f2 = 0.0009f + 0.0012f * random.unit();
            break;
        case 11: {
            const float scale = 1.008f + 0.008f * random.unit();
            const float spin = 0.12f + 0.06f * random.unit();
            map.scale = scale * 0.99f;
            map.scale2 = scale * 1.01f;
            turn = spin * -0.6f;
            turn2 = spin * 0.1f;
            separateTurn2 = true;
            break;
        }
        case 12:
            map.decay = 0.98f;
            break;
        case 13:
            turn = normalTurn(random);
            map.f1 = 0.92f + 0.16f * random.unit();
            break;
        case 15:
            turn = 0.04f * random.unit() + 0.045f * random.unit();
            map.f1 = static_cast<float>(random.below(5) + 2);
            map.f2 = 0.92f + 0.06f * random.unit();
            map.f3 = 0.05f + 0.05f * random.unit();
            break;
        case 25:
            turn = normalTurn(random);
            // The source uses the constant 3. The comment's useful range is about 2...6.
            map.zoom = 2.f + 4.f * random.unit();
            break;
        default:
            // Modes 4, 10, 14, 16, and 17...24. Modes 10 and 12 do not turn.
            turn = normalTurn(random);
            break;
    }
    if (!separateTurn2) {
        turn2 = turn;
    }
    // Every mode then flips its rotation half the time and slows it to 0.6.
    if (random.below(2) == 1) {
        turn = -turn;
        turn2 = -turn2;
    }
    map.turn = turn * 0.6f;
    map.turn2 = turn2 * 0.6f;

    const bool dampened = map.mode <= 16 && map.mode != 3 && map.mode != 6;
    map.damping = dampened ? 0.5f : 1.f;
    return map;
}

SourcePoint mode25Source(int x, int y, int width, int height, const MapParameters& map) {
    MapParameters zoom = map;
    zoom.mode = 25;
    return sourceAt(x, y, width, height, zoom, rotationsFor(zoom));
}

SourcePoint mapSource(int x, int y, int width, int height, const MapParameters& map) {
    return sourceAt(x, y, width, height, map, rotationsFor(map));
}

void buildMap(std::vector<SourcePoint>& out, int width, int height, const MapParameters& map) {
    const Rotations rotations = rotationsFor(map);
    out.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            out[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)] =
                sourceAt(x, y, width, height, map, rotations);
        }
    }
}

void warpIndices(const std::uint8_t* src, std::uint8_t* dst, int width, int height,
                 const SourcePoint* map, float step, float decay) {
    if (width < 2 || height < 2 || src == nullptr || dst == nullptr || map == nullptr) {
        return;
    }
    step = std::clamp(step, 0.f, 1.f);
    decay = std::clamp(decay, 0.f, 1.f);
    const float maxY = static_cast<float>(height - 1);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x);
            const SourcePoint target = map[index];
            // Columns wrap around, as the original's newx did (newx += FXW-1 while below 0),
            // so maps that read past the side edges pull in the opposite edge. Rows clamp.
            const float sx = wrapX(static_cast<float>(x) + (target.x - static_cast<float>(x)) * step, width);
            float sy = static_cast<float>(y) + (target.y - static_cast<float>(y)) * step;
            sy = std::clamp(sy, 0.f, maxY);

            const int x0 = std::clamp(static_cast<int>(sx), 0, width - 1);
            const int y0 = std::clamp(static_cast<int>(sy), 0, height - 1);
            const int x1 = std::min(x0 + 1, width - 1);
            const int y1 = std::min(y0 + 1, height - 1);
            const float fx = std::clamp(sx - static_cast<float>(x0), 0.f, 1.f);
            const float fy = std::clamp(sy - static_cast<float>(y0), 0.f, 1.f);

            const float p00 = src[static_cast<std::size_t>(y0) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x0)];
            const float p10 = src[static_cast<std::size_t>(y0) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x1)];
            const float p01 = src[static_cast<std::size_t>(y1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x0)];
            const float p11 = src[static_cast<std::size_t>(y1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x1)];
            const float top = p00 * (1.f - fx) + p10 * fx;
            const float bottom = p01 * (1.f - fx) + p11 * fx;
            float value = (top * (1.f - fy) + bottom * fy) * decay;
            value = std::clamp(value, 0.f, 255.f);
            dst[index] = static_cast<std::uint8_t>(value + 0.5f);
        }
    }
}

int weightNumerator(int width, int height) {
    const long area = static_cast<long>(width) * static_cast<long>(height);
    int factor = 255;
    if (area <= 320L * 240L) {
        factor = 250;
    } else if (area <= 400L * 300L) {
        factor = 251;
    } else if (area <= 512L * 384L) {
        factor = 252;
    } else if (area <= 800L * 600L) {
        factor = 253;
    } else if (area <= 1280L * 960L) {
        factor = 254;
    }
    return factor;
}

}  // namespace geiss
