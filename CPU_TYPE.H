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

/* Cyrix Device ID Register 0 (DIR0) Values */
#define UNKNOWN_VENDOR      0xff     /* Unknown */
#define Cx486_pr            0xfd     /* Cx486SLC and DLC, No ID Register */
#define Cx486S_a            0xfe     /* A step, No ID Register */


   /***********************/
   /**  486 DLC and SLC  **/
   /***********************/
#define Cx486_SLC    0x0      
#define Cx486_DLC    0x1      
#define Cx486_SLC2   0x2      
#define Cx486_DLC2   0x3      
#define Cx486_SRx    0x4      /* Retail Upgrade Cx486SLC    */
#define Cx486_DRx    0x5      /* Retail Upgrade Cx486DLC    */
#define Cx486_SRx2   0x6      /* Retail Upgrade 2x Cx486SLC */
#define Cx486_DRx2   0x7      /* Retail Upgrade 2x Cx486DLC */

   /***********************/
   /* 486 S, DX, DX2, DX4 */
   /***********************/
#define Cx486S       0x10     /* B step */
#define Cx486S2      0x11     /* B step */
#define Cx486Se      0x12     /* B step */
#define Cx486S2e     0x13     /* B step */
#define Cx486DX      0x1a      
#define Cx486DX2     0x1b      
#define Cx486DX4     0x1f     

   /***********************/
   /***       5x86      ***/
   /***********************/
#define _5x86_1xs    0x28     /* 5x86 1x Core/Bus Clock */
#define _5x86_1xp    0x2a     /* 5x86 1x Core/Bus Clock */
#define _5x86_2xs    0x29     /* 5x86 2x Core/Bus Clock */
#define _5x86_2xp    0x2b     /* 5x86 2x Core/Bus Clock */
#define _5x86_3xs    0x2d     /* 5x86 3x Core/Bus Clock */
#define _5x86_3xp    0x2f     /* 5x86 3x Core/Bus Clock */
#define _5x86_4xs    0x2c     /* 5x86 4x Core/Bus Clock */
#define _5x86_4xp    0x2e     /* 5x86 4x Core/Bus Clock */

   /***********************/
   /***       6x86      ***/
   /***********************/
#define _6x86_1xs    0x30     /* 6x86 1x Core/Bus Clock */
#define _6x86_1xp    0x32     /* 6x86 1x Core/Bus Clock */
#define _6x86_2xs    0x31     /* 6x86 2x Core/Bus Clock */
#define _6x86_2xp    0x33     /* 6x86 2x Core/Bus Clock */
#define _6x86_3xs    0x35     /* 6x86 3x Core/Bus Clock */
#define _6x86_3xp    0x37     /* 6x86 3x Core/Bus Clock */
#define _6x86_4xs    0x34     /* 6x86 4x Core/Bus Clock */
#define _6x86_4xp    0x36     /* 6x86 4x Core/Bus Clock */

   /***********************/
   /***      MediaGX    ***/
   /***********************/
#define MediaGX_x1      0x41     /* MediaGX */

#define MediaGX_3xs     0x45     /* MediaGX 3x Core/Bus Clock */
#define MediaGX_3xp     0x47     /* MediaGX 3x Core/Bus Clock */
#define MediaGX_4xs     0x44     /* MediaGX 4x Core/Bus Clock */
#define MediaGX_4xp     0x46     /* MediaGX 4x Core/Bus Clock */


   /*******************************/
   /***          GXm            ***/
   /*******************************/
#define GXm_4x0     0x40     /* GxM 4x Core/Bus Clock */
#define GXm_4x1     0x42     /* GxM 4x Core/Bus Clock */
#define GXm_5x1     0x47     /* GxM 5x Core/Bus Clock */
#define GXm_6x0     0x41     /* GxM 6x Core/Bus Clock */
#define GXm_6x1     0x43     /* GxM 6x Core/Bus Clock */
#define GXm_7x0     0x44     /* GxM 7x Core/Bus Clock */
#define GXm_7x1     0x46     /* GxM 7x Core/Bus Clock */
#define GXm_8x0     0x45     /* GxM 8x Core/Bus Clock */


   /***********************/
   /***     6x86MX      ***/
   /***********************/
#define _6x86mxs_base     0x50     /* 6x86MX s base 1x */
#define _6x86mxp_base     0x58     /* 6x86MX p base 1x */
#define _6x86mx_1xs       0x50     /* 6x86MX 1x   Core/Bus Clock */
#define _6x86mx_1xp       0x58     /* 6x86MX 1x   Core/Bus Clock */
#define _6x86mx_2xs       0x51     /* 6x86MX 2x   Core/Bus Clock */
#define _6x86mx_2xp       0x59     /* 6x86MX 2x   Core/Bus Clock */
#define _6x86mx_2p5xs     0x52     /* 6x86MX 2.5x Core/Bus Clock */
#define _6x86mx_2p5xp     0x5a     /* 6x86MX 2.5x Core/Bus Clock */
#define _6x86mx_3xs       0x53     /* 6x86MX 3x   Core/Bus Clock */
#define _6x86mx_3xp       0x5b     /* 6x86MX 3x   Core/Bus Clock */
#define _6x86mx_3p5xs     0x54     /* 6x86MX 3.5x Core/Bus Clock */
#define _6x86mx_3p5xp     0x5c     /* 6x86MX 3.5x Core/Bus Clock */
#define _6x86mx_4xs       0x55     /* 6x86MX 4x   Core/Bus Clock */
#define _6x86mx_4xp       0x5d     /* 6x86MX 4x   Core/Bus Clock */
#define _6x86mx_4p5xs     0x56     /* 6x86MX 4.5x Core/Bus Clock */
#define _6x86mx_4p5xp     0x5e     /* 6x86MX 4.5x Core/Bus Clock */
#define _6x86mx_5xs       0x57     /* 6x86MX 5x   Core/Bus Clock */
#define _6x86mx_5xp       0x5f     /* 6x86MX 5x   Core/Bus Clock */


   /***************************************************************/
   /******************    Processor Names       *******************/
   /***************************************************************/
#define _486SLCName     "Cx486SLC(tm)"
#define _486DLCName     "Cx486DLC(tm)"
#define _486SLC2Name    "Cx486SLC2(tm)"
#define _486DLC2Name    "Cx486DLC2(tm)"
#define _486SRxName     "Cx486SRx(tm)"
#define _486DRxName     "Cx486DRx(tm)"
#define _486SRx2Name    "Cx486SRx2(tm)"
#define _486DRx2Name    "Cx486DRx2(tm)"
#define _486SName       "Cx486S(tm)"
#define _486S2Name      "Cx486S2(tm)"
#define _486SeName      "Cx486Se(tm)"
#define _486S2eName     "Cx486S2e(tm)"
#define _486DXName      "Cx486DX(tm)"
#define _486DX2Name     "Cx486DX2(tm)"
#define _486SDLCName    "Cx486SLC/DLC(tm)"
#define _486SaName      "Cx486Sa(tm)"
#define _486DX4Name     "Cx486DX4(tm)"
#define _5x86Name       "5x86(tm)"
#define _6x86Name       "6x86(tm)"
#define _6x86LName      "6x86L(tm)"
#define _6x86MXName     "6x86MX(tm)"
#define _M_IIName       "M II(tm)"
#define MediaGXName     "MediaGX(tm)"
#define GXmName         "GXm(tm)"

