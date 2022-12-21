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

#ifndef GEISS_PROCESS_MAP_HEADER 
#define GEISS_PROCESS_MAP_HEADER 1

#include <time.h>

void Process_Map(void *p1, void *p2);//, LPDIRECTDRAWSURFACE lpDDSurf);

extern long     FX_YCUT        ;// 			 =90;			   //skip this many vert. lines in calculation
extern long     FX_YCUT_HIDE   ;//		 =92; //= FX_YCUT + 2;
extern long	 FX_YCUT_NUM_LINES ;//  =FXW*(FXH-FX_YCUT*2);
extern long	 FX_YCUT_xFXW_x8   ;//  =FX_YCUT*FXW*8;
extern long	 FX_YCUT_xFXW      ;//  =FX_YCUT*FXW;
extern long	 FX_YCUT_HIDE_xFXW ;//	 =FX_YCUT_HIDE*FXW;
extern long     FXW_x_FXH      ;//     =FXW*FXH;
extern long     BUFSIZE        ;//     =max(FXW*2, MINBUFSIZE);
extern unsigned char *DATA_FX;
extern int iDispBits;
extern long FXW;
extern long FXH;
extern clock_t core_clock_time;
extern int initial_map_offset;
//extern bool bCyrix;
extern bool bBypassAssembly;

#endif