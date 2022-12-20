/* defines */
#define TRUE      1
#define FALSE     0

/* Structures */

typedef struct tagCPUInfo
  {
    char CPUName[49];
    char VendorString[13];
    unsigned long Features;
    unsigned char CPUID;
    unsigned char MMX;
    unsigned char TSCounter;
    unsigned char FPU;
    unsigned int Stepping;
    float MHz;
  } CPUInfo;


/* Prototypes */

int     IsCyrixCPU (void);
int     IsCyrixFPU (void);
float   cpu_mhz (void);
void    GetCPUName ( char* );
void    GetCPUInfo ( CPUInfo* );
