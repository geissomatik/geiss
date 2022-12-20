
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
