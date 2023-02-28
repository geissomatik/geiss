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

#ifndef GEISS_VIDEO
#define GEISS_VIDEO 1

// this file contains:
// 1. merge_all_vs() functions   (320 and regular)
// 2. palette functions
// 3. loadimage and screenshot functions



#define y_inc 18

#define MY_TEXT_COLOR RGB(255, 240, 125) //RGB( 140, 140, 240 )


#if PLUGIN
void Check_For_New_Song_Title()
{
    // get song name (as a text string)
    char g_newtitle[256];

    if (g_bDisableSongTitlePopups) return;
    if (g_hit >= 0) return;

    if (1)//(intframe%5)==0 || g_hit>0)//==g_song_tooltip_frames)
    {
        ::GetWindowText(this_mod_hwndParent, g_newtitle, sizeof(g_newtitle));

        // remove '- Winamp' or '- Winamp [Paused]' at end
        if (strlen(g_newtitle) > 8)
        {
            int check_pos = strlen(g_newtitle) - 8;
            if (strcmp("- Winamp", (char *)(g_newtitle + check_pos)) == 0)
                g_newtitle[check_pos] = 0;
        }
        if (strlen(g_newtitle) > 17)
        {
            int check_pos = strlen(g_newtitle) - 17;
            if (strcmp("- Winamp [Paused]", (char *)(g_newtitle + check_pos)) == 0)
                g_newtitle[check_pos] = 0;
        }

        // remove song # and period
        if (strlen(g_newtitle) > 6)
        {
            int dot_pos = 1;
            int len = strlen(g_newtitle);

            while (dot_pos < len && dot_pos < 6)
            {   
                if (g_newtitle[dot_pos-1] == '.' && g_newtitle[dot_pos] == ' ')
                {
                    for (int i = dot_pos; i < len; i++)
                        g_newtitle[i - dot_pos] = g_newtitle[i];
                    g_newtitle[len - dot_pos] = 0;
                    break;
                }
                dot_pos++;
            }
        }

        if (strcmp(g_newtitle, g_song_title) != 0)
        {
            g_title_R = 128 + rand() % 99;
            g_title_G = 128 + rand() % 99;
            g_title_B = 128 + rand() % 99;
	        strcpy(g_song_title, g_newtitle);
			if (!g_LastMessageWasCustom)
			{
				g_hit = g_song_tooltip_frames*fps_at_last_mode_switch/30.0f;
			}
			g_LastMessageWasCustom = false;
        }

    }
}
#endif

#if (GRFX==1)
	void Plop_Songtitle_Using_Backbuffer_As_Scratch(int phase)
	{
		// phase = 0: we haven't copied VS to backbuffer yet; so we must textout to backbuffer & copy to VS
		// phase = 1: we textout to the back buffer just before the page flip

	/*
	#if PLUGIN
		if (g_bDisableSongTitlePopups) 
		{
			return;
		}
	#endif
	*/
    
		// merge into VS
		if (g_hit >= 2)// && g_hit <= g_song_tooltip_frames) 
		{
			HDC hdc;
			//g_song_title_y = FXH/2;
			//g_song_title_x = FXW/2;
			HRESULT ddrval;
    
			LPDIRECTDRAWSURFACE     lpDDSurf = lpDDSBack;

			if ((ddrval = lpDDSurf->GetDC(&hdc)) == DD_OK)
			{
				// load font if not loaded
				if ( g_title_font==NULL && 
					#if PLUGIN
					!g_bDisableSongTitlePopups &&
					#endif
					FXW>320 )
				{
					dumpmsg("calling CreateFont()");
        
					g_title_font = CreateFont( 
						// best combination so far: 24x10 with 700 thickness
						24,   //height - 24
						10,   //width - 10      
						0, 
						0, 
						700,  // thickness/strength - 400 normal, 700 bold
						false,   //italics - looks good!
						false,   //underline
						false,   //strikethrough
						DEFAULT_CHARSET, 
						OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						VARIABLE_PITCH | FF_DONTCARE,
						NULL 
					);

					dumpmsg("finished CreateFont()");
				}

				if (g_title_font)
				{
					SelectObject(hdc, g_title_font);
				}

				SIZE siz;
				::GetTextExtentPoint(hdc, g_song_title, lstrlen(g_song_title), &siz);
 
				int y_range = (FXH - FX_YCUT_HIDE*2 - siz.cy);
				int x_range = (FXW-siz.cx);

        static int g_hit_prev = -1;
				if (g_hit_prev < g_hit)//(g_hit==g_song_tooltip_frames)
				{
					if (x_range > 0)
						g_song_title_x = (rand() % (x_range/4)) + 
										  (rand() % (x_range/4)) + 
										   (rand() % (x_range/4)) +
											(rand() % (x_range/4));
					else 
						g_song_title_x = 0;
					g_song_title_y = FX_YCUT_HIDE + 
									  (rand() % (y_range/4)) + 
									   (rand() % (y_range/4)) + 
										(rand() % (y_range/4)) +
										 (rand() % (y_range/4));
					//g_song_title_x =                x_range/3 + rand() % (x_range/3);
					//g_song_title_y = FX_YCUT_HIDE + y_range/3 + rand() % (y_range/3);
				}
        g_hit_prev = g_hit;

  				//SetBkColor( hdc, RGB( 1, 1, 1 ) );
				SetBkColor( hdc, RGB( 0, 0, 0 ) );  // hopefully fixes the white-palette WHITE BLOCK shown during song title popups

				if (g_hit==2)
				//if (g_hit>=2 && g_hit<=8)
				{
					SetBkMode( hdc, OPAQUE );
				}
				else
				{
					SetBkMode( hdc, TRANSPARENT );
				}

				if (g_hit>2) 
				//if (g_hit>8) 
				{
    				SetTextColor( hdc, RGB( 20, 20, 20 ) );
					TextOut( hdc, g_song_title_x+1, g_song_title_y+1, g_song_title, lstrlen(g_song_title) );
				}
				if (g_hit>2 || (g_hit==2 && phase==0))
				//if (g_hit>8 || (g_hit>=2 && g_hit<=8 && phase==0))
				{
					SetTextColor( hdc, RGB( 225, 225, 225 ) );
					TextOut( hdc, g_song_title_x, g_song_title_y, g_song_title, lstrlen(g_song_title) );
				}
				lpDDSurf->ReleaseDC(hdc);


				//---------------------------------------------
				// now copy from backbuffer (scratchpad) to VS.
				//---------------------------------------------
				
				if (g_hit==2 && phase==0)
				//if (g_hit>=2 && g_hit <= 8 && phase==0)
				{
					DDSURFACEDESC ddsd;
					ddsd.dwSize = sizeof(ddsd);

					// lock the buffer
					HRESULT ddrval = lpDDSurf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
					if (ddrval == DD_OK) 
					{
            			unsigned char *dest = (unsigned char *)ddsd.lpSurface;
						unsigned char *dest2 = NULL;
						int linesize = ddsd.lPitch;
						int x, y;
						int vs_index;
						int first_x = g_song_title_x;
						int last_x = min(FXW-2, g_song_title_x + siz.cx);

						if (iDispBits==8)
						{
							for (y=0; y<siz.cy; y++)
							{
								dest2 = (unsigned char *)(dest + linesize*(g_song_title_y + y));
								vs_index = (g_song_title_y + y)*FXW + g_song_title_x;
								for (x = first_x; x < last_x; x++)
								{
									if (dest2[x] > 1) VS1[vs_index] = 225;
									vs_index++;
								}
							}
						}
						else
						{
							int lookat_inc = 0;
							if (iDispBits==32) lookat_inc = 4;
							if (iDispBits==24) lookat_inc = 3;
							if (iDispBits==16) lookat_inc = 2;
							//int lookat_inc = (iDispBits==16) ? 2 : 4;    // 3.40
							first_x *= lookat_inc;
							last_x *= lookat_inc;
							for (y=0; y<siz.cy; y++)
							{
								dest2 = (unsigned char *)(dest + linesize*(g_song_title_y + y));
								vs_index = ((g_song_title_y + y)*FXW + g_song_title_x) << 2;
								for (x = first_x; x < last_x; x += lookat_inc)
								{
									if (dest2[x] > 1) 
									{
										VS1[vs_index    ] = g_title_R;//225;
										VS1[vs_index + 1] = g_title_G;//225;
										VS1[vs_index + 2] = g_title_B;//225;
									}
									vs_index += 4;
								}
							}
						}

						ddrval = lpDDSurf->Unlock(NULL);
						if (ddrval != DD_OK) 
						{
							dumpmsg("Plop_Songtitle_Using_Backbuffer_As_Scratch: lpDDSurf->Unlock() failed");
							//Try_To_Recover(lpDDSurf, ddrval, true);
							return;
						}
					}
    				else 
					{
						dumpmsg("Plop_Songtitle_Using_Backbuffer_As_Scratch: lpDDSurf->Lck() failed");
						//Try_To_Recover(lpDDSurf, ddrval, true);
						return;
					}
				}
			}
			else 
			{
				dumpmsg("Plop_Songtitle_Using_Backbuffer_As_Scratch: lpDDSurf->GetDC() failed");
				//Try_To_Recover(lpDDSurf, ddrval, true);    
				return;
			}
		}
	}

	void Merge_All_VS_To_Backbuffer()
	{

		unsigned char *dest = NULL;
		unsigned int linesize, linesize_over_FXW, linesize_over_FXW4, linesize_over_FXW3, half_linesize, quarter_linesize, linesize_over_FXW2;
		unsigned int FX_YCUT_HIDE_xFXW4 = FX_YCUT_HIDE*FXW*4;
		unsigned long FX_YCUT_HIDE_xLINESIZE; 
		unsigned long LINES_TO_DISPLAY;

		unsigned __int16 FXW_DIV_4 = FXW/4;

		clock_t temp_clock = clock();

		WORD  *worddest = NULL;
		DWORD *dworddest = NULL;
		DWORD pos1, pos2;
		unsigned char clr1, clr2, clr3;
		unsigned __int16 w;
		unsigned __int32 dw;
		unsigned long x2, y2, offset;//, x, y, bx2, by2, x_times_4, y_offset, offset;
		unsigned __int16 FXW_DIV_2 = FXW/2;
		unsigned __int16 FXW_DIV_8 = FXW/8;
		unsigned __int16 FXW_DIV_16 = FXW/16;
		unsigned __int16 FXW_DIV_32 = FXW/32;
		unsigned __int16 FXW_DIV_40 = FXW/40;

		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(ddsd);

		// lock the buffer
		HRESULT ddrval = lpDDSBack->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		
		//if (rand()%40==0) Try_To_Recover(lpDDSBack, ddrval+1, true);


		if (ddrval != DD_OK) 
		{
			// crazy crack patch for when the program comes back from SC_MONITORPOWER
			// state and the palette won't be attached for a while
			iBlendsLeftInPal = 3;
			//Try_To_Recover(lpDDSBack, ddrval, true);

			//dumpmsg("Merge_All_VS_To_Backbuffer: lpDDSBack->Lock() failed");
			return;
			/*
			switch(ddrval)
			{
				case DDERR_INVALIDOBJECT:   dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
				case DDERR_INVALIDPARAMS:   dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
				case DDERR_SURFACEBUSY:     dumpmsg("  [DDERR_SURFACEBUSY]");     break;
				case DDERR_SURFACELOST:     dumpmsg("  [DDERR_SURFACELOST]");     break;
				case DDERR_WASSTILLDRAWING: dumpmsg("  [DDERR_WASSTILLDRAWING]"); break;
				case DDERR_OUTOFMEMORY:     dumpmsg("  [DDERR_OUTOFMEMORY]");     break;
				case DD_OK:                 dumpmsg("  [OK]");                    break;
				default:                    dumpmsg("  [UNKNOWN]");               break;
			}
			if (ddrval==DDERR_SURFACELOST)
			{
				dumpmsg("Calling ->Restore()...");
				ddrval = lpDDSBack->Restore();
				switch(ddrval)
				{
					case DDERR_INVALIDOBJECT:       dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
					case DDERR_INVALIDPARAMS:       dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
					case DDERR_GENERIC:             dumpmsg("  [DDERR_GENERIC]");     break;
					case DDERR_IMPLICITLYCREATED:   dumpmsg("  [DDERR_IMPLICITLYCREATED]");     break;
					case DDERR_INCOMPATIBLEPRIMARY: dumpmsg("  [DDERR_INCOMPATIBLEPRIMARY]"); break;
					case DDERR_OUTOFMEMORY:         dumpmsg("  [DDERR_OUTOFMEMORY]");     break;
					case DDERR_NOEXCLUSIVEMODE:     dumpmsg("  [DDERR_NOEXCLUSIVEMODE]");     break;
					case DDERR_UNSUPPORTED:         dumpmsg("  [DDERR_UNSUPPORTED]");     break;
					case DDERR_WRONGMODE:           dumpmsg("  [DDERR_WRONGMODE]");     break;
					case DD_OK:                     dumpmsg("  [OK]"); break;
					default:                        dumpmsg("  [UNKNOWN]");               break;
				}
			}

			return;  // we'll get it right next time.
			//Try_To_Recover(lpDDSBack, ddrval, true);
			*/
		}


		//stuff
		dest = (unsigned char *)ddsd.lpSurface;
		worddest = (WORD *)ddsd.lpSurface;
		dworddest = (DWORD *)ddsd.lpSurface;

		
		if (intframe==30) clearframes = max(clearframes, 3);
		#if PLUGIN
			if (intframe==80) clearframes = max(clearframes, 3);
		#endif
		#if SAVER
				if (intframe==25 && !g_bDisableCdControls && g_bAutostartCD)
				{
					//dumpmsg("   -ready to autostart CD.  Checking current state.");
					//CheckForNewCD();
					dumpmsg("   -issuing cdaudio commands.");

					if (eDiscState == PAUSED)
					{
						SHOW_TRACK_MSG = 30;
						eDiscState = PLAYING;

						mciSendString("resume cdaudio wait",NULL,NULL,NULL);
					}
					else if (eDiscState == STOPPED)
					{
						SHOW_TRACK_MSG = 30;
						eDiscState = PLAYING;

						mciSendString("stop cdaudio",NULL,NULL,NULL);
						mciSendString("seek cdaudio to 1", NULL, NULL, NULL);
						mciSendString("play cdaudio",NULL,NULL,NULL);
					}

					dumpmsg("   -cdaudio commands complete.");
				}
		#endif
    

		if (intframe<20) 
		{ 
			dumpmsg("[frame %d/20 ok]", intframe);
		}

		if (clearframes > 0)//(intframe <= 4)
		{
			dumpmsg("frame %d: clearing edges", intframe);

			//temp_clock = clock();
			clearframes--;
			memset(ddsd.lpSurface, 0, ddsd.lPitch*FXH);
			//clock_debt += (clock() - temp_clock);
		}
		//memset((unsigned char *)(dest + (intframe%CurVideoHeight)*ddsd.lPitch), 0, ddsd.lPitch);


		if (!bLocked)
		{
    		if (y_map_pos == -1) FX_Pick_Random_Mode();   // sets global 'mode' to a valid random number.
			FX_Apply_Mode();
			/*
			if (--frames_til_auto_switch == 0)
			{
				frames_til_auto_switch = 1000;
				FX_Pick_Random_Mode();
				FX_Apply_Mode();
				FX_Random_Palette();
			}
			*/
		}

		linesize = ddsd.lPitch;
		half_linesize = linesize/2;
		quarter_linesize = linesize/4;
		linesize_over_FXW = linesize-FXW;       // 768
		linesize_over_FXW2 = linesize-FXW*2;
		linesize_over_FXW3 = linesize-FXW*3;
		linesize_over_FXW4 = linesize-FXW*4;
		//linesize_over_FXW8 = linesize-FXW*8;
		FX_YCUT_HIDE_xLINESIZE = FX_YCUT_HIDE * linesize;
		LINES_TO_DISPLAY = (FXH-FX_YCUT_HIDE*2);

		//dumpmsg("ltd=%d", LINES_TO_DISPLAY);
		//dumpmsg("linesize_over_FXW=%d", linesize_over_FXW);


    
		if (iDispBits==8)
		{
			if (bMMX)      // use MMX copy over memcpy()
			{
				if (FXW%32 == 0)   // we can do 32 bytes per loop
				__asm        
				{
					push edi
					push esi

					mov        edi, dworddest
					add        edi, FX_YCUT_HIDE_xLINESIZE
					
					mov        eax, VS1
					add        eax, FX_YCUT_HIDE_xFXW
					
					// [edi]           is start pt. on back surface
					// [eax]           points to VS1
					// ecx is the loop counter
					// ebx holds linesize_over_FXW

					mov        ebx, linesize_over_FXW
					mov        ecx, LINES_TO_DISPLAY

					Scanline_Loop3:

						// upper word of ECX: # of scanlines left
						// lower word of ECX: # of pixels left in current scanline.

						shl   ecx, 16
						mov    cx, FXW_DIV_32

						PixelChunk_Loop3:

							// load, saturate & store the red, green, & blue values
							MOVQ       mm0, [eax]       // read 8 bgrx pixels
							MOVQ       mm1, [eax+8]     // read 8 bgrx pixels
							MOVQ       mm2, [eax+16]    // read 8 bgrx pixels
							MOVQ       mm3, [eax+24]    // read 8 bgrx pixels
							MOVQ       [edi], mm0
							MOVQ       [edi+8], mm1
							MOVQ       [edi+16], mm2
							MOVQ       [edi+24], mm3

							ADD        eax, 32
							ADD        edi, 32

							dec    cx
							jnz    PixelChunk_Loop3
						
						add   edi, ebx         // extra bytes per line
						shr   ecx, 16
						dec   cx
						jnz   Scanline_Loop3

					EMMS
					pop esi
					pop edi
				}
				else if (FXW%40==0)
				__asm        // use MMX over memcpy()
				{
					push edi
					push esi

					mov        edi, dworddest
					add        edi, FX_YCUT_HIDE_xLINESIZE
					
					mov        eax, VS1
					add        eax, FX_YCUT_HIDE_xFXW
					
					// [edi]           is start pt. on back surface
					// [eax]           points to VS1
					// ecx is the loop counter
					// ebx holds linesize_over_FXW

					mov        ebx, linesize_over_FXW
					mov        ecx, LINES_TO_DISPLAY

					Scanline_Loop3b:

						// upper word of ECX: # of scanlines left
						// lower word of ECX: # of pixels left in current scanline.

						shl   ecx, 16
						mov    cx, FXW_DIV_40

						PixelChunk_Loop3b:

							// load, saturate & store the red, green, & blue values
							MOVQ       mm0, [eax]       // read 8 bgrx pixels
							MOVQ       mm1, [eax+8]     // read 8 bgrx pixels
							MOVQ       mm2, [eax+16]    // read 8 bgrx pixels
							MOVQ       mm3, [eax+24]    // read 8 bgrx pixels
							MOVQ       mm4, [eax+32]    // read 8 bgrx pixels
							MOVQ       [edi], mm0
							MOVQ       [edi+8], mm1
							MOVQ       [edi+16], mm2
							MOVQ       [edi+24], mm3
							MOVQ       [edi+32], mm4

							ADD        eax, 40
							ADD        edi, 40

							dec    cx
							jnz    PixelChunk_Loop3b
						
						add   edi, ebx         // extra bytes per line
						shr   ecx, 16
						dec   cx
						jnz   Scanline_Loop3b

					EMMS
					pop esi
					pop edi
				}
				else 
				{
					dumpmsg("BAD VIDEO MODE: (FXH %% 32 != 0 and FXH %% 40 != 0)");
				}

			}
			else
			{
				if (linesize==FXW)
				{
					offset = FX_YCUT_HIDE*FXW;
					pos1 = FX_YCUT_HIDE*linesize;
					memcpy(&dest[pos1], &VS1[offset], (FXH-FX_YCUT_HIDE*2)*FXW );
				}
				else
				{
					offset = FX_YCUT_HIDE*FXW;
					for (y2 = FX_YCUT_HIDE; y2<(FXH-FX_YCUT_HIDE); y2++)
					{
						pos1 = y2*linesize;
						memcpy(&dest[pos1], &VS1[offset], FXW);
						offset += FXW;
					}
				}
			}
		}
		else if (iDispBits==16)
		{

			if (!bMMX)
			{
				unsigned __int32 *VSX = (unsigned __int32 *)VS1;
				unsigned __int32 val;
				unsigned __int8 val1, val2, val3;
				
				offset = FX_YCUT_HIDE*FXW;


    			if (VIDEO_CARD_FUCKED==1)
				for (y2 = FX_YCUT_HIDE; y2<(FXH-FX_YCUT_HIDE); y2++)
				{
					pos1 = y2*(linesize/2);     // .../2 because we're writing WORDs

					for (x2 = 0; x2<FXW; x2++)
					{
						val = VSX[offset++];
						//val1 = REMAP[(val >> 16)      ];
						//val2 = REMAP[(val >>  8) & 255];
						//val3 = REMAP[(val      ) & 255];
						//worddest[pos1++] = ((val1 >> 3) << 10) | ((val2 >> 3) << 5) | (val3 >> 3);
						val1 = REMAP[(val >> 16)      ];
						val2 = REMAP[(val >>  8) & 255];
						val3 = REMAP[(val      ) & 255];
						worddest[pos1++] = ((val1     ) << 10) | ((val2     ) << 5) | (val3     );
					}
				}
				else
				for (y2 = FX_YCUT_HIDE; y2<(FXH-FX_YCUT_HIDE); y2++)
				{
					pos1 = y2*(linesize/2);     // .../2 because we're writing WORDs

					for (x2 = 0; x2<FXW; x2++)
					{
						val = VSX[offset++];
						//val1 = REMAP[(val >> 16)      ];
						//val2 = REMAP[(val >>  8) & 255];
						//val3 = REMAP[(val      ) & 255];
						//worddest[pos1++] = ((val1 >> 3) << 11) | ((val2 >> 2) << 5) | (val3 >> 3);
						val1 = REMAP2[(val >> 16)      ];
						val2 = REMAP2[(val >>  8) & 255];
						val3 = REMAP2[(val      ) & 255];
						worddest[pos1++] = ((val1  >> 1  ) << 11) | ((val2     ) << 5) | (val3  >> 1 );

					}
				}
			}
			else
			{
				// special MMX optimization for going FXWxFXHx16 display mode
				if (VIDEO_CARD_FUCKED==1)
				__asm
				{
					push edi
					push esi

					mov        edi, worddest
					add        edi, FX_YCUT_HIDE_xLINESIZE
					
					mov        eax, VS1
					add        eax, FX_YCUT_HIDE_xFXW4
					
					// [edi]           is start pt. on back surface
					// [eax]           points to VS1
					// ecx is the loop counter
					// ebx holds linesize_over_FXW (times 4)

					mov        ebx, linesize_over_FXW2
					mov        ecx, LINES_TO_DISPLAY

					Scanline_Loop16b:

						// upper word of ECX: # of scanlines left
						// lower word of ECX: # of pixels left in current scanline.

						shl   ecx, 16
						mov    cx, FXW_DIV_2

						PixelChunk_Loop16b:

							MOVQ       mm0, [eax]     // read 64 bits [RGBxRGBx]

							PADDUSB    mm0, mm0       // saturate (gamma-correction)
							MOVQ       mm1, mm0
							MOVQ       mm2, mm0       //|????????rrrrrrrr|ggggggggbbbbbbbb|????????bbbbbbbb|ggggggggrrrrrrrr|

							PSRLD      mm0, 16+3      //|0000000000000000|00000000000rrrrr|0000000000000000|00000000000rrrrr|
							PSRLD      mm1,  8+3      //|0000000000000000|000rrrrrrrrggggg|0000000000000000|0000000000gggggg|
							PSRLD      mm2,    3      //|00000000000rrrrr|rrrggggggggbbbbb|0000000000000000|00000000000bbbbb|
							PSLLD      mm0, 32-5      //|rrrrr00000000000|0000000000000000|0000000000000000|00000000000rrrrr|
							PSLLD      mm1, 32-5      //|ggggg00000000000|0000000000000000|0000000000000000|0000000000gggggg|
							PSLLD      mm2, 32-5      //|bbbbb00000000000|0000000000000000|0000000000000000|00000000000bbbbb|
							PSRLD      mm0, 1         //|0rrrrr0000000000|0000000000000000|0000000000000000|00000000000rrrrr|
							PSRLD      mm1, 1+5       //|000000ggggg00000|0000000000000000|0000000000000000|0000000000gggggg|
							PSRLD      mm2, 1+5+5     //|00000000000bbbbb|0000000000000000|0000000000000000|00000000000bbbbb|

							POR        mm0, mm1
							POR        mm0, mm2       //|0rrrrrgggggbbbbb|0000000000000000|rrrrrggggggbbbbb|0000000000000000|
                        
							MOVQ       mm1, mm0
							PSRLQ      mm0, 16
							PSRLQ      mm1, 32 
							POR        mm0, mm1       //|????????????????|????????????????|0rrrrrgggggbbbbb|0rrrrrgggggbbbbb|

							MOVD       [edi], mm0     // puts lower 32 bits to memory (2 dest. pixels)

                    
							ADD        eax, 8
							ADD        edi, 4

							dec    cx
							jnz    PixelChunk_Loop16b
						
						add   edi, ebx         // extra bytes per line
						shr   ecx, 16
						dec   cx
						jnz   Scanline_Loop16b

					EMMS
					pop esi
					pop edi

				}
				else
				__asm
				{
					push edi
					push esi

					mov        edi, worddest
					add        edi, FX_YCUT_HIDE_xLINESIZE
					
					mov        eax, VS1
					add        eax, FX_YCUT_HIDE_xFXW4
					
					// [edi]           is start pt. on back surface
					// [eax]           points to VS1
					// ecx is the loop counter
					// ebx holds linesize_over_FXW (times 4)

					mov        ebx, linesize_over_FXW2
					mov        ecx, LINES_TO_DISPLAY

					Scanline_Loop16a:

						// upper word of ECX: # of scanlines left
						// lower word of ECX: # of pixels left in current scanline.

						shl   ecx, 16
						mov    cx, FXW_DIV_2

                    
						// IF DOUBLING THE WORKLOAD:
						//shr   cx, 1


						PixelChunk_Loop16a:

							MOVQ       mm0, [eax]     // read 64 bits [RGBxRGBx]

							PADDUSB    mm0, mm0       // saturate (gamma-correction)

													  //MM0 (high half)                      MM1 (high half)                      MM2 (high half)
													  //-----------------------------------  -----------------------------------  -----------------------------------
													  //|........rrrrrrrr|ggggggggbbbbbbbb|
							MOVQ       mm1, mm0       //|........rrrrrrrr|ggggggggbbbbbbbb|  |........rrrrrrrr|ggggggggbbbbbbbb|
							PSRLW      mm0, 3         //|000........rrrrr|000ggggggggbbbbb|  |........rrrrrrrr|ggggggggbbbbbbbb|
							PSRLW      mm1, 10        //|000........rrrrr|000ggggggggbbbbb|  |0000000000......|0000000000gggggg|
							PSLLW      mm0, 11        //|rrrrr00000000000|bbbbb00000000000|  |0000000000......|0000000000gggggg|
							PSLLD      mm1, 21        //|rrrrr00000000000|bbbbb00000000000|  |00000gggggg00000|0000000000000000|
							MOVQ       mm2, mm0       //|rrrrr00000000000|bbbbb00000000000|  |00000gggggg00000|0000000000000000|  |rrrrr00000000000|bbbbb00000000000|
							POR        mm0, mm1       //|rrrrrgggggg00000|bbbbb00000000000|                                       |rrrrr00000000000|bbbbb00000000000|
							PSLLD      mm2, 5         //|rrrrrgggggg00000|bbbbb00000000000|                                       |00000000000bbbbb|0000000000000000|
							POR        mm0, mm2       //|rrrrrggggggbbbbb|bbbbb00000000000|rrrrrggggggbbbbb|bbbbb00000000000|

							PSRLD      mm0, 16        //|0000000000000000|rrrrrggggggbbbbb|0000000000000000|rrrrrggggggbbbbb|
							MOVQ       mm1, mm0
							PSRLQ      mm1, 16
							POR        mm0, mm1

							MOVD       [edi], mm0     // puts lower 32 bits to memory (2 16-bit RGB quantities == 2 pixels)
                    
							ADD        eax, 8//16//8
							ADD        edi, 4//8//4

							dec    cx
							jnz    PixelChunk_Loop16a
						
						add   edi, ebx         // extra bytes per line
						shr   ecx, 16
						dec   cx
						jnz   Scanline_Loop16a

					EMMS
					pop esi
					pop edi

				}
			}
		}
		else if (iDispBits==24)
		{

			//if (bMMX)
			__asm
			{
				push edi
				push esi

				mov        edi, dworddest
				add        edi, FX_YCUT_HIDE_xLINESIZE
				
				mov        eax, VS1
				add        eax, FX_YCUT_HIDE_xFXW4
				mov        esi, eax
				
				// [esi]           points to VS1
				// [edi]           is start pt. on back surface
				// ecx is the loop counter

				mov        ebx, linesize_over_FXW3
				mov        ecx, LINES_TO_DISPLAY

				Scanline_Loop4:

					// upper word of ECX: # of scanlines left
					// lower word of ECX: # of pixels left in current scanline.

					shl   ecx, 16
					mov    cx, FXW_DIV_8

					PixelChunk_Loop4:

						mov  eax, dword ptr [esi]
						mov  ebx, dword ptr [esi+4]
						mov  edx, dword ptr [esi+8]
                    
						// fill high byte of EAX w/ 'r' of pixel #2
						rol  eax, 8
						mov  al, bl
						ror  eax, 8

						// fill high word of EBX with 'g' and 'b' of pixel #3
						rol  ebx, 8
						mov  bx, dx
						ror  ebx, 16

						// fill bottom 3 bytes of edx with value of pixel #4
						mov  edx, [esi+12]
						rol  edx, 8
						mov  dl, [esi+10]    //!

						// write four pixels, packed into 3     [eax:ebx:edx]
						mov  [edi], eax
						mov  [edi+4], ebx
						mov  [edi+8], edx

						add esi, 16
						add edi, 12

								mov  eax, dword ptr [esi]
								mov  ebx, dword ptr [esi+4]
								mov  edx, dword ptr [esi+8]
                    
								// fill high byte of EAX w/ 'r' of pixel #2
								rol  eax, 8
								mov  al, bl
								ror  eax, 8

								// fill high word of EBX with 'g' and 'b' of pixel #3
								rol  ebx, 8
								mov  bx, dx
								ror  ebx, 16

								// fill bottom 3 bytes of edx with value of pixel #4
								mov  edx, [esi+12]
								rol  edx, 8
								mov  dl, [esi+10]    //!

								// write four pixels, packed into 3     [eax:ebx:edx]
								mov  [edi], eax
								mov  [edi+4], ebx
								mov  [edi+8], edx

								add esi, 16
								add edi, 12

						dec    cx
						jnz    PixelChunk_Loop4
					
					add   edi, linesize_over_FXW3 //ebx         // extra bytes per line
					shr   ecx, 16
					dec   cx
					jnz   Scanline_Loop4

				EMMS
				pop esi
				pop edi

			}
			/*
			else
			{
				// with gamma correction:

				unsigned __int32 val;
				unsigned __int8 val1, val2, val3;
				unsigned __int32 *VSX = (unsigned __int32 *)VS1;
				int x;

				offset = FX_YCUT_HIDE*FXW;

				for (y2 = FX_YCUT_HIDE; y2<(FXH-FX_YCUT_HIDE); y2++)
				{
					pos1 = y2*linesize;
					for (x=0; x<FXW; x++)
					{
						val = VSX[offset++];
						dest[pos1] = REMAP[(val >> 16)      ];
						dest[pos1+1] = REMAP[(val >>  8) & 255];
						dest[pos1+2] = REMAP[(val      ) & 255];
						pos1 += 3;
					}
				}
			}
			*/

		}
		else if (iDispBits==32)
		{
			if (bMMX)
			__asm
			{
				push edi
				push esi

				mov        edi, dworddest
				add        edi, FX_YCUT_HIDE_xLINESIZE
				
				mov        eax, VS1
				add        eax, FX_YCUT_HIDE_xFXW4
				
				// [edi]           is start pt. on back surface
				// [eax]           points to VS1
				// ecx is the loop counter
				// ebx holds linesize_over_FXW (times 4)

				mov        ebx, linesize_over_FXW4
				mov        ecx, LINES_TO_DISPLAY

				Scanline_Loop5:

					// upper word of ECX: # of scanlines left
					// lower word of ECX: # of pixels left in current scanline.

					shl   ecx, 16
					mov    cx, FXW_DIV_16

					PixelChunk_Loop5:

						// load, saturate & store the red, green, & blue values
						MOVQ       mm0, [eax]       // read 2 bgrx pixels
						MOVQ       mm1, [eax+8]     // read 2 bgrx pixels
						MOVQ       mm2, [eax+16]    // read 2 bgrx pixels
						MOVQ       mm3, [eax+24]    // read 2 bgrx pixels
						MOVQ       mm4, [eax+32]    // read 2 bgrx pixels
						MOVQ       mm5, [eax+40]    // read 2 bgrx pixels
						MOVQ       mm6, [eax+48]    // read 2 bgrx pixels
						MOVQ       mm7, [eax+56]    // read 2 bgrx pixels
                    
						PADDUSB    mm0, mm0
						PADDUSB    mm1, mm1
						PADDUSB    mm2, mm2
						PADDUSB    mm3, mm3         // multiply by 2
						PADDUSB    mm4, mm4
						PADDUSB    mm5, mm5
						PADDUSB    mm6, mm6
						PADDUSB    mm7, mm7
                    
						MOVQ       [edi], mm0       
						MOVQ       [edi+8], mm1
						MOVQ       [edi+16], mm2
						MOVQ       [edi+24], mm3    // write result to back surface
						MOVQ       [edi+32], mm4
						MOVQ       [edi+40], mm5
						MOVQ       [edi+48], mm6
						MOVQ       [edi+56], mm7
                    
						ADD        eax, 64
						ADD        edi, 64

						dec    cx
						jnz    PixelChunk_Loop5
					
					add   edi, ebx         // extra bytes per line
					shr   ecx, 16
					dec   cx
					jnz   Scanline_Loop5

				EMMS
				pop esi
				pop edi

			}
			else
			{
				// without gamma correction:
				/*
				offset = FX_YCUT_HIDE*FXW*4;
				for (y2 = FX_YCUT_HIDE; y2<(FXH-FX_YCUT_HIDE); y2++)
				{
					pos1 = y2*linesize;
					memcpy(&dest[pos1], &VS1[offset], FXW*4);
					offset += FXW*4;
				}*/

				// with gamma correction:

				unsigned __int32 val;
				unsigned __int8 val1, val2, val3;
				unsigned __int32 *VSX = (unsigned __int32 *)VS1;
				int x;

				offset = FX_YCUT_HIDE*FXW;

				for (y2 = FX_YCUT_HIDE; y2<(FXH-FX_YCUT_HIDE); y2++)
				{
					pos1 = y2*linesize/4;    // div. by 4 because we're writing DWORDs.
					for (x=0; x<FXW; x++)
					{
						val = VSX[offset++];
						val1 = REMAP[(val >> 16)      ];
						val2 = REMAP[(val >>  8) & 255];
						val3 = REMAP[(val      ) & 255];
						dworddest[pos1++] = (unsigned __int32)((val1 << 16) | (val2 << 8) | val3);
					}
				}
			}
		}

		ddrval = lpDDSBack->Unlock(NULL);
		if (ddrval != DD_OK) 
		{
			dumpmsg("Merge_All_VS_To_Backbuffer: lpDDSBack->Unlock() failed");
			//Try_To_Recover(lpDDSBack, ddrval, true);
			return;
		}

		blit_clock_time += clock() - temp_clock;
	}

	void Put_FPS_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;
    

		if (fps < 0) return;
		sprintf(szFPS, " fps: %3.1f ", fps);

  
		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szFPS, lstrlen(szFPS) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);
		/*if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szFPS, lstrlen(szFPS) );
			lpDDSPrimary->ReleaseDC(hdc);
		}*/
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}

	/*
	#if PLUGIN
	void Put_MP3Title_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;
    
		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			if (iDispBits==8)
				SetTextColor( hdc, MY_TEXT_COLOR );
			else
				SetTextColor( hdc, RGB( g_title_R, g_title_G, g_title_B ));
			TextOut( hdc, 0, y_pos, g_song_title, lstrlen(g_song_title) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			if (iDispBits==8)
				SetTextColor( hdc, MY_TEXT_COLOR );
			else
				SetTextColor( hdc, RGB( g_title_R, g_title_G, g_title_B ));
			TextOut( hdc, 0, y_pos, g_song_title, lstrlen(g_song_title) );
			lpDDSPrimary->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);
	}
	#endif
	*/


	void Put_Trackmsg_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;

		if (eDiscState == PLAYING)
		{
			sprintf(szTrack, " - track %d/%d - ", iTrack, iNumTracks);
		}
		else if (eDiscState == STOPPED)
		{
			sprintf(szTrack, " - cd stopped at track %d/%d - ", iTrack, iNumTracks);
		}
		else if (eDiscState == PAUSED)
		{
			sprintf(szTrack, " - cd is paused - ");
		}
		else if (eDiscState == BUSY)
		{
			sprintf(szTrack, " - can't control CD; close Windows CD player if running - ");
		}
		else
		{
			sprintf(szTrack, " - ? - ");
		}


		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szTrack, lstrlen(szTrack) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		/*if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szTrack, lstrlen(szTrack) );
			lpDDSPrimary->ReleaseDC(hdc);
		}*/
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}

	/*
	void Put_Debugmsg_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;

		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szDEBUG, lstrlen(szDEBUG) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szDEBUG, lstrlen(szDEBUG) );
			lpDDSPrimary->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}

	void Put_Mouseclickmsg_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;

		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szMCM, lstrlen(szMCM) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szMCM, lstrlen(szMCM) );
			lpDDSPrimary->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}


	void Put_Lockedmsg_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;

		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szLM, lstrlen(szLM) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szLM, lstrlen(szLM) );
			lpDDSPrimary->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}

	void Put_Unlockedmsg_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;

		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szULM, lstrlen(szULM) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, szULM, lstrlen(szULM) );
			lpDDSPrimary->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}
	*/

	void Put_Msg_To_Backbuffer(char *str)
	{
		HDC hdc;
		HRESULT ddrval;
		int y_pos = FX_YCUT_HIDE;

		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, str, lstrlen(str) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		/*if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, 0, y_pos, str, lstrlen(str) );
			lpDDSPrimary->ReleaseDC(hdc);
		}*/
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);
	}


	void Put_Helpmsg_To_Backbuffer()
	{
		HDC hdc;
		HRESULT ddrval;
		int indent = 20;
		int y_pos = indent + FX_YCUT_HIDE;

		y_pos = min(y_pos, FXH - y_inc*10 - indent);

		if ((ddrval = lpDDSBack->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, indent, y_pos, szH1, lstrlen(szH1) );
			TextOut( hdc, indent, y_pos+y_inc, szH2, lstrlen(szH2) );
			TextOut( hdc, indent, y_pos+y_inc*2, szH3, lstrlen(szH3) );
			TextOut( hdc, indent, y_pos+y_inc*3, szH4, lstrlen(szH4) );
			TextOut( hdc, indent, y_pos+y_inc*4, szH5, lstrlen(szH5) );
			TextOut( hdc, indent, y_pos+y_inc*5, szH6, lstrlen(szH6) );
			TextOut( hdc, indent, y_pos+y_inc*6, szH7, lstrlen(szH7) );
			TextOut( hdc, indent, y_pos+y_inc*7, szH8, lstrlen(szH8) );
			TextOut( hdc, indent, y_pos+y_inc*8, szH9, lstrlen(szH9) );
			TextOut( hdc, indent, y_pos+y_inc*9, szH10, lstrlen(szH10) );
			lpDDSBack->ReleaseDC(hdc);
		}
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSBack, ddrval, true);

		/*
		if ((ddrval = lpDDSPrimary->GetDC(&hdc)) == DD_OK)
		{
			SetBkColor( hdc, RGB( 0, 0, 0 ) );
			SetTextColor( hdc, MY_TEXT_COLOR );
			TextOut( hdc, indent, y_pos, szH1, lstrlen(szH1) );
			TextOut( hdc, indent, y_pos+y_inc, szH2, lstrlen(szH2) );
			TextOut( hdc, indent, y_pos+y_inc*2, szH3, lstrlen(szH3) );
			TextOut( hdc, indent, y_pos+y_inc*3, szH4, lstrlen(szH4) );
			TextOut( hdc, indent, y_pos+y_inc*4, szH5, lstrlen(szH5) );
			TextOut( hdc, indent, y_pos+y_inc*5, szH6, lstrlen(szH6) );
			TextOut( hdc, indent, y_pos+y_inc*6, szH7, lstrlen(szH7) );
			TextOut( hdc, indent, y_pos+y_inc*7, szH8, lstrlen(szH8) );
			TextOut( hdc, indent, y_pos+y_inc*8, szH9, lstrlen(szH9) );
			TextOut( hdc, indent, y_pos+y_inc*9, szH10, lstrlen(szH10) );
			lpDDSPrimary->ReleaseDC(hdc);
		}
		*/
		//else dumpmsg("error: couldn't get DC for text write");//Try_To_Recover(lpDDSPrimary, ddrval, true);

	}
#endif














void FX_Random_Palette(bool bLoadPal)
{
	if (bPalLocked) return;
	if (iDispBits!=8) return;

	int n, a, b;

    if (!bLoadPal)
    {
        old_palette.lo_band = -1;
        old_palette.hi_band = -1;
        old_palette.bFXPalette = false;
        old_palette.iFXPaletteNum = -1;
        old_palette.c1 = -1;
        old_palette.c2 = -1;
        old_palette.c3 = -1;

        if (rand()%10 < coarse_pal_freq)
        {
            old_palette.lo_band = 7 + rand()%6;
            old_palette.hi_band = 17 + rand()%6;
        }
    }

	iBlendsLeftInPal = 18;

    b = rand() % 6;
    if (bLoadPal)
	{
		if (old_palette.bFXPalette) b = 0; else b = 1;
	}

	if (b==0)    // generate monotone palette, from original FX.
	{
        old_palette.bFXPalette = true;
        if (!bLoadPal) old_palette.iFXPaletteNum = rand()%4;

		if (old_palette.iFXPaletteNum==0) 
		{ 
			for (a=0; a<128; a++) 
			{
				REMAP [a] = a*a/64.0;
				REMAP2[a] = a*2;
				REMAP3[a] = sqrtf(a)*22.6;
			} 
		} 
		else if (old_palette.iFXPaletteNum==1) 
		{ 
			for (a=0; a<128; a++) 
			{
				REMAP [a] = a*a/64.0;
				REMAP2[a] = sqrtf(a)*22.6;
				REMAP3[a] = a*2;
			}
		} 
		else if (old_palette.iFXPaletteNum==2) 
		{ 
			for (a=0; a<128; a++) 
			{
				REMAP [a] = sqrtf(a)*22.6;
				REMAP2[a] = a*2;
				REMAP3[a] = a*a/64.0;
			} 
		} 
		else if (old_palette.iFXPaletteNum==3) 
		{ 
			for (a=0; a<128; a++) 
			{
				REMAP [a] = a*2;
				REMAP2[a] = a*a/64.0;
				REMAP3[a] = sqrtf(a)*22.6;
			} 
		}

        /*
		for (a=0; a<128; a++) 
		{
			REMAP [a] = 63+63*sin(a*0.513);
			REMAP2[a] = 63+63*sin(a*0.536);
			REMAP3[a] = 63+63*sin(a*0.563);
		} 
        */


		for (n=128; n<256; n++)
		{
			REMAP [n] = REMAP [127];
			REMAP2[n] = REMAP2[127];
			REMAP3[n] = REMAP3[127];
		}

		for (n=0; n<256; n++) 
		{
			ape[n] = ape2[n];				// new
			ape2[n].peRed   = REMAP [n];	// switched to ape2
			ape2[n].peBlue  = REMAP2[n];
			ape2[n].peGreen = REMAP3[n];
		}

	}
	else   // generate palette from 3 random curves.
	{
        if (!bLoadPal)
        {
            int temp;

            do
            {
                if (rand()%5 < solar_pal_freq)
                {
			        old_palette.c1 = rand() % 7 + 1;
			        old_palette.c2 = rand() % 7 + 1;
			        old_palette.c3 = rand() % 7 + 1;
                }
                else
                {
			        old_palette.c1 = rand() % 6 + 1;
			        old_palette.c2 = rand() % 6 + 1;
			        old_palette.c3 = rand() % 6 + 1;
                }

                temp = 0; // measure of # of 'dark' color channels in palette
                if (old_palette.c1==6) temp++;
                if (old_palette.c2==6) temp++;
                if (old_palette.c3==6) temp++;
            }
            while (temp > 1);  // disallow really dark palettes.
        }
        
		float x,y,z;
        float gamma_factor = 1 + gamma*0.01;
        //float xs = 0.90 + 0.5*(rand() % 100)*0.01;
        //float ys = 0.90 + 0.5*(rand() % 100)*0.01;
        //float zs = 0.90 + 0.5*(rand() % 100)*0.01;
        //float mag = 1.0/sqrtf(xs*xs+ys*ys+zs*zs);
        //xs *= mag * 4;
        //ys *= mag;
        //zs *= mag;

        if (SoundEmpty) gamma_factor += 0.3;

		//int c = rand()%256;

		for (n=0; n<256; n++) 
		{
			ape[n] = ape2[n];
			
			x = CrankPal(old_palette.c1, (unsigned char)n);
			y = CrankPal(old_palette.c2, (unsigned char)n);
			z = CrankPal(old_palette.c3, (unsigned char)n);
            
			x *= gamma_factor;// * xs;
            y *= gamma_factor;// * ys;
            z *= gamma_factor;// * zs;

            if (n>old_palette.lo_band && n<old_palette.hi_band) 
            { 
				x *= 2; 
				y *= 2; 
				z *= 2; 
			}

			//float t = pow(2.718, -fabs(c-n)*0.1);
			//x = ape[n].peRed*t + x*(1-t);
			//y = ape[n].peBlue*t + y*(1-t);
			//z = ape[n].peGreen*t + z*(1-t);

			ape2[n].peRed   = (unsigned char)min(255.0, x);
			ape2[n].peBlue  = (unsigned char)min(255.0, y);
			ape2[n].peGreen = (unsigned char)min(255.0, z);
		}

	}

}

void PutPalette()
{
	float x, y;
	int n;

	if (iDispBits==8)
	{
		//if (--iBlendsLeftInPal == 0)
		//	FX_Random_Palette();
		
		iBlendsLeftInPal--;
        //dumpmsg("p");

		x = (iBlendsLeftInPal / 18.0);
		y = 1-x;

		for (n=0; n<256; n++)
		{
			apetemp[n].peRed =   ape[n].peRed*x + ape2[n].peRed*y;
			apetemp[n].peBlue =  ape[n].peBlue*x + ape2[n].peBlue*y;
			apetemp[n].peGreen = ape[n].peGreen*x + ape2[n].peGreen*y;
		}

		#if (GRFX==1)
			HRESULT ddrval = lpDDPal->SetEntries(0,0,256,apetemp);
			if (ddrval != DD_OK)
				iBlendsLeftInPal++;
			//lpDDPal->SetEntries(0,0,256,ape2);
		#endif
	}
}

float CrankPal(unsigned int curve_id, int z)
{
	float x = (float)z;

    
    /*
	if (SoundActive && !SoundEmpty && FXW==320)
	{
		x *= 0.8;
	}
    */

	switch(curve_id)
	{
	/*	
	case 1:	// good
		if (z>100) return 255; else return sqrtf(z)*25.4;
	case 2: // bright
		if (z<256/2) return z*2; else return 255;
	case 3: // BRIGHT ... better since 2-->1.5
		if (z<256/1.5) return z*1.5; else return 255;
	case 4:
		if (z<256/1.75) return z*1.75; else return 255;
	case 5:
		if (z>130) return 255; else return (z/130.0)*(z/130.0)*254;
		*/
	case 1:
		return sqrtf(x)*22.6;
	case 2:
		return x*2.0;
		//return 255-x;
	case 3:
		return x*x/64.0;
	case 4:
		return 255*sin(x/256.0*0.5*3.1415927);
	case 5:
    	return x*3.5;
    case 6:
		//return pow(2, x/24.0) - 1;
        return pow(1.5, x/20.0) - 1;    // this is really dark!
    case 7:
		//return 20.0*log(x);
    //case 8:
        return x*1.5 + 128*0.25 + 128*0.25*sin(z*0.3);
	}

	return 255.0;
}
















void FX_Screenshot(int fr)
{
  // looks to the arrays VS_R[], VS_G[], VS_B[] to write 
  // a 24-bit dynamically-sized TGA.  Below is special fix for VidEdit
  // (...VidEdit requires at least 320x200 image size)
//              if (xneg>0) xneg=0;
//              if (xpos<639) xpos=639;
//              if (yneg>0) yneg=0;
//              if (ypos<199) ypos=199;
	
  FILE *outfile;

  char filename[512], temp[64];
  sprintf(filename, "fx%06d.tga", fr);
  //sprintf(temp, "fx%06d.tga", fr);
  //strcpy(filename, winpath);
  //strcat(filename, temp);
  outfile = fopen(filename, "wb");

  int x_span = FXW, y_span = FXH-FX_YCUT_HIDE*2;
  unsigned long k, k_end = (FXH-FX_YCUT_HIDE)*FXW;

  if (iDispBits==8)
  {
      // header for 8-bit targa
      fprintf(outfile, "%c%c%c%c%c%c",  0,  1,  1,  0,  0,  0);
      fprintf(outfile, "%c%c%c%c%c%c",  1, 24,  0,  0,  0,  0);
      fprintf(outfile, "%c%c%c%c%c%c", x_span % 256, x_span/256, y_span % 256, y_span/256, 8, 8);
  }
  else
  {
      // header for 24-bit targa
      fprintf(outfile, "%c%c%c%c%c%c",  0,  0,  2,  0,  0,  0);
      fprintf(outfile, "%c%c%c%c%c%c",  0,  0,  0,  0,  0,  0);
      fprintf(outfile, "%c%c%c%c%c%c", x_span % 256, x_span/256, y_span % 256, y_span/256, 32, 32);
  }



  if (iDispBits==8)
  {
      for (k=0; k<256; k++)
          fprintf(outfile, "%c%c%c", ape2[k].peGreen, ape2[k].peBlue, ape2[k].peRed);

      fwrite((void *)(&VS1[FX_YCUT_HIDE*FXW]), FXW*(FXH-FX_YCUT_HIDE*2), 1, outfile);
  }
  else
  {
      fwrite((void *)(&VS1[FX_YCUT_HIDE*FXW*4]), FXW*4*(FXH-FX_YCUT_HIDE*2), 1, outfile);
      /*
      k_end *= 4;
      for (k=FX_YCUT_HIDE*FXW*4; k<k_end; k += FXW*4)
      {
          fprintf(outfile, "%c%c%c", VS1[k], VS1[k+1], VS1[k+2]);
      }
      */
  }


  fclose(outfile);

}


void FX_Loadimage(char *s)
{
  // must be a XXXxXXXx24 Targa file

  FILE *infile;
  int x_span, y_span, bufpos;
  unsigned long k, k_end, offset, offset2;
  unsigned char a,b,c,d,e,f;
  int x, y, ypos, xpos, x_end;
  unsigned char buf[640*3];
  unsigned int w;

  char filename[512];
  //strcpy(filename, winpath);
  //strcat(filename, s);
  strcpy(filename, s);

  dumpmsg(filename);
  if ((infile = fopen(filename, "rb")) != NULL)
  {
	dumpmsg("opened");
    fscanf(infile, "%c%c%c%c%c%c", &a,&b,&c,&d,&e,&f);
    fscanf(infile, "%c%c%c%c%c%c", &a,&b,&c,&d,&e,&f);
    fscanf(infile, "%c%c%c%c%c%c", &a,&b,&c,&d,&e,&f);
    x_span = a+b*256;
    y_span = c+d*256;
	
    if (e==24)
	{
		dumpmsg("e is 24");
		if (iDispBits==8)
		{
		  for (y=0; y<y_span; y++)
		  {
			 ypos = FXH/2 + y_span/2 - y;
			 fread(buf, x_span*3, 1, infile);
			 if ((ypos>=FX_YCUT_HIDE) && (ypos<=FXH-FX_YCUT_HIDE))
			 {
				// copy to VS
				bufpos = 0;
				offset = FXW/2 - x_span/2;
				if (offset < 0)
				{
					bufpos += 3*-(offset);
					offset = 0;
				}
				offset += ypos*FXW;

				x_end = min(x_span, FXW);
				for (x=0; x<x_end; x++)
				{
					c = (buf[bufpos] + buf[bufpos+1] + buf[bufpos+2])/3;
					if (VS1[offset] < c) 
						VS1[offset] = c;
					offset++;
					bufpos += 3;
				}
			 }
		  }
		}
		else  // 32-bit virtual screen
		{
		  for (y=0; y<y_span; y++)
		  {
			 ypos = FXH/2 + y_span/2 - y;
			 fread(buf, x_span*3, 1, infile);
			 if ((ypos>=FX_YCUT_HIDE) && (ypos<=FXH-FX_YCUT_HIDE))
			 {
				// copy to VS
				bufpos = 0;
				offset = FXW/2 - x_span/2;
				if (offset < 0)
				{
					bufpos += 3*-(offset);
					offset = 0;
				}
				offset += ypos*FXW;
				offset *= 4;

				x_end = min(x_span, FXW);
				for (x=0; x<x_end; x++)
				{
					a = buf[bufpos  ];
					b = buf[bufpos+1];
					c = buf[bufpos+2];
					if (VS1[offset  ] < c) VS1[offset  ] = a;
					if (VS1[offset+1] < c) VS1[offset+1] = b;
					if (VS1[offset+2] < c) VS1[offset+2] = c;
					offset += 4;
					bufpos += 3;
				}
			 }
		  }
		}
	}
    fclose(infile);

  }

  frames_since_last_plop = 0;
}





#endif