// This file is part of: Geiss screensaver and Winamp music visualization plug-in
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

#ifndef GEISS_SOUND_HEADER
#define GEISS_SOUND_HEADER 1


#include "defines.h"


#define PAST_VOL_N 30

extern float gF[6];
extern long chaser_offset;
extern long intframe;
extern int mode;
extern int gXC;
extern int gYC;
extern int waveform;
extern __int16 g_SoundBuffer[16384];
extern float g_fSoundBuffer[16384];
extern bool SoundEnabled;
extern bool SoundReady;
extern bool SoundActive;
extern bool SoundEmpty;
extern int oldSoundBufPos;
extern int oldSoundBufNum;
extern float VOL_SUPERWIDE;
extern int frames_since_silence;
extern float avg_peaks;
extern float current_vol;
extern float past_vol[PAST_VOL_N];
extern bool  past_beat[PAST_VOL_N*2];
extern int   past_vol_pos;
extern int   past_beat_pos;
extern float avg_vol;
extern float avg_vol_wide;
extern float avg_vol_narrow;
extern long FXW;
extern long FXH;
extern int iDispBits;
extern int volpos;
extern float volscale;
extern int g_hit;
extern long FX_YCUT;
extern long FX_YCUT_HIDE;
extern long	FX_YCUT_NUM_LINES;
extern long	FX_YCUT_xFXW_x6;
extern long	FX_YCUT_xFXW;
extern long	FX_YCUT_HIDE_xFXW;
extern long FXW_x_FXH;
extern long BUFSIZE;



void GetWaveData();
void RenderDots(unsigned char *VS1);
void RenderWave(unsigned char *VS1);


#endif
