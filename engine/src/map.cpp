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
#include <cmath>
#include <cstddef>

namespace geiss {
namespace {

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

// GenerateChunkOfNewMap for mode >= 17 normalizes by width, then rotates and scales.
struct Plane {
    float nx = 0;
    float ny = 0;
    float centerX = 0;
    float centerY = 0;
    float halfWidth = 0;
};

// The offset uses the width for both axes, as in GenerateChunkOfNewMap. Height places the center.
Plane planeAt(int x, int y, int width, int height, float centerXNorm, float centerYNorm) {
    Plane plane;
    plane.centerX = centerXNorm * static_cast<float>(width);
    plane.centerY = centerYNorm * static_cast<float>(height);
    const float invWidth = 2.f / static_cast<float>(width);
    plane.nx = (static_cast<float>(x) - plane.centerX) * invWidth;
    plane.ny = (static_cast<float>(y) - plane.centerY) * invWidth;
    plane.halfWidth = 0.5f * static_cast<float>(width);
    return plane;
}

SourcePoint place(const Plane& plane, float scale, float turn) {
    const float cosine = std::cos(turn);
    const float sine = std::sin(turn);
    const float rx = plane.nx * cosine - plane.ny * sine;
    const float ry = plane.nx * sine + plane.ny * cosine;
    SourcePoint point;
    point.x = rx * scale * plane.halfWidth + plane.centerX;
    point.y = ry * scale * plane.halfWidth + plane.centerY;
    return point;
}

}  // namespace

SourcePoint mode25Source(int x, int y, int width, int height, const MapParameters& map) {
    SourcePoint point;
    point.x = static_cast<float>(x);
    point.y = static_cast<float>(y);
    if (width < 2 || height < 2) {
        return point;
    }

    const Plane plane = planeAt(x, y, width, height, map.centerXNorm, map.centerYNorm);
    const float radius = std::sqrt(plane.nx * plane.nx + plane.ny * plane.ny);
    const float zoom = std::max(map.zoom, 0.001f);
    return place(plane, zoom / (zoom + radius), map.turn);
}

SourcePoint mapSource(int x, int y, int width, int height, const MapParameters& map) {
    if (map.mode == 25) {
        return mode25Source(x, y, width, height, map);
    }
    SourcePoint point;
    point.x = static_cast<float>(x);
    point.y = static_cast<float>(y);
    if (width < 2 || height < 2) {
        return point;
    }

    const Plane plane = planeAt(x, y, width, height, map.centerXNorm, map.centerYNorm);
    float scale = 1.f;
    float turn = map.turn;
    if (map.mode == 17) {
        // Horizontal tunnel. The source comment's ranges are 0.96...0.98 and 0.30...0.45.
        // The running formula uses these two constants.
        scale = 0.97f - plane.ny * plane.ny * 0.40f;
    } else if (map.mode == 19) {
        // Vortex. The source comment's ranges are 1.03...1.05 and 0.15...0.30.
        scale = 1.04f - 0.25f * std::sqrt(plane.nx * plane.nx + plane.ny * plane.ny);
    } else if (map.mode == 24) {
        // Fast swirl. The pixel loop writes over the shared turn. The comment's
        // ranges are 0.95...0.97 and 0.04...0.06. The running formula uses these constants.
        scale = 0.96f;
        turn = 0.05f;
    } else {
        return mode25Source(x, y, width, height, map);
    }
    return place(plane, scale, turn);
}

void buildMode25Map(std::vector<SourcePoint>& out, int width, int height, const MapParameters& map) {
    out.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            out[static_cast<std::size_t>(y * width + x)] = mode25Source(x, y, width, height, map);
        }
    }
}

void buildMap(std::vector<SourcePoint>& out, int width, int height, const MapParameters& map) {
    out.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            out[static_cast<std::size_t>(y * width + x)] = mapSource(x, y, width, height, map);
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
            const std::size_t index = static_cast<std::size_t>(y * width + x);
            const SourcePoint target = map[index];
            const float sx = wrapX(static_cast<float>(x) + (target.x - static_cast<float>(x)) * step, width);
            float sy = static_cast<float>(y) + (target.y - static_cast<float>(y)) * step;
            sy = std::clamp(sy, 0.f, maxY);

            const int x0 = std::clamp(static_cast<int>(sx), 0, width - 1);
            const int y0 = std::clamp(static_cast<int>(sy), 0, height - 1);
            const int x1 = std::min(x0 + 1, width - 1);
            const int y1 = std::min(y0 + 1, height - 1);
            const float fx = std::clamp(sx - static_cast<float>(x0), 0.f, 1.f);
            const float fy = std::clamp(sy - static_cast<float>(y0), 0.f, 1.f);

            const float p00 = src[static_cast<std::size_t>(y0 * width + x0)];
            const float p10 = src[static_cast<std::size_t>(y0 * width + x1)];
            const float p01 = src[static_cast<std::size_t>(y1 * width + x0)];
            const float p11 = src[static_cast<std::size_t>(y1 * width + x1)];
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
    return 256 * factor / 256;
}

}  // namespace geiss
