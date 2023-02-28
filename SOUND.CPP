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

#ifndef GEISS_SOUND
#define GEISS_SOUND 1



#include "sound.h"
#include <dsound.h>
#include <math.h>


void GetWaveData()
{
	float fDiv = (1.0/(64.0 * (640.0/(float)FXW) ));
    float billy = volscale * fDiv;
	int i;
	HRESULT hr;


	if (SoundReady && SoundActive)
	{

#if SAVER

		void *pData[2];
		DWORD size[2];
		pData[0] = NULL;
		pData[1] = NULL;
		size[0] = 0;
		size[1] = 0;
		DWORD a, b;
		pDSCB->GetCurrentPosition(&a, &b);	// record pos. (changes always), playback pos. (sometimes sticks)

		//static float s_last_clock;
		//int offset = 44100*4*((clock() - s_last_clock)/(float)CLOCKS_PER_SEC);

		//FILE *infile = fopen("c:\\a.txt", "a");
		//fprintf(infile, "%d %d %p %p\n", size[0], size[1], pData[0], pData[1]);
		//fprintf(infile, "%d %d\n", a, b);
		//fclose(infile);
		
		int offset = 0;
		int new_b = b;

		// if wave input stalled, read further into prev. wave data
		//-----------------------------------------------------------------------------
		
		if (b==oldSoundBufPos)
		{
			if (oldSoundBufNum<0) oldSoundBufNum++;
			offset = ((BUFSIZE))*oldSoundBufNum;
		}
		else
		{
			oldSoundBufNum = -3;		// was -2 in first v3.02 release
			oldSoundBufPos = b;
			offset = ((BUFSIZE))*oldSoundBufNum;
		}
		new_b = (b + offset);
		if (new_b < 0) new_b += 176400;
		if (new_b >= 176400) new_b -= 176400;

		// LOCK BUFFER
		hr = pDSCB->Lock(new_b, ((BUFSIZE)), &pData[0], &size[0], &pData[1], &size[1], NULL);
		if (hr != DS_OK)
		{
			dumpmsg("pDSCB->Lock() failed");
			g_QuitASAP = true;
			//exit(880);
		}

		// COPY DATA
		unsigned char *readpos = (unsigned char *)pData[0];
		memcpy(g_SoundBuffer, readpos, size[0]);
		if (size[1] > 0)
		{
			readpos = (unsigned char *)pData[1];
			memcpy(g_SoundBuffer + size[0], readpos, size[1]);
		}

		// UNLOCK BUFFER
		hr = pDSCB->Unlock(pData[0], size[0], pData[1], size[1]);
		if (hr != DS_OK)
		{
			dumpmsg("pDSCB->Unlock() failed");
			g_QuitASAP = true;
			//exit(881);
		}

#endif
#if PLUGIN

        int FXW_DIV_4 = FXW/4;
        int x = 0;
        int y = 5;
        int v;
        int old_v;

        if (visMode != spectrum)
        {
            // --------------- begin level trigger -------------------
            // --------------- begin level trigger -------------------

            y += FXW_DIV_4;                     // start @ middle

            while (y<(511-365+FXW_DIV_4)) // 365 is max index # we'll ever plot (wave #5)        // start @ middle
            {
                old_v = ((int)(g_this_mod->waveformData[0][y-5] ^ 128) - 128);
                v     = ((int)(g_this_mod->waveformData[0][y] ^ 128) - 128);
                if ( 
                    (abs(v - last_frame_v) <= 1)
                    &&
                    ( (last_frame_slope>0 && abs((v-old_v)>0)) || (last_frame_slope<=0 && abs((v-old_v)<=0)) )
                   )
                {
                    last_frame_slope = v - old_v;
                    last_frame_v = v;
                    break;
                }
                y++;
            }

            if (y>=(511-365+FXW_DIV_4))       // start @ middle
            {
                y = 5 + FXW_DIV_4;//5;        // start @ middle
                old_v = ((int)(g_this_mod->waveformData[0][y-5] ^ 128) - 128);
                v     = ((int)(g_this_mod->waveformData[0][y] ^ 128) - 128);
                last_frame_slope = v - old_v;
                last_frame_v = v;
            }

            y -= FXW_DIV_4;                     // start @ middle

            // --------------- end of level trigger -------------------
            // --------------- end of level trigger -------------------
        }        


       
        // now fill in g_SoundBuffer[]
        if (visMode == spectrum)
        {
	        for ( ; x<BUFSIZE && y<512; x+=2, y++)
	        {
		        g_SoundBuffer[x] = ((int)(256-g_this_mod->spectrumData[0][y]) ) << 10;
		        g_SoundBuffer[x+1] = ((int)(256-g_this_mod->spectrumData[1][y]) ) << 10;
	        }
        }
        else
        {
	        for ( ; x<BUFSIZE && y<512; x+=2, y++)
	        {
		        g_SoundBuffer[x] = ((int)(g_this_mod->waveformData[0][y] ^ 128) - 128) << 8;
		        g_SoundBuffer[x+1] = ((int)(g_this_mod->waveformData[1][y] ^ 128) - 128) << 8;
	        }
        }


#endif

        // ------------- do any necessary formatting to sound wave data ---------------

        float center[2];

		// smoothing shows the bass hits more than treble
		for (i=0; i<BUFSIZE-2; i++) g_fSoundBuffer[i] = 0.8*g_SoundBuffer[i] + 0.2*g_SoundBuffer[i+2];
        //for (i=0; i<BUFSIZE-2; i++) g_fSoundBuffer[i] = 0.15*g_SoundBuffer[i] + 0.85*g_SoundBuffer[i+2];

        // now scale sound by volscale & screen-size-scale factor (fDiv)
		for (i=0; i<BUFSIZE; i++) g_fSoundBuffer[i] *= billy;

#if PLUGIN
        if (visMode != spectrum)
#endif
        {
            // get centroid for L, R channels
            center[0] = 0;
            center[1] = 0;
            for (i=0; i<BUFSIZE; i+=8) center[0] += g_fSoundBuffer[i];
            for (i=1; i<BUFSIZE; i+=8) center[1] += g_fSoundBuffer[i];
            center[0] /= (float)FXW*0.25;
            center[1] /= (float)FXW*0.25;

            // now translate wave so center is at zero.
            for (i=0; i<BUFSIZE; i+=2) g_fSoundBuffer[i] -= center[0];
            for (i=1; i<BUFSIZE; i+=2) g_fSoundBuffer[i] -= center[1];
        }
    }
}

void RenderDots(unsigned char *VS1)
{
	if (SoundReady && SoundActive)// && !SoundEmpty)
	{
		// test for treble hit
		
		int peaks=0, i=0, high, low;
		float vol=0;

	    /*
		i=FXW-4;
		while (i>2)
		{
			while (i>2 && g_SoundBuffer[i*2+2] > g_SoundBuffer[i*2])
			{
				i--;
			}
			low = g_SoundBuffer[i*2];
			i--;
			while (i>2 && g_SoundBuffer[i*2+2] < g_SoundBuffer[i*2])
			{
				i--;
			}
			i--;
			high = g_SoundBuffer[i*2];

			peaks++;
			vol += high-low;
		}
        */

        low = g_SoundBuffer[0];
        high = g_SoundBuffer[0];

        i = BUFSIZE - 4;
        while (i>0)
        {
            low = min(low, g_SoundBuffer[i]);
            high = max(high, g_SoundBuffer[i]);
            i -= 3;
        }

        vol = (high - low) / 256.0;

        //dumpmsg("%f", vol);

        // saver: low & high are around 10000-20000 (pos. or neg.)
        //        vol is around 80-120


       
        //------------------------------
        // for current_vol:
        //------------------------------
        // plugin:  0/1 = silent
        //          50  = soft
        //          100-150 = normal
        //          200 = loud
        //          255 = max (why?)
        // 
        // saver:   80-120 = normal
        //------------------------------
        //sprintf(szH1, "%d/%d (%5.3f)", low, high, vol); //beely!!!


		//if (peaks>0) vol /= (float)peaks;

        past_vol_pos = (past_vol_pos + 1) % PAST_VOL_N;
        past_vol[past_vol_pos] = current_vol;

        current_vol  = vol;
        avg_vol_narrow=avg_vol*0.3 + vol*0.7;
		avg_vol      = avg_vol*0.85 + vol*0.15;
		avg_vol_wide = avg_vol_wide*0.96 + vol*0.04;
		avg_peaks    = avg_peaks*0.9 + peaks*0.1;



        #if PLUGIN
            g_hit = max(g_hit-1, -1);
            if (!g_bDisableSongTitlePopups && g_hit == -1 && rand()%7000 < g_random_songtitle_freq*g_random_songtitle_freq) 
            {
                g_hit = g_song_tooltip_frames;
                g_title_R = 128 + rand() % 99;
                g_title_G = 128 + rand() % 99;
                g_title_B = 128 + rand() % 99;
            }
        #endif

        //if (vol > avg_vol*3.5 && vol > avg_vol_wide*3.0) g_hit = 10;
        //if (g_hit == -1 && vol > avg_vol_wide*3.6) g_hit = 5;

		if (effect[NUCLIDE]>0 && !SoundEmpty)
		{
			if (vol > avg_vol*1.25)
			{
				int nodes = 3 + rand()%5;
				int n;
				int phase = rand()%1000;
				int rad;
				int x,y,cx,cy,str;
                int val;
				int r;// = rand()%8 + 3;  // MAX is **10**

                if (FXW==320)
                    r = 2 + 20*(vol/avg_vol - 1.25);
                else
                    r = 3 + 32*(vol/avg_vol - 1.25);
                if (r<1) r=1;
                if (FXW==320 && r>7) r=7;
                if (FXW!=320 && r>10) r=10;

				if (FXW==320)
					rad = 22 + rand()%6; //min(35, 14 + 0.7*vol);//(20 + rand()%30);
				else
					rad = 34 + rand()%8; //min(50, 20 + vol);//(20 + rand()%30);

				float cr=1.0, cg=1.0, cb=1.0;
				if (iDispBits>8)
				{
					int intfram = intframe + chaser_offset;
					float f = 7*sin(intfram*0.007+29) + 5*cos(intfram*0.0057+27);
					cr = 0.58 + 0.21*sin(intfram*gF[0] +20-f) + 0.21*cos(intfram*gF[3] +17+f);
					cg = 0.58 + 0.21*sin(intfram*gF[1] +42+f) + 0.21*cos(intfram*gF[4] +26-f);
					cb = 0.58 + 0.21*sin(intfram*gF[2] +57-f) + 0.21*cos(intfram*gF[5] +35+f);
				}

				long T_offset;
				int pixelsize = (iDispBits==8) ? 1 : 4;

				for (n=0; n<nodes; n++)
				{
					cx = gXC + rad*cos(n/(float)nodes * 6.28 + phase);
					cy = gYC + rad*sin(n/(float)nodes * 6.28 + phase);

                    int sqrt_pos = 0;
                   
					for (y=-10; y<=10; y++)
                    {
                        T_offset = ((cy+y)*FXW + (cx-10)) * pixelsize;
					    for (x=-10; x<=10; x++)
					    {
						    val = (r - sqrt(x*x+y*y))*25;
						    if (val>0)
						    {
    						    //T_offset = y*FXW + x;
							    if (iDispBits==8)
							    {
                                    str = VS1[T_offset] + val;
								    VS1[T_offset] = min(255, str);
							    }
                                else
                                {
                                    str = VS1[T_offset  ] + val*cr;
							        VS1[T_offset  ] = min(255, str);
                                    str = VS1[T_offset+1] + val*cg;
							        VS1[T_offset+1] = min(255, str);
                                    str = VS1[T_offset+2] + val*cb;
							        VS1[T_offset+2] = min(255, str);
                                }
						    }
                            T_offset += pixelsize;
					    }
                    }
				}
            }
        }
    }
}

void RenderWave(unsigned char *VS1)
{
	int xL, xR, yL, yR, i, total=0, prev=0;
	float zL, prev_zL, zR, prev_zR;
	long D_offset;
	int fact = 2;//(FXW==320) ? 1 : 2;
    int base = 150;
    
    //dumpmsg("%f", current_vol);

    base = (current_vol*4  + avg_vol * 0.4) - 10;
    //base = (current_vol*4.5  + avg_vol * 0.45) - 10;

    /*
    //-------------------------------------------
    base = (avg_vol_narrow*4  + avg_vol * 0.4) - 12;

    static int big_base;

    float avg_vol_uniform = 0;
    for (i=0; i<PAST_VOL_N; i++) avg_vol_uniform += past_vol[i];
    avg_vol_uniform /= (float)PAST_VOL_N;
    float variance = 0;
    for (i=0; i<PAST_VOL_N; i++) variance += past_vol[i]*past_vol[i];
    variance = (variance - avg_vol_uniform*avg_vol_uniform) * (1.0f/((float)PAST_VOL_N - 1));
    float std_dev = sqrt(variance);

    // normally, we'd test
    //      if (current_vol - avg_vol_uniform > std_dev*0.2)
    // but instead, we're going to adjust the *0.2 multiplier based on
    // the # of beats we've heard in the last several frames.

    // past_beat[] stores TRUE wherever the volume exceeded the std_dev*mult,
    // so we have to weed out the loud -series- to count the # of --beats--.
    int beats = 0;
    bool last_beat = 0;
    for (i=0; i<PAST_VOL_N*2; i++) 
        if (past_beat[i] && !last_beat)
        {
            beats++;
            last_beat = true;
        }
        else last_beat = false;

    // < 4 beats: too few - multiplier is 0.1
    //   4 beats: ideal - multiplier is 0.1675
    // > 4 beats: too many - multiplier is 0.25
    float mult = 0.11 + 0.2*(beats*0.4);
    if (mult > 0.4) mult = 0.4;

    if (current_vol - avg_vol_uniform > std_dev*mult)
    {
        past_beat_pos = (past_beat_pos+1) % (PAST_VOL_N*2);
        past_beat[past_beat_pos] = true; // record this as a loud point

        base *= 10.0f*(std_dev-mult);   // at 0.3 and above it will be 100%.
        if (base>155) base = 155;
    }
    else
    {
        past_beat_pos = (past_beat_pos+1) % (PAST_VOL_N*2);
        past_beat[past_beat_pos] = false; // record this as a dead point

        base = 0;
    }

    if (base > big_base)
    {
        big_base = base;
    }
    else
    {
        int decr = 30;
        //int decr = 200;//25 + beats*3;    // this controls the falloff after a beat
        if (big_base > decr) big_base -= decr; else big_base = 0;
        base = big_base;
    }

    // now look at past_beat[] and see if the average deviation of the # of frames
    // between beats is less than 3... if so, keep 'beat mode' brightness;
    // otherwise, use old-style (volume-dictated) wave brightness.
    
    // ....
    // ,,,,
    // ,,,,
    //-------------------------------------------
    */

    if (base>155) base = 155;
    if (base<0) base = 0;

#if SAVER
    // set SoundEmpty
    if (base==0)
    {
		frames_since_silence++;
		if (frames_since_silence > frames_til_auto_switch*2)
		{
			SoundEmpty = true;
        }
	}
	else
	{
		frames_since_silence=0;
		SoundEmpty = false;
	}
#endif    

	//float fDiv = 1.0/(256.0*(640.0/(float)FXW));
	//float fDiv = (1.0/(128.0*(320.0/(float)FXW)));

	unsigned char r, g, b;
	r = base;
	g = base;
	b = base;
	if (iDispBits>8)
	{
        int ir, ig, ib;
		int intfram = intframe + chaser_offset;
		float f = 7*sin(intfram*0.006+59) + 5*cos(intfram*0.0077+17);
		ir = base*1.07 * (1 + 0.3*sin(intfram*gF[0] +10-f)) * (1 + 0.20*cos(intfram*gF[1] +37+f));
		ig = base*1.07 * (1 + 0.3*sin(intfram*gF[2] +32+f)) * (1 + 0.20*cos(intfram*gF[3] +16-f));
		ib = base*1.07 * (1 + 0.3*sin(intfram*gF[4] +87-f)) * (1 + 0.20*cos(intfram*gF[5] +25+f));
        if (ir<0) ir=0;
        if (ir>255) ir=255;
        if (ig<0) ig=0;
        if (ig>255) ig=255;
        if (ib<0) ib=0;
        if (ib>255) ib=255;
        r = ir;
        g = ig;
        b = ib;

		//float mag = 150.0/sqrt((int)r*(int)r + (int)g*(int)g + (int)b*(int)b);
		//r *= mag;
		//g *= mag;
		//b *= mag;
	}

	if (SoundReady && SoundActive && !SoundEmpty)
	{
		if (waveform==1)
		{
            int y_center = FXH/2;
            int start = 0;
            int end = FXW;

            if (mode==10) 
            {
                y_center = ((FXH - FX_YCUT) + (FXH*0.5)) * 0.5;
#if PLUGIN
                if (visMode == spectrum)
                {
                    y_center = 0.9*(FXH - FX_YCUT) + 0.1*(FXH*0.5);
                }
#endif
                start += 10;
                end -= 10;
                if (FXW >= 640)
                {
                    start += 5;
                    end -= 5;
                }
            }
			//zL = g_fSoundBuffer[0] + y_center;
            zL = g_fSoundBuffer[start^(start&1)] + y_center;
			
            for (i=start; i<end; i++)  
			{
				prev_zL = zL;
				zL = g_fSoundBuffer[i^(i&1)] + y_center;
				zL = prev_zL*0.9 + zL*0.1;
				yL = (int)zL;

                if (yL >= FX_YCUT_HIDE && yL <= FXH-FX_YCUT_HIDE)
                {
                    D_offset = FXW*yL + i;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }                
                }
			}
		}
		else if (waveform==2)
		{
            float fDiv = 0.7;
            float h1 = FXH*0.4;
            float h2 = FXH*0.6;
			zL = g_fSoundBuffer[0] * fDiv + h1;
			zR = g_fSoundBuffer[1] * fDiv + h2;
			for (i=0; i<FXW; i++)  
			{
				prev_zL = zL;
				prev_zR = zR;
				zL = g_fSoundBuffer[i^(i&1)] * fDiv + FXH*0.4;
				zR = g_fSoundBuffer[i^(i&1)+1] * fDiv + FXH*0.6;
				zL = prev_zL*0.9 + zL*0.1;
				zR = prev_zR*0.9 + zR*0.1;
				yL = (int)zL;
				yR = (int)zR;
				if (yL>FX_YCUT_HIDE && yL<FXH-FX_YCUT_HIDE) 
				{
					D_offset = FXW*yL + i;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }
				}
				if (yR>FX_YCUT_HIDE && yR<FXH-FX_YCUT_HIDE) 
				{
					D_offset = FXW*yR + i;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }
				}
			}
		}
		else if (waveform==3)
		{
			zL = g_fSoundBuffer[FX_YCUT_HIDE^(FX_YCUT_HIDE & 1)] + FXW/2;
			for (i=FX_YCUT_HIDE; i<FXH-FX_YCUT_HIDE; i++)  
			{
				prev_zL = zL;
				zL = g_fSoundBuffer[i^(i&1)] + FXW/2;
				zL = prev_zL*0.9 + zL*0.1;
				xL = (int)zL;
				if (xL>=0 && xL<FXW)
				{
					D_offset = FXW*i + xL;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }
				}
			}
		}
		else if (waveform==4)
		{
            float fDiv = 0.9;
			zL = g_fSoundBuffer[FX_YCUT_HIDE^(FX_YCUT_HIDE & 1)] * fDiv;
			zR = g_fSoundBuffer[FX_YCUT_HIDE^(FX_YCUT_HIDE & 1)+1] * fDiv;
			for (i=FX_YCUT_HIDE; i<FXH-FX_YCUT_HIDE; i++)  
			{
				prev_zL = zL;
				prev_zR = zR;
				zL = g_fSoundBuffer[i^(i&1)] * fDiv;
				zR = g_fSoundBuffer[i^(i&1)+1] * fDiv;
				zL = prev_zL*0.9 + zL*0.1;
				zR = prev_zR*0.9 + zR*0.1;
				xL = (int)zL + i;
				xR = (int)zR + i + (FXW-FXH);
				if (xL>=0 && xL<FXW)
				{
					D_offset = FXW*i + xL;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }
				}
				if (xR>=0 && xR<FXW)
				{
					D_offset = FXW*i + xR;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }
				}
			}
		}
		else if (waveform==5)
		{
			int px, py;
			float range_inv = 1.0/WAVE_5_BLEND_RANGE;
			float amt;
            float rad;
            float base_rad = (FXW==320) ? 40.0 : 60.0;
            float fDiv = 0.7;

            for (i=0; i<WAVE_5_BLEND_RANGE; i++)  
            {
				amt = i * range_inv;
				g_fSoundBuffer[i^(i&1)] = g_fSoundBuffer[i^(i&1)]*(amt) + (1-amt)*(g_fSoundBuffer[(i+314)^((i+314)&1)]);
            }
            
			
            rad = base_rad + g_fSoundBuffer[0] * fDiv;
            for (i=0; i<314; i++)  
			{
				rad = rad*0.5 + 0.5*(base_rad + g_fSoundBuffer[i^(i&1)] * fDiv);
                if (rad>=5)
                {
				    px = (float)gXC + rad*cos(i*0.02);
				    py = (float)gYC + rad*sin(i*0.02);
				    if (px>=0 && px<FXW && py>=FX_YCUT && py<FXH-FX_YCUT)
				    {
					    D_offset = FXW*py + px;
                        if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                        else 
                        { 
                            D_offset *= 4; 
                            if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                            if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                            if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                        }
				    }
                }
			}
		}
        else if (waveform==6)
        {
			int px, py;
			float range_inv = 1.0/WAVE_5_BLEND_RANGE;
			float amt;
            float rad;
            float base_rad = (FXW==320) ? 40.0 : 60.0;
            float fDiv = 0.7;

            for (i=0; i<WAVE_5_BLEND_RANGE; i++)  
            {
				amt = i * range_inv;
				g_fSoundBuffer[i^(i&1)] = g_fSoundBuffer[i^(i&1)]*(amt) + (1-amt)*(g_fSoundBuffer[(i+314)^((i+314)&1)]);
            }
            
			
            rad = base_rad + g_fSoundBuffer[0] * fDiv;
            float px2, py2, ang;
            ang = sin(intframe*0.01);
            for (i=0; i<314; i++)  
			{
				px2 = g_fSoundBuffer[i^(i&1)];
				py2 = g_fSoundBuffer[i^(i&1)+1];
                px = px2*cos(ang) + py2*sin(ang);
                py = px2*-sin(ang) + py2*cos(ang);
                px += (float)gXC;
                py += (float)gYC;
				if (px>=0 && px<FXW && py>=FX_YCUT && py<FXH-FX_YCUT)
				{
					D_offset = FXW*py + px;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<r) VS1[D_offset  ] = r; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<b) VS1[D_offset+2] = b; 
                    }
				}
			}
        }
	}
}



#endif