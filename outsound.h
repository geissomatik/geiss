

#ifndef OUTSOUND
#define OUTSOUND 1


#include <dsound.h>

//------------------------------
// external variables/functions:
//------------------------------
extern void dumpmsg(char *s);
//------------------------------



int Outsound_Init(HWND hwnd);
void Outsound_Proc(float *g_fSoundLeft, int iSamples);
void Outsound_Fini();


#endif
