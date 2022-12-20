#define TTPS          1193182     //high speed Timer Ticks per second in Mhz
#define CLKs_Per_MHz  1000000     //number of clocks in 1 Mhz
#define NUM_LOOPS     0x2000      //core loop iterations
#define RUNS          10          //number of runs to average
#define DIVS          5           //# of IDIV instructions in the core loop

#define Cx486_core_IDIV_CLKS   40 //clock counts for Cx486 IDIV
#define Cx486_core_LOOP_CLKS   8  //clock counts for Cx486 LOOP
#define _5x86_core_IDIV_CLKS   45 //clock counts for 5x86 and MediaGX IDIV
#define _5x86_core_LOOP_CLKS   2  //clock counts for 5x86 and MediaGX LOOP
#define _6x86_core_IDIV_CLKS   17 //clock counts for 6x86,6x86MX, and MII IDIV
#define _6x86_core_LOOP_CLKS   1  //clock counts for 6x86, 6x86MX and MII LOOP

