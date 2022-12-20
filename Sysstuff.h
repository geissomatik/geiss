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

#ifndef GEISS_SYSTEM_STUFF
#define GEISS_SYSTEM_STUFF 1



// --------------------------------------------------------------------------
bool CheckMMXTechnology()
{
    bool retval = TRUE;
    DWORD RegEDX;

    __try {
        __asm {
            mov eax, 1
            cpuid
            mov RegEDX, edx
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        retval = FALSE;
    }

    if (retval == FALSE) return FALSE;  // processor does not support CPUID

    if (RegEDX & 0x800000)          // bit 23 is set for MMX technology
    {
       __try { __asm emms }          // try executing the MMX instruction "emms"
       __except(EXCEPTION_EXECUTE_HANDLER) { retval = FALSE; }
    }

    else
            return FALSE;           // processor supports CPUID but does not support MMX technology

    // if retval == 0 here, it means the processor has MMX technology but
    // floating-point emulation is on; so MMX technology is unavailable

    return retval;
}

/*
bool CyrixInstead()
{
    bool retval = false;

    unsigned int cpuid_present;

    __try 
    {
        __asm
        {
            
            //pushfd ; get extended flags 
            //pop eax ; store extended flags in eax 
            //mov ebx, eax ; save current flags 
            //xor eax, 200000h ; toggle bit 21 
            //push eax ; put new flags on stack 
           //- pushfd ; get extended flags 
            //pop eax ; store extended flags in eax 
            //xor eax, ebx ; if bit 21 r/w then eax <> 0 
            //popfd ; flags updated now in flags 
            //// je no_cpuid ; can't toggle id bit (21) no cpuid here 
            //mov   cpuid_present, eax
            

            pushfd ; get extended flags 
            pop eax ; store extended flags in eax 
            mov ebx, eax ; back them up in ebx
            xor eax, 200000h ; toggle bit 21 
            push eax ; put new flags on stack 
            popfd ; set EFLAGS
            pushfd ; get EFLAGS that actually took root
            pop eax ; store extended flags in eax 
            xor eax, ebx ; if bit 21 changed then eax != 0 (and CPUID is supported)
            
            ; restore flags to original settings
            push ebx 
            popfd

            mov   cpuid_present, eax
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // this should never happen!
        dumpmsg("CPU DETECTION FAILED - assuming it's a Cyrix 5x86");
        retval = 1;   // assume Cyrix
    }

    if (cpuid_present==0)
    {
        dumpmsg("CPUID not supported...");  // new in 3.61 beta 2
        //MessageBox( NULL, "CPUID is NOT supported, using 5/2 method", "NOTE", MB_OK );
    }
    else
    {
        dumpmsg("CPUID supported...");  // new in 3.61 beta 2
        //MessageBox( NULL, "CPUID is supported... ready to use it.", "NOTE", MB_OK );
    }

    char CpuIdString[13] = "xxxxxxxxxxxx";

    __asm
    {
        mov eax, cpuid_present
        cmp eax, 0
        je no_cpuid ; couldn't toggle id bit (21) in EFLAGS - no cpuid support

        xor eax, eax  ; set eax=0
        CPUID

        mov dword ptr [CpuIdString], ebx
        mov dword ptr [CpuIdString+4], edx
        mov dword ptr [CpuIdString+8], ecx
        mov byte ptr [CpuIdString+12], 0
        jmp end

      //----------------------------------------

      no_cpuid:       ; use 5/2 method

        xor ax, ax    ; clear ax
        sahf          ; clear flags, bit 1 is always 1 in flags
        mov ax, 5     ; move 5 into the dividend
        mov bx, 2     ; move 2 into the divisor
        div bl        ; do an operation that does not change flags
        lahf          ; get flags
        cmp ah, 2     ; check for change in flags
        jne end       ; flags changed, not a Cyrix CPU

          ; we have Cyrix!
          mov  retval, 1

        end:
    }

    if (retval==0)
    {
        if (!strcmp(CpuIdString, "CyrixInstead"))
        {
            retval = 1;
        }
    }
    dumpmsg("CPU detection success.");


    return retval;
}
*/

// --------------------------------------------------------------------------



void GetWindowsPath()
{
	#define RESPATH     "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"

    HKEY key;
    long result;
    //char buf[512];
    DWORD type, size;

    result = RegOpenKeyEx(HKEY_CURRENT_USER, RESPATH, 0, KEY_READ, &key);
    if (result == ERROR_SUCCESS) 
	{
        size = sizeof(winpath);
	    result = RegQueryValueEx(key, "SystemRoot", NULL, &type, (LPBYTE) winpath,
	                         &size);
	    RegCloseKey(key);
	    if (result == ERROR_SUCCESS && type == REG_SZ) // REG_SZ means a null-terminated string.
			strcat(winpath, "\\");
		else
			strcpy(winpath, "");
	}
	else
	{
		strcpy(winpath, "");
	}
}



#if PLUGIN
int PreReadDelayFromRegistry()
{

  LONG res; 
  HKEY skey; 
  DWORD valtype, valsize, val;


  res=RegOpenKeyEx(HKEY_CURRENT_USER,APPREGPATH,0,KEY_ALL_ACCESS,&skey);
  if (res!=ERROR_SUCCESS) return 0;
  
      valsize=sizeof(val); 

      res=RegQueryValueEx(skey,"FrameDelay",0,&valtype,(LPBYTE)&val,&valsize);   
      if (res==ERROR_SUCCESS) iRegistryDelay=val; else dumpmsg("error reading reg: FrameDelay");// { ScrMode = smConfig; g_bFirstRun = true; }

  RegCloseKey(skey);

  return iRegistryDelay;
}
#endif

void OnFxwFxhUpdated()
{
	//FX_YCUT could be zero, but that doesn't leave us with many black lines,
	// and our borders end up looking stupid...
	FX_YCUT             = max(4, (65 - 0.65*iVSizePercent)*0.5*0.01*FXH);//(65 - 0.60*iVSizePercent)*0.5*0.01*FXH
	FX_YCUT_HIDE		= FX_YCUT;// + 2;
	FX_YCUT_NUM_LINES   = FXW*(FXH-FX_YCUT*2);
	FX_YCUT_xFXW_x8     = FX_YCUT*FXW*8;
	FX_YCUT_xFXW        = FX_YCUT*FXW;
	FX_YCUT_HIDE_xFXW	= FX_YCUT_HIDE*FXW;
  FXW_x_FXH           = FXW*FXH;
  BUFSIZE             = max(FXW*2, MINBUFSIZE);
  #if PLUGIN
    BUFSIZE             = min(1023, BUFSIZE);   // because plugin only gives us 512 for each channel.
  #endif
}

void ReadConfigRegistry()
{ 
  LONG res; 
  HKEY skey; 
  DWORD valtype, valsize, val;

  char str[64];
  int i;

  res=RegOpenKeyEx(HKEY_CURRENT_USER,APPREGPATH,0,KEY_ALL_ACCESS,&skey);
  if (res!=ERROR_SUCCESS) return;

  
  valsize=sizeof(val); 

  //res=RegQueryValueEx(skey,"VideoMode3",0,&valtype,(LPBYTE)&val,&valsize);   
  //if (res==ERROR_SUCCESS) VidMode=val; 

    //if (VidMode < 0) VidMode = 0;
	//if (VidMode >= iNumVideoModes) VidMode = 0;
    //FXW = BufferWidth[VidMode];
    //FXH = BufferHeight[VidMode];
    //CurVideoHeight = VideoHeight[VidMode];		// screen size
    //CurVideoWidth  = VideoWidth[VidMode];
    //iDispBits = VidModeBitDepth[VidMode];

  res=RegQueryValueEx(skey,"Screen_Vsize_Percent",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) iVSizePercent = val; else dumpmsg("error reading reg: iVSizePercent");//{ ScrMode = smConfig; g_bFirstRun = true; }
  
    iVSizePercent = min(iVSizePercent, 100);
    iVSizePercent = max(iVSizePercent, 0);

  DWORD valtype_guid = REG_BINARY;
  DWORD valsize_guid = sizeof(GUID);
  res=RegQueryValueEx(skey,"DDrawDeviceGUID",0,&valtype_guid,(LPBYTE)&g_DDrawDeviceGUID,&valsize_guid);   
  if (res!=ERROR_SUCCESS) 
    memset(&g_DDrawDeviceGUID, 0, sizeof(GUID));

  //res=RegQueryValueEx(skey,"16bit_Video_Fix",0,&valtype,(LPBYTE)&val,&valsize);   
  //if (res==ERROR_SUCCESS) VIDEO_CARD_FUCKED=val;
  //dither//res=RegQueryValueEx(skey,"Dither",0,&valtype,(LPBYTE)&val,&valsize);   
  //dither//if (res==ERROR_SUCCESS) bDither=val; else dumpmsg("error reading reg: Dither");// { ScrMode = smConfig; g_bFirstRun = true; }
  //dither//res=RegQueryValueEx(skey,"RandomDither",0,&valtype,(LPBYTE)&val,&valsize);   
  //dither//if (res==ERROR_SUCCESS) bRandomDither=val; else dumpmsg("error reading reg: RandomDither");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"iCurSoundDriver",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) iCurSoundDriver=val; else dumpmsg("error reading reg: iCurSoundDriver");// { ScrMode = smConfig; g_bFirstRun = true; }
#if SAVER
  res=RegQueryValueEx(skey,"SoundEnabled",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) SoundEnabled=val; else dumpmsg("error reading reg: SoundEnabled");// { ScrMode = smConfig; g_bFirstRun = true; }
#endif
#if PLUGIN
  SoundEnabled = true;
#endif
  //res=RegQueryValueEx(skey,"MinimizeAllWindowsOnExec",0,&valtype,(LPBYTE)&val,&valsize);   
  //if (res==ERROR_SUCCESS) AUTOMIN=val; else { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"FrameDelay",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) iRegistryDelay=val; else dumpmsg("error reading reg: FrameDelay");// { ScrMode = smConfig; g_bFirstRun = true; }

  // ------------ read VIDEO_CARD_FUCKED, FXW, FXH, and iDispBits --------------

  res=RegQueryValueEx(skey,"Z_16bit555",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) VIDEO_CARD_FUCKED=val; else dumpmsg("error reading reg: Z_16bit555");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"Z_FXW",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) FXW=val; else dumpmsg("error reading reg: Z_FXW");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"Z_FXH",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) FXH=val; else dumpmsg("error reading reg: Z_FXH");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"Z_BitDepth",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) iDispBits=val; else dumpmsg("error reading reg: Z_BitDepth");// { ScrMode = smConfig; g_bFirstRun = true; }
  //res=RegQueryValueEx(skey,"Z_bModeX",0,&valtype,(LPBYTE)&val,&valsize);   
  //if (res==ERROR_SUCCESS) bModeX=val; else dumpmsg("error reading reg: Z_bModeX");// { ScrMode = smConfig; g_bFirstRun = true; }

  res=RegQueryValueEx(skey,"solar_pal_freq",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) solar_pal_freq=val; else dumpmsg("error reading reg: solar_pal_freq");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"coarse_pal_freq",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) coarse_pal_freq=val; else dumpmsg("error reading reg: coarse_pal_freq");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"gamma",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) gamma=val; else dumpmsg("error reading reg: gamma");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"frames_til_auto_switch",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) frames_til_auto_switch__registry=val; else dumpmsg("error reading reg: frames_til_auto_switch");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"bDebugMode",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bDebugMode=val; else dumpmsg("error reading reg: bDebugMode");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"bSuppressHelpMsg",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bSuppressHelpMsg=val; else dumpmsg("error reading reg: bSuppressHelpMsg");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"volpos",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) volpos=val; else dumpmsg("error reading reg: volpos");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"bSuppressAllMsg",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bSuppressAllMsg=val; else dumpmsg("error reading reg: bSuppressAllMsg");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"ShiftingFreq",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_SlideShiftFreq=val; else dumpmsg("error reading reg: ShiftingFreq");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"ShiftingFrames",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_SlideShiftMinFrames=val; else dumpmsg("error reading reg: ShiftingFrames");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"PopupDissolveFrames",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_song_tooltip_frames=val; else dumpmsg("error reading reg: PopupDissolveFrames");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"UseBeatDetection",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bUseBeatDetection=val; else dumpmsg("error reading reg: UseBeatDetection");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"SyncColorToSound",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bSyncColorToSound=val; else dumpmsg("error reading reg: SyncColorToSound");// { ScrMode = smConfig; g_bFirstRun = true; }
  

#if PLUGIN
  res=RegQueryValueEx(skey,"g_bDisableSongTitlePopups",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bDisableSongTitlePopups=val; else dumpmsg("error reading reg: g_bDisableSongTitlePopups");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"g_bMinimizeWinamp",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bMinimizeWinamp=val; else dumpmsg("error reading reg: g_bDisableSongTitlePopups");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"g_random_songtitle_freq",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_random_songtitle_freq=val; else dumpmsg("error reading reg: g_bDisableSongTitlePopups");// { ScrMode = smConfig; g_bFirstRun = true; }
#endif
#if SAVER
  res=RegQueryValueEx(skey,"g_bDisableCdControls",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bDisableCdControls=val; else dumpmsg("error reading reg: g_bDisableCdControls");// { ScrMode = smConfig; g_bFirstRun = true; }
  res=RegQueryValueEx(skey,"g_bAutostartCD",0,&valtype,(LPBYTE)&val,&valsize);   
  if (res==ERROR_SUCCESS) g_bAutostartCD=val; else dumpmsg("error reading reg: g_bDisableSongTitlePopups");// { ScrMode = smConfig; g_bFirstRun = true; }
#endif

  //res=RegQueryValueEx(skey,"bExitOnMouse",0,&valtype,(LPBYTE)&val,&valsize);   
  //if (res==ERROR_SUCCESS) bExitOnMouse=val; else { ScrMode = smConfig; g_bFirstRun = true; }

  static float default_modeprefs[NUM_MODES+1] = {
	  0,
	    3,3,3,3,5,5,5,5,5,  // 01-09
	  3,3,1,3,3,2,3,4,3,3,  // 10-19
	  3,3,3,3,3,3           // 20-25
  };

  // LOAD MODEPREFS[]---------------------------------------------
  for (i=1; i<=NUM_MODES; i++)
  {
	  sprintf(str, "mode%2dfreq", i);
	  res=RegQueryValueEx(skey,str,0,&valtype,(LPBYTE)&val,&valsize);   
	  if (res==ERROR_SUCCESS) modeprefs[i]=val; else 
	  { 
		  dumpmsg("error reading reg: "); 
		  dumpmsg(str); 
		  modeprefs[i] = default_modeprefs[i];
	  }
	  if (modeprefs[i]<0) modeprefs[i]=0;
	  if (modeprefs[i]>5) modeprefs[i]=5;
	  if (i > NUM_MODES) modeprefs[i]=0;
  }
  modeprefs_total = 0;
  for (i=1; i<=NUM_MODES; i++) modeprefs_total += modeprefs[i];
  //--------------------------------------------------------------

  if (frames_til_auto_switch__registry <          5) frames_til_auto_switch__registry = 400;
  if (frames_til_auto_switch__registry >  999999999) frames_til_auto_switch__registry = 400;
  frames_til_auto_switch = frames_til_auto_switch__registry;
  if (gamma < 0) gamma = 0;
  if (gamma > 100) gamma = 0;
  if (volpos < 0) volpos = 10;
  if (volpos > 20) volpos = 10;

  // set initial volscale (float) from volpos (int)
  i = 10; 
  while (i < volpos) 
  {
     i++;
     volscale *= 1.25;
  }
  while (i > volpos) 
  {
     i--;
     volscale *= 0.8;
  }

  ///AMDTEST
#if AMDTEST
  FXW=640;
  FXH=480;
  iDispBits=8;
  iVSizePercent = 100;
#endif
  ///AMDTEST

  // DAMMIT!  This was screening out the 320x200's because of the 200 part
  // in v3.21.  Big bug.  Damn!  Fixed now.
  if (ScrMode==smSaver && (FXW<320 || (FXW%4) != 0 || FXH<200 || (FXH%4) != 0 || (iDispBits%8) != 0))
  dumpmsg("invalid reg params: width<320, height<200, or iDispBits%8 or FXW%4 or FXH%4 nonzero.");// { ScrMode = smConfig; g_bFirstRun = true; }

  OnFxwFxhUpdated();
  
  //if (VIDEO_CARD_FUCKED != 1) VIDEO_CARD_FUCKED = 0;

  RegCloseKey(skey);


}  

void WriteConfigRegistry()
{ 
  LONG res; 
  HKEY skey; 
  DWORD val, disp;
  
  
  res=RegCreateKeyEx(HKEY_CURRENT_USER,APPREGPATH,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
  if (res!=ERROR_SUCCESS) return;

  RegSetValueEx(skey,"DDrawDeviceGUID",0,REG_BINARY,(LPBYTE)&g_DDrawDeviceGUID,sizeof(GUID));

  //val=VidMode; 
  //RegSetValueEx(skey,"VideoMode3",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=iVSizePercent; // screen size
  RegSetValueEx(skey,"Screen_Vsize_Percent",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  //val=VIDEO_CARD_FUCKED;
  //RegSetValueEx(skey,"16bit_Video_Fix",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  //dither//val=bDither;
  //dither//RegSetValueEx(skey,"Dither",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  //dither//val=bRandomDither;
  //dither//RegSetValueEx(skey,"RandomDither",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=iCurSoundDriver;
  RegSetValueEx(skey,"iCurSoundDriver",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=SoundEnabled;
  RegSetValueEx(skey,"SoundEnabled",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=AUTOMIN;
  RegSetValueEx(skey,"MinimizeAllWindowsOnExec",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=iRegistryDelay;
  RegSetValueEx(skey,"FrameDelay",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=CURRENT_GEISS_VERSION;
  RegSetValueEx(skey,"Version",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));

  val=VIDEO_CARD_FUCKED;
  RegSetValueEx(skey,"Z_16bit555",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=FXW;
  RegSetValueEx(skey,"Z_FXW",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=FXH;
  RegSetValueEx(skey,"Z_FXH",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=iDispBits;
  RegSetValueEx(skey,"Z_BitDepth",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  //val=bModeX;
  //RegSetValueEx(skey,"Z_bModeX",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));

  
  val=solar_pal_freq;
  RegSetValueEx(skey,"solar_pal_freq",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=coarse_pal_freq;
  RegSetValueEx(skey,"coarse_pal_freq",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=gamma;
  RegSetValueEx(skey,"gamma",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=frames_til_auto_switch__registry;
  RegSetValueEx(skey,"frames_til_auto_switch",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bDebugMode;
  RegSetValueEx(skey,"bDebugMode",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bSuppressHelpMsg;
  RegSetValueEx(skey,"bSuppressHelpMsg",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=volpos;
  RegSetValueEx(skey,"volpos",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bSuppressAllMsg;
  RegSetValueEx(skey,"bSuppressAllMsg",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_SlideShiftFreq;
  RegSetValueEx(skey,"ShiftingFreq",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_SlideShiftMinFrames;
  RegSetValueEx(skey,"ShiftingFrames",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_song_tooltip_frames;
  RegSetValueEx(skey,"PopupDissolveFrames",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bUseBeatDetection;
  RegSetValueEx(skey,"UseBeatDetection",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bSyncColorToSound;
  RegSetValueEx(skey,"SyncColorToSound",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));


#if SAVER
  val=g_bDisableCdControls;
  RegSetValueEx(skey,"g_bDisableCdControls",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bAutostartCD;
  RegSetValueEx(skey,"g_bAutostartCD",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
#endif
#if PLUGIN
  val=g_bDisableSongTitlePopups;
  RegSetValueEx(skey,"g_bDisableSongTitlePopups",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_bMinimizeWinamp;
  RegSetValueEx(skey,"g_bMinimizeWinamp",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  val=g_random_songtitle_freq;
  RegSetValueEx(skey,"g_random_songtitle_freq",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
#endif

  //val=bExitOnMouse;
  //RegSetValueEx(skey,"bExitOnMouse",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));

  RegCloseKey(skey);
}





void WriteModePrefs()
{ 
  LONG res; 
  HKEY skey; 
  DWORD val, disp;
  char str[64];
  
  res=RegCreateKeyEx(HKEY_CURRENT_USER,APPREGPATH,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
  if (res!=ERROR_SUCCESS) return;

  for (int i=1; i<=NUM_MODES; i++)
  {
	  sprintf(str, "mode%2dfreq", i);
	  val = modeprefs[i];
      RegSetValueEx(skey,str,0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  }

  RegCloseKey(skey);
}


 /**************************************************************************** 
 * 
 *      GetDXVersion 
 * 
 *  This function returns two arguments: 
 *dwDXVersion: 
 *    0    No DirectX installed 
 *    0x100   DirectX version 1 installed 
 *    0x200   DirectX 2 installed 
 *    0x300   DirectX 3 installed 
 *    0x500   At least DirectX 5 installed. 
 *    0x501   At least DirectX 5a installed. 
 *  
 * Please note that this code is intended as a general guideline. Your app will 
 * probably be able to simply query for functionality (via COM's QueryInterface) 
 * for one or two components. 
 * Please also note: 
 *      "if (dxVer != 0x500) return FALSE;" is BAD.  
 *      "if (dxVer < 0x500) return FALSE;" is MUCH BETTER. 
 * to ensure your app will run on future releases of DirectX. 
 * 
 ****************************************************************************/ 

/*

      WARNING

      This code gave one user a 'page fault' crash in Geiss v3.09.  

*/
 
#include <dinput.h>
typedef HRESULT (WINAPI *DIRECTDRAWCREATE)(GUID *, LPDIRECTDRAW *, IUnknown *); 
typedef HRESULT (WINAPI *DIRECTINPUTCREATE)(HINSTANCE, DWORD, LPDIRECTINPUT *, IUnknown *); 
 
void GetDXVersion(LPDWORD pdwDXVersion) 
{ 
    HRESULT    hr; 
    HINSTANCE    DDHinst = 0; 
    HINSTANCE    DIHinst = 0; 
    LPDIRECTDRAW    pDDraw = 0; 
    LPDIRECTDRAW2    pDDraw2 = 0; 
    DIRECTDRAWCREATE    DirectDrawCreate = 0; 
    DIRECTINPUTCREATE    DirectInputCreate = 0; 
    OSVERSIONINFO    osVer; 
    LPDIRECTDRAWSURFACE    pSurf = 0; 
    LPDIRECTDRAWSURFACE3    pSurf3 = 0; 
 
  
    /* 
     * First see if DDRAW.DLL even exists. 
     */ 
    DDHinst = LoadLibrary("DDRAW.DLL"); 
    if (DDHinst == 0)  
    { 
        *pdwDXVersion = 0; 
        FreeLibrary(DDHinst); 
        return; 
    } 
 
    /* 
     *  See if we can create the DirectDraw object. 
     */ 
    DirectDrawCreate = (DIRECTDRAWCREATE) 
                            GetProcAddress(DDHinst, "DirectDrawCreate"); 
    if (DirectDrawCreate == 0)  
    { 
        *pdwDXVersion = 0; 
        FreeLibrary(DDHinst); 
                OutputDebugString("Couldn't LoadLibrary DDraw\r\n"); 
        return; 
    } 
 
    hr = DirectDrawCreate(NULL, &pDDraw, NULL); 
    if (FAILED(hr))  
    { 
        *pdwDXVersion = 0; 
        FreeLibrary(DDHinst); 
                OutputDebugString("Couldn't create DDraw\r\n"); 
        return; 
    } 
 
    /* 
     *  So DirectDraw exists.  We are at least DX1. 
     */ 
    *pdwDXVersion = 0x100; 
 
    /* 
     *  Let's see if IID_IDirectDraw2 exists. 
     */ 
    hr = pDDraw->QueryInterface(IID_IDirectDraw2, (LPVOID *)&pDDraw2); 
    if (FAILED(hr))  
    { 
        /* 
         * No IDirectDraw2 exists... must be DX1 
         */ 
        pDDraw->Release(); 
        FreeLibrary(DDHinst); 
                OutputDebugString("Couldn't QI DDraw2\r\n"); 
        return; 
    } 
    /* 
     * IDirectDraw2 exists. We must be at least DX2 
     */ 
    pDDraw2->Release(); 
    *pdwDXVersion = 0x200; 
 
    /* 
     *  See if we can create the DirectInput object. 
     */ 
    DIHinst = LoadLibrary("DINPUT.DLL"); 
    if (DIHinst == 0)  
    { 
        /* 
         * No DInput... must be DX2 
         */ 
        OutputDebugString("Couldn't LoadLibrary DInput\r\n"); 
        pDDraw->Release(); 
        FreeLibrary(DDHinst); 
        return; 
    } 
 
    DirectInputCreate = (DIRECTINPUTCREATE) 
                            GetProcAddress(DIHinst, "DirectInputCreateA"); 
    FreeLibrary(DIHinst); 
 
    if (DirectInputCreate == 0)  
    { 
        /* 
         * No DInput... must be DX2 
         */ 
        FreeLibrary(DDHinst); 
        pDDraw->Release(); 
                OutputDebugString("Couldn't GetProcAddress DInputCreate\r\n"); 
        return; 
    } 
 
    /* 
     * DirectInputCreate exists. That's enough to tell us that we are at least DX3 
     */ 
    *pdwDXVersion = 0x300; 
 
    /* 
     * Checks for 3a vs 3b? 
     */ 
 
    /* 
     * We can tell if DX5 is present by checking for the existence of IDirectDrawSurface3. 
     * First we need a surface to QI off of. 
     */ 
/*
    DDSURFACEDESC desc; 
 
    ZeroMemory(&desc, sizeof(desc)); 
    desc.dwSize = sizeof(desc); 
    desc.dwFlags = DDSD_CAPS; 
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE; 
 
    hr = pDDraw->SetCooperativeLevel(NULL,DDSCL_NORMAL); 
    if (FAILED(hr))  
    { 
        /* 
         * Failure. This means DDraw isn't properly installed. 
         */ 
/*      pDDraw->Release(); 
        FreeLibrary(DDHinst); 
        *pdwDXVersion = 0; 
                OutputDebugString("Couldn't Set coop level\r\n"); 
        return; 
    } 
 
    hr = pDDraw->CreateSurface(&desc, &pSurf, NULL); 
    if (FAILED(hr))  
    { 
        /* 
         * Failure. This means DDraw isn't properly installed. 
         */ 
/*      pDDraw->Release(); 
        FreeLibrary(DDHinst); 
        *pdwDXVersion = 0; 
                OutputDebugString("Couldn't CreateSurface\r\n"); 
        return; 
    } 
 
    /* 
     * Try for the IDirectDrawSurface3 interface. If it works, we're on DX5 at least 
     */ 
/*  if ( FAILED(pSurf->QueryInterface(IID_IDirectDrawSurface3,(LPVOID*)&pSurf3)) ) 
    { 
        pDDraw->Release(); 
        FreeLibrary(DDHinst); 
 
        return; 
    } 
 
    /* 
     * QI for IDirectDrawSurface3 succeeded. We must be at least DX5 
     */ 
/*
    *pdwDXVersion = 0x500; 
 
    pSurf->Release(); 
*/
    pDDraw->Release(); 
    FreeLibrary(DDHinst); 
 
    return; 
} 
 



#endif