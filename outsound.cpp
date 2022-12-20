// This file is part of: Geiss screensaver and Winamp music visualization plug-in
//                           ___ ___ ___ ___ ___
//                          / __| __|_ _/ __/ __|
//                         | (_ | _| | |\__ \__ \
//                          \___|___|___|___/___/
// BSD 3-Clause License
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

#include "outsound.h"
#include <math.h>




#define OUTSOUND_FOURIER_DETAIL 64
double			outsound_power[OUTSOUND_FOURIER_DETAIL];
double			outsound_power_average[OUTSOUND_FOURIER_DETAIL];
LPDIRECTSOUND	lpDirectSound = NULL; 



int Outsound_Init(HWND hwnd)
{
	// see 'access to the primary buffer' for better init
	// see 'access to the primary buffer' for better init
	// see 'access to the primary buffer' for better init
	HRESULT       hr;

	hr = DirectSoundCreate(NULL, &lpDirectSound, NULL);
	if (hr != DS_OK) 
	{
		dumpmsg("Outsound: DirectSoundCreate() failed.");
		return 0;
	}
	
	hr = lpDirectSound->SetCooperativeLevel(hwnd, DSSCL_WRITEPRIMARY);
	if (hr != DS_OK) 
	{
		dumpmsg("Outsound: SetCooperativeLevel() failed.");
		return 0;
	}



	memset(outsound_power, 0, sizeof(double)*OUTSOUND_FOURIER_DETAIL);
	memset(outsound_power_average, 0, sizeof(double)*OUTSOUND_FOURIER_DETAIL);
	return 1;
}

void Outsound_Proc(float *g_fSoundLeft, int iSamples)
{
	int n, i;
	double a, b, power, theta;

	for (n=1; n<OUTSOUND_FOURIER_DETAIL; n++)
	{
		a = 0;
		b = 0;
		double w = 6.28 * (20*powf(2, n/(double)OUTSOUND_FOURIER_DETAIL*10)/44100.0);
		for (i=0; i<iSamples; i++)
		{
			theta = w*i;
			a += (g_fSoundLeft[i] * cos(theta));
			b += (g_fSoundLeft[i] * sin(theta));
		}
		//g_power[n] = max(sqrt(a*a + b*b), g_power[n]*0.95);
		power = sqrt(a*a + b*b);
		outsound_power[n] = power;//outsound_power[n]*0.5 + power*0.5;
		outsound_power_average[n] = outsound_power_average[n]*0.99 + power*0.01;
	}

	// re-cast right channel as left (space->freq->space)
	//   warning: for best accuracy, set g_power[n] above to
	//   actual current power - don't attenuate.
	/*
	memset(&g_fSoundRight[0], 0, 512*sizeof(float));
	for (n=1; n<OUTSOUND_FOURIER_DETAIL; n++)
	{
		a = 0;
		b = 0;
		float w = 6.28 * (20*powf(2, n/(float)FOURIER_DETAIL*10)/44100.0);
		for (i=0; i<512; i++)
		{
			theta = w*i;
			g_fSoundRight[i] += g_power[n] * cos(theta);
					          + g_power[n] * sin(theta);
		}
	}
	*/

}

void Outsound_Fini()
{
	if (lpDirectSound != NULL)
	{
		lpDirectSound->Release();
		lpDirectSound = NULL;
	}
}


