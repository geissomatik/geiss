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

#pragma once

#include <cstdint>
#include <vector>

namespace geiss {

// Mode 25 parameters. Centers are fractions of the frame so a resize keeps the zoom.
struct MapParameters {
    float centerXNorm = 0.5f;
    float centerYNorm = 0.5f;
    float turn = 0;
    float zoom = 3;
};

struct SourcePoint {
    float x = 0;
    float y = 0;
};

// Full-step source for GenerateChunkOfNewMap mode 25:
//   scale = zoom / (zoom + length(offset * 2 / width))
//   source = center + rotate(offset) * scale
// `zoom` is the constant 3 in the original, varied across 2...6 for a new visual.
SourcePoint mode25Source(int x, int y, int width, int height, const MapParameters& map);

void buildMode25Map(std::vector<SourcePoint>& out, int width, int height, const MapParameters& map);

// Samples `src` through `map`. `step` is 0...1 from the pixel toward the map target.
// `decay` multiplies the bilinear sample. `src` and `dst` must not overlap.
void warpIndices(const std::uint8_t* src, std::uint8_t* dst, int width, int height,
                 const SourcePoint* map, float step, float decay);

// Original weightsum numerator for this area. The per-frame multiplier at 30 fps is numerator/256.
int weightNumerator(int width, int height);

}  // namespace geiss
