
#ifndef GEISS_DEFINES
#define GEISS_DEFINES 1



#define weightsum 253


#define NUM_EFFECTS 8

extern int effect[];
enum { CHASERS, BAR, DOTS, SOLAR, GRID, NUCLIDE, SHADE };


#define NUM_MODES 11       // warning!  modes start at 1, not zero!


#define NUM_WAVES 5
#define WAVE_5_BLEND_RANGE   50
const int MINBUFSIZE         = ((314+WAVE_5_BLEND_RANGE)*2 + 20);


#endif