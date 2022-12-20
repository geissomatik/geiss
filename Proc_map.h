
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