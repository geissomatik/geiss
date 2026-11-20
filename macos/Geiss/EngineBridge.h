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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// What the current look is made of, for the debug info readout.
@interface GeissLookInfo : NSObject

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@property (nonatomic, readonly) int mapMode;
@property (nonatomic, readonly, copy) NSString *mapName;
@property (nonatomic, readonly) int waveform;
@property (nonatomic, readonly, copy) NSString *waveformName;
// True when the nuclide replaces the waveform in this look.
@property (nonatomic, readonly) BOOL waveformHidden;
// The look's effects in the original's order. Chasers reads "Chasers x2" for two.
@property (nonatomic, readonly, copy) NSArray<NSString *> *effects;
// True while the latest audio is silent, so the effects rest.
@property (nonatomic, readonly) BOOL resting;

@end

// The SwiftUI shell sees only these Objective-C types. The pointer from tick is
// valid until the next tick or resize.
@interface GeissBridge : NSObject

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)initWithSeed:(uint32_t)seed NS_DESIGNATED_INITIALIZER;
- (void)setSensitivity:(float)sensitivity;
- (void)setPaused:(BOOL)paused;
- (void)setLocked:(BOOL)locked;
- (void)nextVisual;
- (void)resizeWidth:(int)width height:(int)height;
- (void)ingestInterleaved:(const float * _Nullable)samples
                frameCount:(int)frameCount
                  channels:(int)channels
                sampleRate:(double)sampleRate
                      time:(double)time;
- (GeissLookInfo *)lookInfo;
- (const uint8_t *)tick:(double)dt width:(int *)width height:(int *)height;

@end

NS_ASSUME_NONNULL_END
