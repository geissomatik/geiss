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

#include "geiss/audio.hpp"
#include "geiss/map.hpp"

#include <cstdint>

namespace geiss {

struct FrameView {
    const std::uint8_t* rgba = nullptr;
    int width = 0;
    int height = 0;
};

// Index-buffer feedback picture. Metal receives only the RGBA view.
class Engine {
public:
    explicit Engine(std::uint32_t seed);

    void resize(int width, int height);
    void setPaused(bool paused);
    void setLocked(bool locked);
    void nextVisual();

    bool paused() const { return paused_; }
    bool locked() const { return locked_; }
    int width() const { return width_; }
    int height() const { return height_; }

    // `dt` is seconds. Gaps longer than 0.1s count as 0.1s.
    // The returned pointer is valid until the next tick or resize.
    FrameView tick(double dt, const AudioFrame& audio);

private:
    void rebuildMap();
    void expandPalette(const std::uint8_t* indices);

    int width_ = 0;
    int height_ = 0;
    bool paused_ = false;
    bool locked_ = false;
    double mapElapsed_ = 0;
    float phase_ = 0;
    MapParameters mapParameters_{};
    std::uint32_t rngState_;
    std::vector<SourcePoint> map_;
    std::vector<std::uint8_t> front_;
    std::vector<std::uint8_t> back_;
    std::vector<std::uint8_t> rgba_;
};

}  // namespace geiss
