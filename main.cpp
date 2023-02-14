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

//
//  GEISS 4.25 SCREENSAVER/PLUGIN - main.cpp
//

/*
BUILD BOTH with VS 2003 .NET, *release* builds.
PLUGIN: 
  -make sure you don't change code gen settings; you must use BLEND processor and 'Use Global Optimizations'==NO.  
    -otherwise, whenever any of the effect[]s are on, you'll see a flashing white dot in the upper-left pixel!
    -(in VC++ 6.0, you get this always, no matter what your project settings/optimizations are.  It's a compiler bug.)
SAVER: 
  -same notes about code gen settings.
UPDATING VERSION NUMBER:
  -update CURRENT_GEISS_VERSION below
  -update dialog title(s)
  -update documentation/etc.
*/


// VS6 project settings:
// ---------------------
//  libs:  comctl32.lib ddraw.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib winmm.lib LIBCMT.LIB 
//  settings:  C/C++ -> code generation -> calling convention is __stdcall.
//    plugin:   
//    saver:   dsound.lib  winmm.lib  LIBCMT.LIB 

#define CURRENT_GEISS_VERSION 429

#ifndef PLUGIN
 #ifndef SAVER
  #error one of those must be defined by the project!!!
 #endif
#endif

#define GRFX 1

/*
hard-to-test list
-----------------
  check all video modes, mmx and non-mmx
  check all color depths, and go up to 1024x768 full-screen
  test in machine w/o sound card - okay
  TEST IN MACHINE W/O CD-ROM DRIVE - okay
  test in machine w/o directx - make sure the message pops up.


to update version number
------------------------
  1. change titles of config & about dialogs (in resources)
  2. change the #define of CURRENT_GEISS_VERSION
  3. change it on geiss.html (the download link)



v4.00 
-----
1. added passwords to screensaver
2. added level trigger to screensaver
3. fixed bug in level trigger for plugin
4. added new maps
5. added presets - use '[##' to load and ']##' to save
6. added a new waveform (oscilloscope X-Y mode)
7. added true Beat Detection... now, when a "beat" is detected in the music, the wave
     brightness is more sharply linked to the beat volume, so the beats really come 
     out.  When no beat is detected, you see flat color, to emphasize the waveform.
8. map now changes on beat (when beat detected)
9. further thinned out the DirectX code (QueryInterface).. could have fixed NT 
     DirectX problems.  ?
10. NOTE: plugin now requires Winamp 2.10 for extended map keys (SHIFT-1 through 
     SHIFT-0) to function.
11. added dymanic map modification (linked to beats), and checkbox for default setting
     of this feature, in config panel.
12. added info about FULLSCREEN/EXCLUSIVE failure for plugin: "don't use DirectSound 
     output from Winamp!  Use waveout!"
13. added palette locking ('T')
14. added option to suppress ALL text messages (for nightclub-type displays)
15. added customizable messages... place them in GEISS.INI like so: (range 0-99)
        [CUSTOM MESSAGES]
        MESSAGE 1=Hello Dad - I'm in Jail!
        MESSAGE 99= ---  Welcome to Club Vinyl  ---
16. hopefully fixed a bug where certain (bright) greyscale palettes would cause
      the song title popups to become a solid white block when they faded out.
17. made an 'advanced options' panel to thin out the config panel... added a few 
      sliders too.
18. added dynamic map shifting - VERY, VERY COOL.  Use the 'I' key to
      turn it on and off.  It only takes effect when a good beat is
      detected.  You can set thresholds for how sensitive it is from
      the config panel.
19. spectrum analyzer: scaled bass by 0.2x, treble by 5x (linear interp)
20. fps is finally fixed
21. added rating system for the various maps
22. toned down the 'shifting' amount
23. removed call to getDXVersion(), because NT4/SP4 users were still getting the
      'ordinal 6 not found' message at times.  Hopefully, 99% of users will have
      at least DirectX 3 or later, which should be all that is required now.
24. made the new 'beat detection' for wave brightness toggle-able from the main
      config panel, since some people liked it more, and others preferred the old
      mode.
25. config panel items now 'tab' in order.
26. fixed bug w/presets for mode 14 (on load, it always had a really huge rotation).
27. fixed bug w/presets where palette would load wrong 20% of the time

4.01
----
1. fixed bug w/config panel's "use shifting effect" slider
2. fixed bug w/saver where it sometimes failed to autostart
      (WM_SYSKEYUP came in errantly on frame 0... now ignored)
3. plugin & saver now use GEISS.INI in your WINDOWS directory.
      (plugin previously used your winamp/plugins directory, which
       was sometimes impossible to detect)
4. removed old usage of 't' key from help screen (no longer shows song title)
5. set default gamma correction to 10%.
6. screensaver no longer exits on mouse movement
7. some effects can now be controlled by moving the mouse 

4.20
----
1. shifting effect improvements... marked by 'new_tech'
2. hicolor waves now linked to freq... marked by 'fourier' and
        'g_power_smoothed'
3. IMPORTANT: restore functionality of SPACE key: 
    case ' ':
        FX_Loadimage("fx.tga");
        --> g_hit/song_title stuff
4. aligned VS1, VS2, DATA_FX, DATA_FX2 to 8-byte boundaries.
5. MMX 32-bit proc_map() is now MEGA-FAST!!!!!!! (and one code
     base serves all screen widths!)
6. MMX 32-bit proc_map() now also supports DITHER!!!!!!!!!!!
7. 8-bit Intel proc_map() loops are now aligned to 8 bytes.  
     (Cyrix loops already were).
8. 32-bit wave color is now 60% freq, 40% random.
9. 32-bit wave colors are now 35% brighter
10. changed the way x/y pos of messages/songtitles are positioned.
       (more freedom now)
11. fixed modeprefs bug (screen 16 wasn't ever loading)

12+ BIG CHANGE: Changed map packet size from 6 to 8.  No speed hit...
    now massively strange maps are possible, however!
13. added new modes (17-25)
14. added new palette curve (#6) - darkish - allows for fire/sun palette
15. improved beat detection (for shifting)
16. shifting freq. default is now 33% of the time


4.21
----
1. removed bCyrix, CyrixInstead()
2. added map damping (0.5=soft music, 1.0=no damping=fast music) 
      based on the 'spectral variance' BUT it is disabled for now...
      have to ask Dave Ulmer for permission to use it.  To enable it,
      make freq. analysis occur in 8-bit mode as well, (comment out
      'if (iDispBits > 8)') and make 'suggested_damping' not always equal 1.0f.
      To make this easy, just look for the two lines enclosed with 
      '----disable map damping----' and comment them out.
3. updated freq. ranges for truecolor sound-freq. correlation (too much red before)

4.22/4.23
---------
1. fixed bug that had disabled the FPS display; already released plugin v4.23.
2. removed initial_map_offset, and proc_map_asm() no longer multiplies the
     offsets by 4 in 32-bit mode (it's stored in DATA_FX[] as offset*4).
3. removed slider2 (slider1 is now +/-)

4.24
----
1. updated web link (geisswerks.com) and e-mail address.
2. changed registry key from HKEY_LOCAL_MACHINE to HKEY_CURRENT_USER
3. bug: maps 21+ weren't selectable; you can now access them directly.
    -press '##' to select a mode directly (ex: mode 5 is entered as '05').
    -press 'm##' to select a custom message 
    -press '[##' to load a preset
    -press ']##' to save a preset
    -you can press ESC at any time to cancel an in-progress command.
    -pressing ESC again will exit.

4.24b
-----
1. used DEFAULT_CHARSET instead of ANSI_CHARSET in CreateFont() call,
    so now, more foreign text (song titles) should display properly.
2. made some small changes to make exiting the plugin a bit safer
    (changed the way winamp is minimized/restored; waited to destroy
    plugin window until window class was unregistered; removed 
    a gratuitous 'setcursor' call at the very end.)
3. a while back I changed it so the wave colors went to the frequencies
    in the music, instead of oscillating on their own, and a lot of people
    have yelled at me over the years for not making this optional, so
    here ya go - now it's an option - see the lower-left corner of the
    config panel.  =)
4. marked the 'frame idle delay' more clearly in the advanced config 
    panel: now says "Frame idle delay: (*crank this up if it runs 
    too fast!!*)".
5. in system settings/code generation, told it to use a multithreaded 
    DLL run-time library (was set to single-threaded [non-DLL]).

4.24c
-----
1. plug-in: recreated the Visual C++ project file for the plug-in, and in the 
    process, changed code gen. from 'multithreaded DLL' to just 'multithreaded' 
    (this was the default suggested by VC++ for a DLL, though it does differ 
    from what was in 4.24: 'single-threaded [non-DLL]'.)  (plug-in was reported 
    to not work at all on many systems, saying it couldn't find 'msvcrtd.dll' - 
    the debug runtime library, which is a developers-only dll.)
2. screensaver: reverted code gen. from multithreaded DLL to single-threaded 
    non-DLL    (plug-in [sic] was reported to not work at all on many systems, 
    saying it couldn't find 'msvcrtd.dll' - the debug runtime library, which 
    is a developers-only dll.  Didn't hear any feedback on the screensaver,
    but I'm assuming it would have had the same problem.)
3. fixed annoying pause when the screensaver was run with /p (preview mode)
    by delay-loading dsound.lib and WINMM.LIB (the key).  The delay loading
    was set up by 1) adding DelayImp.lib to the project, and 2) specifying
    /DELAYLOAD:XYZ.LIB in Project Settings -> Link -> Customize -> Project Options
    for each lib file you want to delay-load.

4.25
----
1. there are no longer any restrictions on screen resolution.  (Before, we had to
    write a separate loop for each possible res, because we were out of registers
    and couldn't hold that info in a register.  Now, the code just modifies itself
    to poke in the right variable into an immediate value in the code itself.)
2. the default resolution is now whatever your desktop is at.
3. when the framerate exceeds 30 fps, we now slow down the motion (per frame)
    so that it appears to be the same rate of motion as if you were running at 30
    fps - just smoother.
4. the overall image darkening-over-time rate (the sum of the 4 weights for bilinear 
    interpolation) is now tuned based on the # of pixels.  At high resolutions,
    there is very little loss (weightsum == 256), while at 800x600, the weights
    total about 253, and at low resolutions (320x240), it goes down as low as 251.
5. 'frames_til_auto_switch' is now actually adjusted to the current framerate, so,
    set its value as if the plugin/screensaver were going to be running at 30 fps,
    and it will do the right thing.  (This way, you don't have to mess with that
    value to correct for whatever fps you're getting.)
6. updated the waveforms to draw properly at higher resolutions, by smoothly upsampling
    (to 2X or 4X the # of dots).  This also keeps them nice and mostly-solid.
7. made a bunch of hte other little special effects work nicely at higher resolutions
    (that usually means fatter dots), for both 8-bit and >8-bit color.
8. wow - fixed a bug in the screensaver version that proves I hardly knew what a 
    thread *was* back in 1998.  Oops.  Keypresses are actually SAFE now!
9. also cranked up default wave brightness/scaling for the screensaver.

4.26
----
-added 'S'huffle and 'R'epeat toggle keys to plugin version.
-*plugin* no longer flashes the help message at you when you move the mouse.
-made waveform colors richer in >8bpp modes
-'shifting' effect now runs correctly at >30 fps.  (it got a bit spastic before.)
-default 'frames_til_auto_switch' is a little longer now.
-fixed a bug in 8-bit modes where last ~256 pixels on last scanline weren't updating.
-when you set it to use 100% of the screen, it now uses a bit more (before it was actually like 96%;
      now it's about 98%.  My sincere apologies, but I just don't have time to revamp it all to use 100%...
      trying to do what I can in limited time...)
-we no longer call DrawText() on the front buffer.  (Embarrassing... I know... I was
      *very* new to programming and graphics back in '98!)  This was causing problems on
      some machines whenever you tried to draw text.
-increased the chromatic dispersion on the 'bar' effect ('u' key), and slowed it down a bit,
      for 32-bit modes.
-worked on the chases a bit - slowed them down in high-fps situations, and they new draw more dots
    when the res. is high (proportional to the resolution), so the overall feel is the same.
-default mode preferences (ratings) now biased so that modes 5-9 show up more often (4 stars)
      than the others.  Mode 15 shows up less often (2 stars).  (Defaults only)
-in mode 6, you now get swirlies more often.
-worked around a compiler bug that was sometimes putting a flashing white dot in the upper-left corner.
    (had to move from VC++6 to VS2003 .NET; had to make sure processor was set to 'Blend' and Global Optimizations
    were off.)
-disabled all message processing that might interfere with power functions (monitor turning off,
    sleep, etc).
-some of the more colorful effects now show up a little more often, in hi-color modes; and the grid
    effect now shows up ~1% more often, in all modes.
-fixed aspect ratio of 'grid' effect
-changed text color to a more pleasant yellow
-mouse cursor is now hidden on every frame (...before, if you hit a winamp play/pause/etc key, the cursor
    would show, and you'd have to wiggle the mouse to make it go away).
-moved the 'A' dots a little further away from the center of the screen when at larger resolutions.
-if you try to run the plugin without music, it now gives you a messagebox telling you why it's not going
    to work (instead of just leaving you with a black screen, wondering why nothing is happening).


4.27
----
-added support for multiple monitors!
  -select which monitor from the config panel
  -geiss will run on that monitor, and you can work freely in the other(s)
-removed the 'minimize winamp' option from plugin (not necessary)
-added instructions to the config panel on how to run with live input, as well as
  a button to run 'sndvol32 /r' for you.
-default video mode is now 32-bit (was 8-bit), and the 32-bit modes now say "[recommended]" next to them,
  instead of the 8-bit ones.
-we now clear the edges on one of the main buffers [front or back - it will alternate] 
  every 11 frames, so if you're working on another monitor and something accidentally paints
  over part of the visualizer window, it'll be gone within a second or less.
-fixed bug where waveform #1, with really loud music, could sometimes draw pixels just beyond the
  bottom line of the screen, which would cause them to bleed into the image [at least, in modes such as mode 6].
-duration (and frequency, if randomization is on) of song title popups 
  is now sync'd to the framerate.
-fixed bug where pausing the music would sometimes cause song title popup 
  to appear with " - Winamp [Paused]" at the end of the song title.
-tuned some spatial and temporal settings on the Grid effect so that it looks nicer at
  fast framerates and/or higher resolutions.  (Looks the same at old/slow/low framerates/resolutions.)
-when you start the plugin, or when you start the config panel, or (when on the config panel)
  select a new monitor while it is up, it now auto-finds the closest match to the previously-selected
  resolution and color depth, if the exact match can not be found.  (Rather than just giving you
  an error and quitting, in the case of running the plugin; or selecting some lazy default,
  for the config panel.)



4.30
----------
  TODO:
    -UNIFNISHED: see NEW_COLORS_430 - great!!!
    -decrease rate of map changes?  it's great to sit for minutes at a time...
      -also, crank down # effects in high color modes - better with just waves...

4.29
----------
  -Video mode list is now sorted.
  -Default video mode selection now matches whatever your primary monitor is set to.
  -On advanced settings panel, changed 'frames between mode switches' to 
    'seconds between mode switches', and made the new range from 1..60 seconds.
    Also fixed the saving of this value (in 4.28, there was a bug, and
    it wouldn't save the value if you changed it).



4.28
----------
-In VS2003 project settings, switched from multi-threaded DLL 
  to multi-threaded (oops)... was causing an unnecessary MSVCR71.DLL 
  dependency, which some systems do not have.



DDraw documentation: http://msdn.microsoft.com/en-us/library/aa917130.aspx
NEXT REV:
		// silly:	screensaver, in	multimon config, only	affects	the	SELECTED monitor...
    // if they try to run @ a res that is no longer supported, it will give them an error
    //   ( -> and probably not the best error message)
    // ALT+TAB is janky; if you click it once quickly, it doesn't really exit.
    //   If you hold ALT and keep tabbing, it minimizes the plugin.
    //   ALT+TABbing back will restore it.
		// (NOTE:	only the monitors	that are part	of windows desktop show	up in	the	list.)
*    -play around with "COFFEE FILTER"
    -latest msg from work:
        -text can start/stop just fine, EXCEPT for the help *screen*, which, when it goes away, turns the screen black.  Going to a new map still fixes it. Note that other single-line text messages can come up and go away without a problem; so there is something special about the help screen (10 lines).  Are we not releasing the DC or something?
        -VERY INTERESTING: the help-msg-going-away bug only happens in 8-bit color!  In 32-bit modes, it comes and goes without a problem.  So, is it just a driver bug, or maybe a palette thing?



  








to do:
    -why does plugin make desktop icons flash on exit?  (win2k)

    -make beat det. focus on low-range frequencies (bass!), not the composite volume.
    -beat det. still sucks (especially w/high fps)
    -random 'custom message' popups
    -*specify* which monitor to run on
    -MTV-style song info + time @ bottom-left corner?
    -option to turn off freq-color sync. in hicolor modes
    -change the font/size on custom messages/song titles
    -remap keys; [ and ] cause trouble on German keyboards.

    -"can't set 0x0x0 video mode" bug after 1st run...
        -LOTS of reports, just search for 0x0x0
        -12/24/99: Roland McIntosh
        -12/31/99: Tha Shaydiest
    -lot of people still complaining that it stops after each song.
    



future work:
  -make it so you can have a preset generate in the background & pop up when you hit
     some special key (---> concerts!)
  
  0. optimize 32-bit mmx loop (reorder instructions...?)
  1. restore dither to 32-bit mmx loop
  2. createDIBitmap from the VS, then use DC to blit to video.
     this will enable DMA!  However, then have to worry about
     brightening (x2) the 32-bit images...  =(
  3. make it run in a window (easy once (1) is complete)
  4. make it run in desktop background! (GetDesktopWindow())



  -presets are in;
     1. need a 'random' mode
     2. save mode #6 swirl positions, types, etc.
  -need a slider-bar for freq. of random custom message popups




  Final big tasks
  ---------------
  I.   add multimon support.

   





controling the colors by pressing keys(R,G,B,+/-)
no hanging screen by the start of a new track
no hanging screen if a number or n is pressed
no unlocking when number or n id pressed
no change of wave/effects/color when a number is pressed
additional keyboard-LED spectrum
nice looking text(position selection, antialiasing, maybe blurring)
self defined keys
start presets to songs(+keys to save/load presets)
saving favorite combinations (10 or so)

-NT4 still hates directsound


-sound bug: sometimes Sound is initially disabled on saver (on certain systems).
    -some guy is going to send debug file - should say why sound is initially off.
    -(w won't work because 'o' needs pressed first)
    ->made special version has items marked with 'tempsound' for extra debug info.  
        Sent to Piotr cxvsdfas... waiting for debug file from him.

  
*/









//int weightsum = 253;







    
    

// items commented out:
// #define STRICT


//#define STRICT
#define WIN32_LEAN_AND_MEAN
//#include <afx.h>  // for CString

//#if PLUGIN
    //#include <afxwin.h> // for drawing text to virtual screen (CDC objects)
//#endif

#include <windows.h>
#include <regstr.h>
#include <stdlib.h>
#pragma hdrstop
#include "resource.h"
#include <stdio.h>        // for sprintf() for fps display
#if SAVER
#include <mmsystem.h>   // for mci/cd stuff
#endif

//#if SAVER
 //#define _MT             // for multithreading
 //#include <process.h>    // for multithreading
//#endif

//for Geiss:
#if SAVER
    #define NAME "Geiss screensaver"
    #define TITLE "Geiss screensaver"
#else
    #define NAME "Geiss for Winamp"
    //#define NAME2 "Geiss Subwindow"
    #define TITLE "Geiss for Winamp"
#endif

#include <windowsx.h>
#include <ddraw.h>
//#include <stdarg.h>  //RECENT
#include <time.h>
#include <math.h>


#if SAVER
    #include <mmreg.h>
    #include <dsound.h>
    //#include "outsound.h"
#endif
#include <commctrl.h>
#include <shellapi.h>

#include "helper.h" //for GetNumCores

int g_nCores = 1;

int g_desktop_w   = 800;
int g_desktop_h   = 600;
int g_desktop_bpp = 8;
int g_desktop_hz  = 60;

float AdjustRateToFPS(float per_frame_decay_rate_at_fps1, float fps1, float actual_fps)
{
    // returns the equivalent per-frame decay rate at actual_fps
    
    // basically, do all your testing at fps1 and get a good decay rate;
    // then, in the real application, adjust that rate by the actual fps each time you use it.
    
    float per_second_decay_rate_at_fps1 = powf(per_frame_decay_rate_at_fps1, fps1);
    float per_frame_decay_rate_at_fps2 = powf(per_second_decay_rate_at_fps1, 1.0f/actual_fps);

    return per_frame_decay_rate_at_fps2;
}

void GetDesktopDisplayMode()
{
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
    {
        g_desktop_w = dm.dmPelsWidth;
        g_desktop_h = dm.dmPelsHeight;
        g_desktop_bpp = dm.dmBitsPerPel;//(dm.dmBitsPerPel==16) ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
        g_desktop_hz  = dm.dmDisplayFrequency;
    }
}

#if PLUGIN

    /*
    #define NUM_BLOBS 8
    float blob_x[NUM_BLOBS];
    float blob_y[NUM_BLOBS];
    float blob_str[NUM_BLOBS];
    float blob_str2[NUM_BLOBS];
    int   matrix[40][30];
    */

    enum visModeEnum { spectrum, wave };
    visModeEnum visMode;

    //RECT g_WinampWindowRect;

    #define APPREGPATH "SOFTWARE\\geissplugin"
    #include "vis.h"

    struct winampVisModule *g_this_mod = NULL;
    HWND this_mod_hwndParent = NULL;
    HINSTANCE this_mod_hDllInstance = NULL;

    void ReadConfigRegistry();
    void WriteConfigRegistry();
    BOOL CALLBACK ConfigDialogProc(HWND  hwndDlg, UINT  uMsg, WPARAM  wParam, LPARAM  lParam);
    BOOL CALLBACK Config2DialogProc(HWND  hwndDlg, UINT  uMsg, WPARAM  wParam, LPARAM  lParam);

    // plug-in functions
    void config(struct winampVisModule *this_mod); // configuration dialog
    int  init(struct winampVisModule *this_mod);       // initialization for module
#endif
    int  render1(struct winampVisModule *this_mod);  // rendering for module 1
#ifdef PLUGIN
    void quit(struct winampVisModule *this_mod);   // deinitialization for module

    // configuration declarations
    int param_1=0, param_2=50;    // param_1: CRUIZIN (0/1).  param_2: free
    void config_read(struct winampVisModule *this_mod);        // reads the configuration
    void config_write(struct winampVisModule *this_mod);    // writes the configuration
    //void config_getinifn(struct winampVisModule *this_mod, char *ini_file); // makes the .ini file filename

    // returns a winampVisModule when requested. Used in hdr, below
    winampVisModule *getModule(int which);

    // our window procedure
    //LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND hMainWnd; // main window handle
    //HWND hSecondWnd;  // handle to keypress-processing window

    // Double buffering data
    //HDC memDC;        // memory device context
    //HBITMAP    memBM,  // memory bitmap (for memDC)
    //        oldBM;  // old bitmap (from memDC)


    // Module header, includes version, description, and address of the module retriever function
    winampVisHeader hdr = { VIS_HDRVER, NAME, getModule };

    int PreReadDelayFromRegistry();

    // first module (oscilliscope)
    winampVisModule mod1 =
    {
        NAME,
        NULL,    // hwndParent
        NULL,    // hDllInstance
        0,        // sRate
        0,        // nCh
        0,// 18,        // latencyMS... the higher this number ,the earlier the graphics react.  
                        // 70 seems to match perfectly.
                        // 0 will make the graphics a little sluggish.
        PreReadDelayFromRegistry(), //0,//18,        // delayMS
        2,        // spectrumNch
        2,        // waveformNch
        { 0, },    // spectrumData
        { 0, },    // waveformData
        config,
        init,
        render1, 
        quit
    };

    // this is the only exported symbol. returns our main header.
    // if you are compiling C++, the extern "C" { is necessary, so we just #ifdef it
    //#ifdef __cplusplus
    extern "C" {
    //#endif
    __declspec( dllexport ) winampVisHeader *winampVisGetHeader()
    {
        //MessageBox(NULL, "TEXT", "CAPTION", MB_OK);

        return &hdr;
    }
    //#ifdef __cplusplus
    }
    //#endif

#else       //---------------- SCREENSAVER-SPECIFIC---------------------



    #define APPREGPATH "SOFTWARE\\geissscreensaver"



#endif


    //#define TIMER_ID        1
    //#define TIMER_RATE      200             // # of ms; was 500
    //#define FX_YCUT         15               //skip this many vert. lines in calculation
    //#define FX_YCUT_HIDE    20               //skip this many vert. lines when SHOWING (=FX_YCUT+7)

/*-------------------------------------------------*/

bool CheckMMXTechnology();
//bool CyrixInstead();



#if SAVER
    DSCBUFFERDESC              dscbd;
    LPDIRECTSOUNDCAPTURE       pDSC;
    LPDIRECTSOUNDCAPTUREBUFFER pDSCB;
    WAVEFORMATEX               wfx =
        { WAVE_FORMAT_PCM, 2, 44100, 176400, 4, 16, 0 };

    int  g_DDRAW_ERRORS = 0;
    bool g_DDRAW_FRAGGED_req = false;
    bool g_DDRAW_FRAGGED_ack = false;
    HWND hSaverMainWindow;
        // wFormatTag, nChannels, nSamplesPerSec, mAvgBytesPerSec = nSamplesPerSec*nBlockAlign,
        // nBlockAlign = nChannels*wBitsPerSample/8, wBitsPerSample, cbSize

#endif

    //DDSURFACEDESC g_ddsd;


    char g_song_title[256];                // also used to display custom messages
    //#define g_song_tooltip_frames 20
    int g_song_tooltip_frames = 20;
    HFONT g_title_font = NULL;
    int g_song_title_y;
    int g_song_title_x;
    int g_title_R;// = 190 + rand() % 50;
    int g_title_G;// = 190 + rand() % 50;
    int g_title_B;// = 190 + rand() % 50;
    bool g_LastMessageWasCustom = false;

#if PLUGIN
    BOOL g_bDisableSongTitlePopups = false;  
    BOOL g_WinampMinimizedByHand = false;
    BOOL g_bMinimizeWinamp = false;//true;
    HCURSOR hOldCursor = NULL;
    WINDOWPLACEMENT wp_winamp;
    int g_random_songtitle_freq = 0;
#endif

    int last_frame_v = 0;   // for sound level-triggering
    int last_frame_slope = 0;

#if SAVER
    BOOL g_bAutostartCD = false;
    BOOL g_bDisableCdControls = false;
#endif


///int wave_brightness = 0;

#define NUM_FREQS 3
#define FREQ_SAMPLES 192
__int16 g_SoundBuffer[16384];

float g_phase_inc[NUM_FREQS];

float g_fSoundBuffer[16384];
LPGUID g_lpGuid;
BOOL SoundEnabled=TRUE;
BOOL SoundReady=FALSE;
BOOL SoundActive=FALSE;
BOOL SoundEmpty=FALSE;
int  oldSoundBufPos=0;
int  oldSoundBufNum=0;
//BOOL fOnWin95;
char szSoundDrivers[10][1024];
int iNumSoundDrivers = 0;
int iCurSoundDriver = 0;
int frames_since_silence = 0;
float avg_peaks = 100;
float current_vol = 0;
#define PAST_VOL_N 120
float past_vol[PAST_VOL_N];
int   past_vol_pos = 0;
float avg_vol = 1.0;
float avg_vol_wide = 1.0;
float avg_vol_narrow = 1.0;
unsigned long iVolumeSum = 1;
float debug_param = 1.0;
float damped_std_dev = 5;
bool bBeatMode = false;
bool bBigBeat = false;
float fBigBeatThreshold = 1.10;

float rand_array[2345];
float sqrt_tab[21][21];
int rand_array_pos = 0;

int iTrack = 0;      // current CD track
int iNumTracks = 0;  // # cd tracks
enum DISC_STATES { STOPPED, PLAYING, PAUSED, UNKNOWN, BUSY };
DISC_STATES eDiscState = UNKNOWN;
//BUSY means that Windows CD player is running & we can't control the cd.
//when BUSY is set, g_bDisableCdControls will also get set (for that execution)

// user volume control
int volpos = 10;                // from 0 to 20... 10 is middle.
float volscale = 0.20;//1.0;    // designed for Windows' line-in to be at 1/2 position.
int slider1 = 0;
int g_FramesSinceShift = 0;
bool g_bSlideShift = true;
int g_SlideShiftFreq = 33;  // 33% of the time
int g_SlideShiftMinFrames = 5; // min. no. of frames between shifts

// new shifting technique...
float g_ShiftMaxVol = 0;



#define FOURIER_DETAIL 24
float g_power[FOURIER_DETAIL];
float g_power_smoothed[FOURIER_DETAIL];




int g_hit = 0;
int AUTOMIN = 0;
//BOOL bScrollMap = false;
//BOOL new_bScrollMap = false;

float gF[6];
#define SCATTERVALS 226
float fScatter[SCATTERVALS];
int g_iScatterPos = 0;




    // globals for bkg map generation:
    float f1, f2, f3, f4;
    float old_f1, old_f2, old_f3, old_f4;
    //unsigned char p1, p2, p3, p4;
    float cx[10], cy[10], ci[10], cj[10], cr[10], cr_inv[10];
    int   ctype[10];
    int   R_offset, old_R_offset;
    int  y_map_pos = -1;
    float max_rad_inv;// = 1.0/sqrtf(FXW*FXW+FXH*FXH);
    float rmult;// = 640.0/(float)FXW;
    float rdiv;// = 1.0/rmult;
    float protective_factor;// = (FXW > 640) ? 640.0/(float)FXW : 1;

//----------------------------------------------------

#define num_vid_modes 11 //12

int  VidMode = -1;
BOOL VidModeAutoPicked = false;
int  iNumVidModes = 0;  // set by DDraw's enumeration
int  iDispBits=32;
//BOOL bModeX=false;
#if SAVER
    long FXW = -1;         // to set default pick, go to EnumModesCallback()
    long FXH = -1;         // to set default pick, go to EnumModesCallback()
    int  iVSizePercent = 100;//45;
#endif
#if PLUGIN
    long FXW = -1;//320;   // to set default pick, go to EnumModesCallback()
    long FXH = -1;//240;   // to set default pick, go to EnumModesCallback()
    int  iVSizePercent = 100;//35;//100;
#endif


typedef struct
{
    int lo_band;
    int hi_band;
    bool bFXPalette;
    int iFXPaletteNum;
    int c1, c2, c3;
} pal_td;
pal_td old_palette;


typedef struct
{
    int iMode;
    int iParam1;
    int iParam2;
    int iParam3;
    float fParam1;
    float fParam2;
    float fParam3;
    //int palType;      // 0 
} preset;
preset g_Preset[10];


typedef struct
{
    int  iDispBits;
    int  FXW;
    int  FXH;
    BOOL VIDEO_CARD_FUCKED;
    //BOOL bModeX;
    char name[64];
} VidModeInfo;
#define MAX_VID_MODES 512
VidModeInfo VidList[MAX_VID_MODES];

typedef struct 
{
  GUID guid;
  char szDesc[256];
  char szName[256];
} DDrawDeviceInfo;
#define MAX_DEVICES 16
DDrawDeviceInfo g_device[MAX_DEVICES];
int g_nDevices = 0;
GUID g_DDrawDeviceGUID;


enum modesel_state { NONE_SEL, MODE_SEL_1 };
enum preset_state  { NONE, LOAD_1, LOAD_2, SAVE_1, SAVE_2 };
modesel_state eModeSelState = { NONE_SEL };
preset_state  ePresetState = { NONE };
preset_state  eCustomMsgState = { NONE };
int iModeSelection = 0;
int iPresetNum     = 0;
int iCustomMsgNum  = 0;

//int  ThreadNr = 0;
BOOL g_rush_map = false;
BOOL g_QuitASAP = false;
BOOL g_GeissProcFinished = false;
BOOL g_bFirstRun = false;
BOOL g_bDumpFileCleared = false;
BOOL g_bDebugMode = false;
BOOL g_bSuppressHelpMsg = false;
BOOL g_bSuppressAllMsg = false;
BOOL g_DisclaimerAgreed = true;
BOOL g_ConfigAccepted = false;
BOOL g_Capturing = false;
BOOL g_bUseBeatDetection = true;
BOOL g_bSyncColorToSound = false;

bool g_bLoadPreset = false;
int  g_iPresetNum = -1;

/*-------------------------------------------------*/

//int                     iBands = 4;  // 0=always...5=never
int                     iRegistryDelay = 0;  // milliseconds per frame
bool                    bMMX;
//bool                    bCyrix;
bool                    bBypassAssembly = FALSE;
bool                    bExitOnMouse = FALSE;
bool                    bLocked = FALSE;
bool                    bPalLocked = FALSE;
//dither//BOOL                    bDither = TRUE;
//dither//BOOL                    bRandomDither = FALSE;
//dither//BOOL                    bMEGARandomDither = FALSE;
int waveform, new_waveform;
int last_mouse_x = -1;
int last_mouse_y = -1;

#define NUM_WAVES 6

//void RestoreCtrlAlt(void);
void __cdecl RestoreCtrlAlt(void);

void GetWaveData();
void RenderDots(unsigned char *VS1);
void RenderWave(unsigned char *VS1);

#define WAVE_5_BLEND_RANGE   50
const int MINBUFSIZE         = ((314+WAVE_5_BLEND_RANGE)*2 + 20);

long     FX_YCUT             =90;               //skip this many vert. lines in calculation
long     FX_YCUT_HIDE         =92; //= FX_YCUT + 2;
long     FX_YCUT_NUM_LINES   =FXW*(FXH-FX_YCUT*2);
long     FX_YCUT_xFXW_x8     =FX_YCUT*FXW*8;
long     FX_YCUT_xFXW        =FX_YCUT*FXW;
long     FX_YCUT_HIDE_xFXW     =FX_YCUT_HIDE*FXW;
long     FXW_x_FXH           =FXW*FXH;
long     BUFSIZE             =max(FXW*2, MINBUFSIZE);
    //int FX_YCUT_HIDE_xLINESIZE = FX_YCUT_HIDE*1280;


    int   modeprefs[128];    // values range from 0 to 5 stars; default is 3.
    int   modeprefs_total = -1;  // ---REMEMBER, RANGE is [1,NUM_MODES], INCLUSIVE!---

    float fps = 40.0f;
    float fps_at_last_mode_switch = 40.0f;
    //float fps_core = 30;
    float time_array[30];
    int   time_array_pos = 0;
    bool  time_array_ready = false;
    clock_t start_clock = 0;      //DEPRECATED
    clock_t clock_debt = 0;       //DEPRECATED
    clock_t core_clock_time = 0;  //DEPRECATED
    clock_t blit_clock_time = 0;  //DEPRECATED
    clock_t flip_clock_time = 0;  //DEPRECATED
    //clock_t temp_clock;//, start_clock = -1;
    char inifile[512];
    char szDEBUG[512];
    char szFPS[] = "abcdefghijkabcdefghijkabcdefghijkabcdefghijkabcdefghijk";
#if SAVER
    char szMCM[] = " [click mouse button to exit - press h for help] ";
#endif
#if PLUGIN
    char szMCM[] = " [press ESC to exit - press h for help] ";
#endif
    char szLM[] = " - screen is LOCKED -       ";
    char szULM[] = " - screen is unlocked -       ";
    char szMisc[512];
    char szTrack[] = " - stopped at track xxx/yyy -                                    ";
    
    char szH1[] = " q/e/u/g/d/a/y: control effects ";
#if SAVER
    //dither//char szH2[] = " i/o: toggle dither/toggle sound ";
    char szH2[] = " o: toggle sound ";
#endif
#if PLUGIN
    char szH2[] = " SPACE: show song title ";
#endif
    char szH3[] = " j/k: temporarily scale wave down/up ";
    char szH4[] = " L/T: (un)lock screen/palette,   < and >: rate screen ";
    char szH5[] = " n: new screen (random),   i: shifting on/off";
    char szH6[] = " w/p: change waveform/palette ";
    char szH7[] = " ##: pick map (01-25)";
    char szH8[] = " h/f: toggle help/fps displays ";
#if SAVER
    char szH9[] = " z x c v b:  << play pause stop >> (for CD) ";
#endif
#if PLUGIN
    char szH9[] = " z x c v b r s:  << play pause stop >> repeat shuffle (Winamp) ";
#endif
    //char szH10[]= " ESC/click: quit,        r: refresh screen edges ";
    char szH10[]= " ESC/click: quit ";
    char szCurrentCD[128];
    char szNewCD[128];

    int chaser_x[20], chaser_y[20], chaser_ptr;
    unsigned char chaser_r[20], chaser_g[20], chaser_b[20];

#if (GRFX==1)
    LPDIRECTDRAW            lpDD = NULL;           // DirectDraw object
    //LPDIRECTDRAW2           lpDD2 = NULL;          // DirectDraw object
    LPDIRECTDRAWSURFACE     lpDDSPrimary = NULL;   // DirectDraw primary surface
    LPDIRECTDRAWSURFACE     lpDDSBack = NULL;      // DirectDraw back surface
#endif
  bool g_bLost = false;
    LPDIRECTDRAWPALETTE        lpDDPal = NULL;
    PALETTEENTRY            ape[256];
    PALETTEENTRY            ape2[256];
    PALETTEENTRY            apetemp[256];
    int                     iBlendsLeftInPal;  // out of 64
    int                        iFramesPerPaletteTick = 7;

    //BOOL                    bActive;        // is application active?
    BOOL                    bUserQuit = FALSE;
    bool                    g_PassDialogReq = false;
    bool                    g_PassDialogAck = false;
    bool                    g_PowerSaveAck = false; // means the monitor shut off.
    bool                    g_PowerSaveReq = false;

    //void FX_Random_Palette();
    void FX_Random_Palette(bool bLoadPal = false);
    void PutPalette();
    float CrankPal(unsigned int curve_id, int z);



//#define DllImport   __declspec( dllimport )
//DllImport void Apply_DLL_Mode();






//typedef void (CALLBACK* LPFNDLLFUNC1)();
//typedef void (CALLBACK* LPFNDLLFUNC1)();

//HINSTANCE hMapLoaded;         // Handle to DLL

//typedef int (*MAPFUNCTION)(void);

//MAPFUNCTION mapfunction;

//LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer (to Apply_DLL_Mode())

//__declspec(dllimport) void DoMapFromDLL();



//extern "C" void __declspec(dllexport) WINAPI DoMapFromDLL();  






/*
#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllimport ) void Apply_DLL_Mode();
#ifdef __cplusplus
}
#endif
*/

    unsigned char *original_VS[2];
    unsigned char *original_DATA_FX[2];

    unsigned char *VS1 = NULL;
    unsigned char *VS2 = NULL;
    unsigned char *TEMPPTR = NULL;
    unsigned char *DATA_FX = NULL;  //    each pixel has 6 bytes of info saved for it:
                                    //    d1,d2,d3,d4, and the offset to look at.
    unsigned char *DATA_FX2 = NULL;
    char winpath[512];
    
    float scale1, scale2, turn1, turn2;//, x_trans=0, y_trans=0;
    float old_scale1, old_scale2, old_turn1, old_turn2;
    float cos_turn1, cos_turn2, sin_turn1, sin_turn2;
    float center_dwindle, floatframe=0;
    long  frames_this_mode=0, intframe=0, clearframes=4, chaser_offset;
    long  last_mousemove_frame=-5;
    int   frames_til_auto_switch = 550; //...at 30 fps; but gets to be equiv. at the current fps.
    int   frames_til_auto_switch__registry = 550;
    int   gamma = 10;
    int   solar_pal_freq = 1;
    int   coarse_pal_freq = 1;
    int   frames_crunching_this_mode=0;
    long  frames_since_last_plop=10;
    int       mode,     gXC,     gYC, solar_max, grid_dir;  // grid_dir is 1 or -1
    int   new_mode, new_gXC, new_gYC;
    float damping = 1.0f, new_damping = 1.0f;
    float suggested_damping = 1.0f;
    int   weightsum, weightsum_res_adjusted, old_weightsum;
    unsigned char _REMAP_VALUES[2048];
    unsigned char *REMAP, *REMAP2, *REMAP3;
    unsigned char VIDEO_CARD_FUCKED=0, RND=1;
    //unsigned char SHOW_TITLE=-1;
    unsigned char SHOW_DEBUG          = 0;
    unsigned char SHOW_MOUSECLICK_MSG = 0;
    unsigned char SHOW_LOCKED_MSG     = 0;
    unsigned char SHOW_UNLOCKED_MSG   = 0;
    unsigned char SHOW_TRACK_MSG      = 0;
    unsigned char SHOW_MODEPREFS_MSG  = 0;
    unsigned char SHOW_MISC_MSG       = 0;
    bool          SHOW_FPS            = false;
    bool          SHOW_HELP_MSG       = false;



    BOOL FX_Init();
    void FX_Fini();
    void FX_Pick_Random_Mode();  // sets global 'mode' to a valid random number.
    void FX_Apply_Mode(bool bLoadPreset = false, int iPresetNum = 0);         // sets up DATA_X and other misc. stuff based on 'mode'
    void Solid_Line(float frame, unsigned char far *VS2);
    void Two_Chasers(float frame);
    void Put_Helpmsg_To_Backbuffer();
    void Put_FPS_To_Backbuffer();
    void Put_Trackmsg_To_Backbuffer();
    void Put_Msg_To_Backbuffer(char *str);
    //void Put_Debugmsg_To_Backbuffer();
    //void Put_Lockedmsg_To_Backbuffer();
    //void Put_Unlockedmsg_To_Backbuffer();
    //void Put_Mouseclickmsg_To_Backbuffer();
    void Merge_All_VS_To_Backbuffer();
    //void Process_Map(void *p1, void *p2, LPDIRECTDRAWSURFACE lpDDSurf);
    //void Frame_Seq();
    void RenderFX();
    void Drop_Solar_Particles(int);
    void Drop_Solar_Particles_320(int);
    void CheckForNewCD();

    BOOL CALLBACK ConfigDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    BOOL CALLBACK Config2DialogProc(HWND  hwndDlg, UINT  uMsg, WPARAM  wParam, LPARAM  lParam);
    BOOL CALLBACK DisclaimerDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    BOOL CALLBACK AboutDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

    BOOL doInit( HINSTANCE hInstance, int nCmdShow );
    void finiObjects( void );
    long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    //long FAR PASCAL WindowProc2( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

    
   extern HINSTANCE hMainInstance; /* screen saver instance handle  */ 

enum TScrMode {smNone,smConfig,smPassword,smPreview,smSaver};
TScrMode ScrMode=smNone;


LARGE_INTEGER m_high_perf_timer_freq;  // 0 if high-precision timer not available
LARGE_INTEGER m_prev_end_of_frame;
//int  g_nTargetFps = 9999;
//bool g_bTargetFpsStrict = false;
#define HALFSPEED_MOTION_CUTOFF_FRAMERATE ( 46.0f )

#define NUM_EFFECTS 9
#define NUM_MODES 25//16//19       // warning!  modes start at 1, not zero!

//int char_to_mode[256];
int effect[NUM_EFFECTS];
enum { CHASERS, BAR, DOTS, SOLAR, GRID, NUCLIDE, SHADE, SPECTRAL };  //'spectral' was the freq. spectrum dots; deprecated.

float micro_f1[10];
float micro_f2[10];
float micro_f3[10];
float micro_f4[10];
float micro_c1[10];
float micro_c2[10];
float micro_c3[10];
float micro_rad[4][10];

void FX_Screenshot(int fr);
void FX_Loadimage(char *s);
void GetWindowsPath(); // sets winpath
void ReadConfigRegistry();
void WriteConfigRegistry();
bool Try_To_Recover();
void dumpmsg(char *s);


#include "effects.h"
#include "proc_map.h"
#include "video.h"
#include "sysstuff.h"


//fontobj g_font;

bool mode_motion_dampened[] = 
{ 
    true, true, true, false, true, true, false, true, true, true,  // 0 is bunk; 1-9 here (3,6 not dampened)
    true, true, true, true, true, true, true,  // modes 10-16 are all dampened
    false, false, false,                       // 17 - 19 are normal
    false, false, false, false, false,         // 20 - 24 are normal
    false, false, false, false, false,         // 25 - 29 are normal
    false, false, false, false, false,         // 30 - 34 are normal
    false, false, false, false, false,         // 35 - 39 are normal
};

bool rotation_dither[] =
{ 
    false, true, false, false, false, false, false, false, false, true,   // 0-9 (0 is unused)
    false, true,  false, false, false,         // 10 - 14
    false, false, false, false, false,         // 15 - 19
    false, false, false, false, false,         // 20 - 24
    false, false, false, false, false,         // 25 - 29
    false, false, false, false, false,         // 30 - 34
    false, false, false, false, false,         // 35 - 39
};

bool custom_motion_vectors[] = 
{
    // 6, 10, 12 are true
    false, false, false, false, false, false, true, false, false, false,   // 0-9 (0 is unused)
    true,  false, true,  false, false,         // 10 - 14
    false, false, false, false, false,         // 15 - 19
    false, false, false, false, false,         // 20 - 24
    false, false, false, false, false,         // 25 - 29
    false, false, false, false, false,         // 30 - 34
    false, false, false, false, false,         // 35 - 39
};


class CModeInfo
{
public:

    CModeInfo();

    // all out of 1000.
    int effect_freq[NUM_EFFECTS];
    int solar_max;
    float center_dwindle;
    int max_effects;
    int min_effects;

    void Clip_Num_Effects();

protected:
    static int Get_Num_Effects();
};


CModeInfo::CModeInfo()
{
    min_effects = 1;
    max_effects = 2;
    solar_max = 60;
    center_dwindle = 0.99;
    for (int i=0; i<NUM_EFFECTS; i++) effect_freq[i] = 1000/NUM_EFFECTS;
}

int CModeInfo::Get_Num_Effects()
{
    int n = 0;
    for (int i=0; i<NUM_EFFECTS; i++)
        if (effect[i] > 0) 
            n++;
    
    return n;
}

void CModeInfo::Clip_Num_Effects()
{
    int n = Get_Num_Effects();
    int j;
    BOOL bGotOne;

    if (!SoundActive || SoundEmpty)
    while (n < min_effects)
    {
        bGotOne = FALSE;
        for (j=0; j<NUM_EFFECTS; j++)
            if ((effect[j]==-1) && (rand()%1000 < effect_freq[j]) && (bGotOne==FALSE))
            {
                effect[j] = 1;
                bGotOne = TRUE;
                n++;
            }
    }

    // make SURE the 1000-freq. ones get put in.
    for (j=0; j<NUM_EFFECTS; j++)
        if (effect_freq[j]>=1000)
            effect[j] = 1;

    while (n > max_effects)
    {
        j = rand() % NUM_EFFECTS;
        if (effect[j]==1 && effect_freq[j]<1000)
        {
            effect[j] = -1;
            n--;
        }
    }
}

CModeInfo modeInfo[NUM_MODES+1];       // array starts at 1.


BOOL DlgItemIsChecked(HWND hDlg, int nIDDlgItem)
{ return ((SendDlgItemMessage(hDlg, nIDDlgItem, BM_GETCHECK, (WPARAM) 0, (LPARAM) 0) == BST_CHECKED) ? TRUE : FALSE); }


//BOOL CALLBACK DialogProc(HWND  hwndDlg, UINT  uMsg, WPARAM  wParam, LPARAM  lParam);
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, DWORD wParam, LONG lParam); 

//BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, DWORD wParam, LONG lParam);


/*----------------------------------------------------------------------------------*/

#define DEBUG FALSE

#define REGSTR_PATH_PLUSSCR (REGSTR_PATH_SETUP "\\Screen Savers")
#define REGSTR_PATH_CONFIG  ("Software\\Lu\\Minimal Saver")
HINSTANCE hInstance=NULL;
HWND hScrWindow=NULL;
class TSaverSettings; 
TSaverSettings *ss=NULL;

#if DEBUG
void Debug(char *c) {OutputDebugString(c); OutputDebugString("\n");}
#else
void Debug(char *) {}
#endif


// CreateTrackbar - creates and initializes a trackbar. 
// 
// Global variable 
//     g_hinst - instance handle 
/*
HWND WINAPI CreateTrackbar( 
    HWND hwndDlg,  // handle of dialog box (parent window) 
    UINT iMin,     // minimum value in trackbar range 
    UINT iMax,     // maximum value in trackbar range 
    UINT iSelMin,  // minimum value in trackbar selection 
    UINT iSelMax)  // maximum value in trackbar selection 
{ 
 
    InitCommonControls(); // loads common control’s DLL 
 
    hwndTrack = CreateWindowEx( 
        0,                             // no extended styles 
        TRACKBAR_CLASS,                // class name 
        "Trackbar Control",            // title (caption) 
        WS_CHILD | WS_VISIBLE | 
        TBS_AUTOTICKS | TBS_ENABLESELRANGE,  // style 
        10, 10,                        // position 
        200, 30,                       // size 
        hwndDlg,                       // parent window 
        ID_TRACKBAR,             // control identifier 
        g_hinst,                       // instance 
        NULL                           // no WM_CREATE parameter 
        ); 
 
    SendMessage(hwndTrack, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(iMin, iMax));  // min. & max. positions 
    SendMessage(hwndTrack, TBM_SETPAGESIZE, 
        0, (LPARAM) 4);                  // new page size 
 
    SendMessage(hwndTrack, TBM_SETSEL, 
        (WPARAM) FALSE,                  // redraw flag 
        (LPARAM) MAKELONG(iSelMin, iSelMax); 
    SendMessage(hwndTrack, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) iSelMin); 
 
    SetFocus(hwndTrack); 
 
    return hwndTrack; 
} 
*/

// CheckHots: this routine checks for Hot Corner services.
// It first tries with SAGE.DLL, which comes with Windows Plus!
// Failint this it tries with ScrHots, a third-party hot-corner
// service program written by the author that is freely
// distributable and works with NT and '95.
// It's not actually used in the saver, but code is included here
// just to be useful.
/*
BOOL CheckHots()
{ typedef BOOL (WINAPI *SYSTEMAGENTDETECT)();
  HINSTANCE sagedll=LoadLibrary("Sage.dll");
  if (sagedll!=NULL)
  { SYSTEMAGENTDETECT detectproc=(SYSTEMAGENTDETECT)
        GetProcAddress(sagedll,"System_Agent_Detect");
    BOOL res=FALSE;
    if (detectproc!=NULL) res=detectproc();
    FreeLibrary(sagedll);
    if (res) return TRUE;
  }
  HINSTANCE hotsdll=LoadLibrary("ScrHots.dll");
  if (hotsdll!=NULL)
  { SYSTEMAGENTDETECT detectproc=(SYSTEMAGENTDETECT)
        GetProcAddress(hotsdll,"System_Agent_Detect");
    BOOL res=FALSE;
    if (detectproc!=NULL) res=detectproc();
    FreeLibrary(hotsdll);
    if (res) return TRUE;
  }
  return FALSE;
}
*/

// NotifyHots: if you make any changes to the hot corner
// information in the registry, you must call NotifyHots
// to infxorm the hot corner services of your change.
// It's not actually used in the saver, but code is included here
// just to be useful.
/*
void __fastcall NotifyHots()
{ typedef VOID (WINAPI *SCREENSAVERCHANGED)();
  HINSTANCE sagedll=LoadLibrary("Sage.DLL");
  if (sagedll!=NULL)
  { SCREENSAVERCHANGED changedproc=(SCREENSAVERCHANGED)
        GetProcAddress(sagedll,"Screen_Saver_Changed");
    if (changedproc!=NULL) changedproc();
    FreeLibrary(sagedll);
  }
  HINSTANCE hotsdll=LoadLibrary("ScrHots.dll");
  if (hotsdll!=NULL)
  { SCREENSAVERCHANGED changedproc=(SCREENSAVERCHANGED)
        GetProcAddress(hotsdll,"Screen_Saver_Changed");
    if (changedproc!=NULL) changedproc();
    FreeLibrary(hotsdll);
  }
}
*/

BOOL VerifyPassword(HWND hwnd)
{ // Under NT, we return TRUE immediately. This lets the saver quit, and the system manages passwords.
  // Under '95, we call VerifyScreenSavePwd. This checks the appropriate registry key and, if necessary, pops up a verify dialog

  OSVERSIONINFO osv; 
  osv.dwOSVersionInfoSize=sizeof(osv); 
  GetVersionEx(&osv);
  if (osv.dwPlatformId==VER_PLATFORM_WIN32_NT) 
      return TRUE;  

  HINSTANCE hpwdcpl = ::LoadLibrary("PASSWORD.CPL");
  if (hpwdcpl==NULL) 
  {
      Debug("Unable to load PASSWORD.CPL. Aborting");
      return TRUE;
  }


  // Pause for password dialog; give it time to take effect.
  g_PassDialogAck = false;
  g_PassDialogReq = true;
  //Sleep(200);
  while (!g_PassDialogAck)
  {
      Sleep(10);   //wait until Geiss pauses it's stuff...
  }
  dumpmsg("VerifyPassword: geissproc stopped... bringing up dialog...");

  typedef BOOL (WINAPI *VERIFYSCREENSAVEPWD)(HWND hwnd);
  VERIFYSCREENSAVEPWD VerifyScreenSavePwd;
  VerifyScreenSavePwd = (VERIFYSCREENSAVEPWD)GetProcAddress(hpwdcpl,"VerifyScreenSavePwd");
  if (VerifyScreenSavePwd==NULL) 
  {
      dumpmsg("VerifyPassword: Unable to get VerifyPwProc address. Aborting");
      FreeLibrary(hpwdcpl);
      g_PassDialogReq = false;
      g_PassDialogAck = false;
      return TRUE;
  }
  BOOL bres=VerifyScreenSavePwd(hwnd); 
  FreeLibrary(hpwdcpl);
  dumpmsg("VerifyPassword: dialog finished...");
  g_PassDialogReq = false;
  g_PassDialogAck = false;
  return bres;
}


void ChangePassword(HWND hwnd)
{ // This only ever gets called under '95, when started with the /a option.
  HINSTANCE hmpr=::LoadLibrary("MPR.DLL");
  if (hmpr==NULL) {Debug("MPR.DLL not found: cannot change password.");return;}
  typedef VOID (WINAPI *PWDCHANGEPASSWORD) (LPCSTR lpcRegkeyname,HWND hwnd,UINT uiReserved1,UINT uiReserved2);
  PWDCHANGEPASSWORD PwdChangePassword=(PWDCHANGEPASSWORD)::GetProcAddress(hmpr,"PwdChangePasswordA");
  if (PwdChangePassword==NULL) {FreeLibrary(hmpr); Debug("PwdChangeProc not found: cannot change password");return;}
  PwdChangePassword("SCRSAVE",hwnd,0,0); FreeLibrary(hmpr);
}



/*
class TSaverSettings
{ public:
  HWND hwnd;
  DWORD PasswordDelay;   // in seconds
  DWORD MouseThreshold;  // in pixels
  BOOL  MuteSound;
  POINT InitCursorPos;
  DWORD InitTime;        // in ms
  UINT  idTimer;         // a timer id, because this particular saver uses a timer
  BOOL  IsDialogActive;
  BOOL  ReallyClose;     // for NT, so we know if a WM_CLOSE came from us or it.
  BOOL  FlashScreen;     // this is a user-configuration option, particular to this example saver
  TSaverSettings();
  void ReadGeneralRegistry();
  void ReadConfigRegistry();
  void WriteConfigRegistry();
  void CloseSaverWindow();
  void StartDialog();
  void EndDialog();
};
TSaverSettings::TSaverSettings() {hwnd=NULL; ReallyClose=FALSE; idTimer=0;}
void TSaverSettings::ReadGeneralRegistry()
{ PasswordDelay=15; MouseThreshold=50; IsDialogActive=FALSE; // default values in case they're not in registry
  LONG res; HKEY skey; DWORD valtype, valsize, val;
  res=RegOpenKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_PLUSSCR,0,KEY_ALL_ACCESS,&skey);
  if (res!=ERROR_SUCCESS) return;
  valsize=sizeof(val); res=RegQueryValueEx(skey,"Password Delay",0,&valtype,(LPBYTE)&val,&valsize); if (res==ERROR_SUCCESS) PasswordDelay=val;
  valsize=sizeof(val); res=RegQueryValueEx(skey,"Mouse Threshold",0,&valtype,(LPBYTE)&val,&valsize);if (res==ERROR_SUCCESS) MouseThreshold=val;
  valsize=sizeof(val); res=RegQueryValueEx(skey,"Mute Sound",0,&valtype,(LPBYTE)&val,&valsize);     if (res==ERROR_SUCCESS) MuteSound=val;
  RegCloseKey(skey);
}
void TSaverSettings::ReadConfigRegistry()
{ FlashScreen=TRUE;
  LONG res; HKEY skey; DWORD valtype, valsize, val;
  res=RegOpenKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_CONFIG,0,KEY_ALL_ACCESS,&skey);
  if (res!=ERROR_SUCCESS) return;
  //rmg:
  valsize=sizeof(val); res=RegQueryValueEx(skey,"Flash Screen",0,&valtype,(LPBYTE)&val,&valsize);   if (res==ERROR_SUCCESS) FlashScreen=val;
  RegCloseKey(skey);
}  
void TSaverSettings::WriteConfigRegistry()
{ LONG res; HKEY skey; DWORD val, disp;
  res=RegCreateKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_CONFIG,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
  if (res!=ERROR_SUCCESS) return;
  //rmg:
  val=FlashScreen; RegSetValueEx(skey,"Flash Screen",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
  RegCloseKey(skey);
}
void TSaverSettings::CloseSaverWindow() {ReallyClose=TRUE; PostMessage(hwnd,WM_CLOSE,0,0);}
void TSaverSettings::StartDialog() {IsDialogActive=TRUE;SendMessage(hwnd,WM_SETCURSOR,0,0);}
void TSaverSettings::EndDialog()   {IsDialogActive=FALSE;SendMessage(hwnd,WM_SETCURSOR,0,0);GetCursorPos(&InitCursorPos);}
  

/**
LRESULT CALLBACK SaverWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{ switch (msg)
  { case WM_CREATE:
    { Debug("WM_CREATE..."); ss->hwnd=hwnd;
      GetCursorPos(&(ss->InitCursorPos)); ss->InitTime=GetTickCount();
      ss->idTimer=SetTimer(hwnd,0,100,NULL);
    } break;
    /*
    case WM_TIMER:
    { if (ss->FlashScreen)
      { HDC hdc=GetDC(hwnd); RECT rc; GetClientRect(hwnd, &rc); 
        FillRect(hdc,&rc,GetSysColorBrush((GetTickCount()>>8)%25));
        ReleaseDC(hwnd,hdc);
      }
    } break;
    */
/**    case WM_ACTIVATE: 
    case WM_ACTIVATEAPP: 
    case WM_NCACTIVATE:
    { 
        if (ScrMode==smSaver && !ss->IsDialogActive && LOWORD(wParam)==WA_INACTIVE && !DEBUG)
        {
            Debug("WM_ACTIVATE: about to inactive window, so sending close"); 
            ss->CloseSaverWindow();
        }
    } 
    break;
    case WM_SETCURSOR:
    { if (ScrMode==smSaver && !ss->IsDialogActive && !DEBUG) SetCursor(NULL);
      else SetCursor(LoadCursor(NULL,IDC_ARROW));
    } break;
    case WM_LBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDOWN: case WM_KEYDOWN:
    { if (ScrMode==smSaver && !ss->IsDialogActive) {Debug("WM_BUTTONDOWN: sending close");ss->CloseSaverWindow();}
    } break;
    case WM_MOUSEMOVE:
    { if (ScrMode==smSaver && !ss->IsDialogActive && !DEBUG)
      { POINT pt; GetCursorPos(&pt);
        int dx=pt.x-ss->InitCursorPos.x; if (dx<0) dx=-dx; int dy=pt.y-ss->InitCursorPos.y; if (dy<0) dy=-dy;
        if (dx>(int)ss->MouseThreshold || dy>(int)ss->MouseThreshold)
        { Debug("WM_MOUSEMOVE: gone beyond threshold, sending close"); ss->CloseSaverWindow();
        }
      }
    } break;
    case (WM_SYSCOMMAND):
    { if (ScrMode==smSaver)
      { if (wParam==SC_SCREENSAVE) {Debug("WM_SYSCOMMAND: gobbling up a SC_SCREENSAVE to stop a new saver from running.");return FALSE;}
        if (wParam==SC_CLOSE && !DEBUG) {Debug("WM_SYSCOMMAND: gobbling up a SC_CLOSE");return FALSE;}
      }
    } break;
    case (WM_CLOSE):
    { 
        if (ScrMode==smSaver && ss->ReallyClose && !ss->IsDialogActive)
        { 
            Debug("WM_CLOSE: maybe we need a password");
            BOOL CanClose=TRUE;
            if (GetTickCount()-ss->InitTime > 1000*ss->PasswordDelay)
            { 
                ss->StartDialog(); CanClose=VerifyPassword(hwnd); ss->EndDialog();
            }
            if (CanClose) 
            {
                Debug("WM_CLOSE: doing a DestroyWindow"); 
                DestroyWindow(hwnd);
            }
            else 
            {
                Debug("WM_CLOSE: but failed password, so doing nothing");
            }
      }
      if (ScrMode==smSaver) return FALSE; // so that DefWindowProc doesn't get called, because it would just DestroyWindow
    } break;
    case (WM_DESTROY):
    { if (ss->idTimer!=0) KillTimer(hwnd,ss->idTimer); ss->idTimer=0;
      Debug("POSTQUITMESSAGE from WM_DESTROY!!");
      PostQuitMessage(0);
    } break;
  }
  return DefWindowProc(hwnd,msg,wParam,lParam);
}
**/

#if SAVER
BOOL __stdcall DSEnumCallback(LPGUID lpGuid,              
                    LPCSTR lpcstrDescription,  
                    LPCSTR lpcstrModule,         
                    LPVOID lpContext          )
{
    int i;

    char str[1024];

    strcpy(str, lpcstrModule);
    int len = strlen(str);

    for (i=0; i<len; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z') str[i] -= ('a' - 'A');
    }

    //if (!SoundReady)
    //for (i=0; i<len; i++)
    //{
        //if (strstr((char *)(str+i), "WAVEIN") || strstr((char *)(str+i), "WAVE IN"))

    strcpy(szSoundDrivers[iNumSoundDrivers], lpcstrDescription);
    if (strlen(lpcstrModule) > 0)
    {
        strcat(szSoundDrivers[iNumSoundDrivers], " [");
        strcat(szSoundDrivers[iNumSoundDrivers], lpcstrModule);
        strcat(szSoundDrivers[iNumSoundDrivers], "]");
    }

    iNumSoundDrivers++;

    if (ScrMode == smSaver && iNumSoundDrivers-1 == iCurSoundDriver)
    {
        HRESULT hr;
          
        hr = ::DirectSoundCaptureCreate(lpGuid, &pDSC, NULL);
        if (hr == DS_OK)
        {
            dumpmsg("DirectSoundCaptureCreate() success.");

            dscbd.dwSize = sizeof(DSCBUFFERDESC);
            dscbd.dwFlags = 0;
            // We're going to capture 1/30 second's worth of audio
            dscbd.dwBufferBytes = wfx.nAvgBytesPerSec;
            dscbd.dwReserved = 0;
            dscbd.lpwfxFormat = &wfx;

            pDSCB = NULL;

            // pDSC is the pointer to the DirectSoundCapture object
            hr = pDSC->CreateCaptureBuffer(&dscbd, &pDSCB, NULL);
            switch(hr)
            {
            case DS_OK:
                SoundReady = TRUE;
                SoundActive = TRUE;
                break;
            case DSERR_INVALIDPARAM:
                MessageBox( NULL, "CreateCaptureBuffer: DSERR_INVALIDPARAM", "ERROR", MB_OK );
                SoundEnabled = FALSE;
                return FALSE;//exit(891);
            case DSERR_BADFORMAT :
                char buf[1024];
                sprintf(buf, "CreateCaptureBuffer: DSERR_BADFORMAT.  Most likely, your system is "
                             "playing an \"Open Program\" sound which is interfering with "
                             "Geiss's sound channels when it starts.  Go to the Control Panel, select "
                             "[Sounds], then change your [Open Program] sound to [None].\r\n\r\n"
                             
                             "In the rare case that this "
                             "doesn't fix it, try reinstalling DirectX or updating your drivers for "
                             "your sound card.  If none of these things work, your sound card probably "
                             "can't support 44100 Hz, 16-bit, stereo sound capture... \r\n\r\n"
                             "--> Geiss will now run, but without sound synchronization <--"
                        );
                MessageBox( NULL, buf, "ERROR - read carefully, you can probably fix it", MB_OK );
                //MessageBox( NULL, "CreateCaptureBuffer: DSERR_BADFORMAT", "ERROR", MB_OK );
                SoundEnabled = FALSE;
                return FALSE;//exit(892);
            case DSERR_GENERIC :
                MessageBox( NULL, "CreateCaptureBuffer: DSERR_GENERIC", "ERROR", MB_OK );
                SoundEnabled = FALSE;
                return FALSE;//exit(893);
            case DSERR_NODRIVER :
                MessageBox( NULL, "CreateCaptureBuffer: DSERR_NODRIVER", "ERROR", MB_OK );
                SoundEnabled = FALSE;
                return FALSE;//exit(894);
            case DSERR_OUTOFMEMORY :
                MessageBox( NULL, "CreateCaptureBuffer: DSERR_OUTOFMEMORY", "ERROR", MB_OK );
                SoundEnabled = FALSE;
                return FALSE;//exit(895);
            case DSERR_UNINITIALIZED :
                MessageBox( NULL, "CreateCaptureBuffer: DSERR_UNINITIALIZED", "ERROR", MB_OK );
                SoundEnabled = FALSE;
                return FALSE;//exit(896);
            }
        }
        else
        {
            //MessageBox( NULL, "DirectSoundCaptureCreate() failed", "ERROR", MB_OK );
            //dumpmsg("DirectSoundCaptureCreate() failed");
            //exit(896);
            dumpmsg("DirectSoundCaptureCreate() failed!!!");
            switch(hr)
            {
                case DSERR_INVALIDPARAM:  dumpmsg("(...DSERR_INVALIDPARAM)" ); break;
                case DSERR_NOAGGREGATION: dumpmsg("(...DSERR_NOAGGREGATION)"); break;
                case DSERR_OUTOFMEMORY:   dumpmsg("(...DSERR_OUTOFMEMORY)"  ); break;
            }
             iNumSoundDrivers = 0;
            SoundEnabled = FALSE;
            SoundReady = FALSE;
            SoundActive = FALSE;
        }
          return TRUE;
    }
    return TRUE;
}
#endif

BOOL WINAPI EnumDevicesCallback(
  GUID FAR* lpGUID, 
  LPSTR lpDriverDescription, 
  LPSTR lpDriverName, 
  LPVOID lpContext, 
  HMONITOR hm)
{
  if (g_nDevices < MAX_DEVICES)
  {
    memset(&g_device[g_nDevices].guid, 0, sizeof(GUID));
    if (lpGUID)
      g_device[g_nDevices].guid = *lpGUID;
    strcpy( g_device[g_nDevices].szDesc, lpDriverDescription);
    strcpy( g_device[g_nDevices].szName, lpDriverName);
    g_nDevices++;  
    return TRUE;
  }
  return FALSE;
}

HRESULT WINAPI EnumModesCallback(
  LPDDSURFACEDESC lpDDSurfaceDesc,  
  LPVOID lpContext  )
{
    char modemsg[256];
    sprintf(modemsg, "video mode: %dx%dx%d, linearsize=%d, Rmask=%x, Gmask=%x, Bmask=%x, RGBAmask=%x ", 
        lpDDSurfaceDesc->dwWidth, 
        lpDDSurfaceDesc->dwHeight, 
        lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, 
        lpDDSurfaceDesc->dwLinearSize,
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask,
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask,
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask,
        lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask
    );

    int w = lpDDSurfaceDesc->dwWidth;
    int h = lpDDSurfaceDesc->dwHeight;
    int bits = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;

    if (//(lpDDSurfaceDesc->dwFlags | DDSD_PITCH) &&
        //lpDDSurfaceDesc->dwLinearSize > 0 &&
        (bits==8 || bits==16 || bits==24 || bits==32) && 
        #if SAVER
            (1)//(w==320 || w==400 || w==480 || w==512 || w==640 || w==800 || w==1024)// || w==1280 || w==1600)
        #endif
        #if PLUGIN
            (1)//(w==320 || w==400 || w==480 || w==512 || w==640 || w==800)
        #endif
       )
    {

        // determine # of green bits for 16-bit color modes
        int gmask, gbits=0;
        gmask = lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask;
        while (gmask>0) { gbits += gmask & 1; gmask >>= 1; }
        int fct = ((bits==16) && (gbits==5)) ? 1 : 0;
    
       

      // if we're enumerating for config panel, if this video mode is the one they have picked, we need
      // to remember that so it shows up as selected in the combobox.
        int current_best_w = (VidMode >= 0) ? VidList[VidMode].FXW : 999999;
        int current_best_h = (VidMode >= 0) ? VidList[VidMode].FXH : 999999;
        int current_best_bpp = (VidMode >= 0) ? VidList[VidMode].iDispBits : 999999;
        int current_best_fct = (VidMode >= 0) ? (VidList[VidMode].VIDEO_CARD_FUCKED ? 1 : 0) : 999999;
        if (abs(w   -FXW      ) <= abs(current_best_w   - FXW      ) &&
            abs(h   -FXH      ) <= abs(current_best_h   - FXH      ) &&
            abs(bits-iDispBits) <= abs(current_best_bpp - iDispBits) &&
            abs(fct -VIDEO_CARD_FUCKED) <= abs(current_best_fct - VIDEO_CARD_FUCKED)
            //bits == iDispBits 
            //&& ((bits!=16) || ((gbits==5)==VIDEO_CARD_FUCKED))
            )
        {
            VidMode = iNumVidModes;
        }

       
        // collect mode info in VidList[]

        VidList[iNumVidModes].VIDEO_CARD_FUCKED = (gbits == 5);   // regular = 5/6/5, fucked = (1)/5/5/5
        VidList[iNumVidModes].FXW = w;
        VidList[iNumVidModes].FXH = h;
        VidList[iNumVidModes].iDispBits = bits;
        //VidList[iNumVidModes].bModeX = ((lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MODEX) != 0);
        sprintf(VidList[iNumVidModes].name, "%4d x %4d   %2d-bit", 
                lpDDSurfaceDesc->dwWidth, 
                lpDDSurfaceDesc->dwHeight,
                lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount
            );
        //if (VidList[iNumVidModes].bModeX) strcat(VidList[iNumVidModes].name, " [mode X]");
        //else 
        if (VidList[iNumVidModes].iDispBits==32) strcat(VidList[iNumVidModes].name, "       [recommended]");

        if (iNumVidModes < MAX_VID_MODES-1)
            iNumVidModes++;
        else
            dumpmsg("**********TOO MANY VIDEO MODES*************!!!!!!");

        strcat(modemsg, " - okay");
    }

    dumpmsg(modemsg);

    return DDENUMRET_OK;

}

/*
#if SAVER
void __cdecl GeissProc( void *p )
{    
    int iStep = 1;
    //MSG msg;

    //while (!g_QuitASAP)
    //{
        if (!g_rush_map && 
            !g_QuitASAP && 
            !g_PassDialogAck &&
            !g_PowerSaveAck &&
            !g_DDRAW_FRAGGED_ack
            )
        {
            //dumpmsg(">");

            if (iStep==1 && !g_rush_map && !g_QuitASAP)
            {
                /*
                HRESULT ddrval;

                dumpmsg("(re)setting the display mode...");
                ddrval = lpDD->SetDisplayMode( FXW, FXH, iDispBits );
                switch(ddrval)
                {
                    case DDERR_GENERIC:             dumpmsg("  [DDERR_GENERIC]");         break;
                    case DDERR_INVALIDMODE:         dumpmsg("  [DDERR_INVALIDMODE]");                    break;
                    case DDERR_INVALIDOBJECT:       dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
                    case DDERR_INVALIDPARAMS:       dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
                    case DDERR_LOCKEDSURFACES:      dumpmsg("  [DDERR_LOCKEDSURFACES]");                    break;
                    case DDERR_NOEXCLUSIVEMODE:     dumpmsg("  [DDERR_NOEXCLUSIVEMODE]"); break;
                    case DDERR_SURFACEBUSY:         dumpmsg("  [DDERR_SURFACEBUSY]");                    break;
                    case DDERR_UNSUPPORTED:         dumpmsg("  [DDERR_UNSUPPORTED]");     break;
                    case DDERR_UNSUPPORTEDMODE:     dumpmsg("  [DDERR_UNSUPPORTEDMODE]");     break;
                    case DDERR_WASSTILLDRAWING:     dumpmsg("  [DDERR_WASSTILLDRAWING]");                    break;
                    case DD_OK:                     dumpmsg("  [OK]");                    break;
                    default:                        dumpmsg("  [UNKNOWN]");               break;
                }

                if (lpDDSPrimary->IsLost() == DDERR_SURFACELOST) 
                {
                    dumpmsg("restoring primary surface...");
                    Sleep(400);
                    ddrval = lpDDSPrimary->Restore();
                    switch(ddrval)
                    {
                        case DDERR_INVALIDOBJECT:       dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
                        case DDERR_INVALIDPARAMS:       dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
                        case DDERR_GENERIC:             dumpmsg("  [DDERR_GENERIC]");         break;
                        case DDERR_IMPLICITLYCREATED:   dumpmsg("  [DDERR_IMPLICITLYCREATED]"); break;
                        case DDERR_INCOMPATIBLEPRIMARY: dumpmsg("  [DDERR_INCOMPATIBLEPRIMARY]"); break;
                        case DDERR_OUTOFMEMORY:         dumpmsg("  [DDERR_OUTOFMEMORY]");     break;
                        case DDERR_NOEXCLUSIVEMODE:     dumpmsg("  [DDERR_NOEXCLUSIVEMODE]"); break;
                        case DDERR_UNSUPPORTED:         dumpmsg("  [DDERR_UNSUPPORTED]");     break;
                        case DDERR_WRONGMODE:           dumpmsg("  [DDERR_WRONGMODE]");       break;
                        case DD_OK:                     dumpmsg("  [OK]");                    break;
                        default: { char buf[256]; sprintf(buf, "  [UNKNOWN: %08x]"); dumpmsg(buf); } break;
                    }
                    Sleep(50);
                }

                if (lpDDSBack->IsLost() == DDERR_SURFACELOST) 
                {
                    dumpmsg("restoring back surface...");
                    Sleep(400);
                    ddrval = lpDDSBack->Restore();
                    switch(ddrval)
                    {
                        case DDERR_INVALIDOBJECT:       dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
                        case DDERR_INVALIDPARAMS:       dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
                        case DDERR_GENERIC:             dumpmsg("  [DDERR_GENERIC]");         break;
                        case DDERR_IMPLICITLYCREATED:   dumpmsg("  [DDERR_IMPLICITLYCREATED]"); break;
                        case DDERR_INCOMPATIBLEPRIMARY: dumpmsg("  [DDERR_INCOMPATIBLEPRIMARY]"); break;
                        case DDERR_OUTOFMEMORY:         dumpmsg("  [DDERR_OUTOFMEMORY]");     break;
                        case DDERR_NOEXCLUSIVEMODE:     dumpmsg("  [DDERR_NOEXCLUSIVEMODE]"); break;
                        case DDERR_UNSUPPORTED:         dumpmsg("  [DDERR_UNSUPPORTED]");     break;
                        case DDERR_WRONGMODE:           dumpmsg("  [DDERR_WRONGMODE]");       break;
                        case DD_OK:                     dumpmsg("  [OK]");                    break;
                        default: { char buf[256]; sprintf(buf, "  [UNKNOWN: %08x]"); dumpmsg(buf); } break;
                    }
                    Sleep(50);
                }
                */
/*
                RenderFX();        // updates VS2

                iStep = 3;
            }

            if (iStep==3 && !g_rush_map && !g_QuitASAP)
            {
                Process_Map(VS1, VS2);//, lpDDSBack);
                iStep = 6;
            }


            if (iStep==6 && !g_rush_map && !g_QuitASAP)
            {
                GetWaveData();
                RenderDots(VS2);
                RenderWave(VS2);

                //if (intframe > 5)
                //{
                    //if (FXW!=320) RenderWave(VS2, VS4, VS6);
                    //RenderDots(VS2, VS4, VS6);
                //}
                iStep = 7;
            }

            if (iStep==7 && !g_rush_map && !g_QuitASAP)
            {
                //----------- step 2: swap buffers & put VS2 to the back surface
                TEMPPTR = VS1;    VS1 = VS2;    VS2 = TEMPPTR;
                #if (GRFX==1)
                    Plop_Songtitle_Using_Backbuffer_As_Scratch(0);
                    Merge_All_VS_To_Backbuffer();
                    Plop_Songtitle_Using_Backbuffer_As_Scratch(1);
                #endif
                iStep = 8;
            }
            
            // skip the text-message step if messages are suppressed.
            if (iStep==8 && g_bSuppressAllMsg) iStep = 9;

            if (iStep==8 && !g_rush_map && !g_QuitASAP)
            {
                //----------- step 3: fps to back surface
                //float time_array[120] = { 0 };
                //int   time_array_pos = 0;
                //bool  time_array_ready = false;

                float now = clock();

                if (time_array_ready)
                {
                    float mix = (intframe <= 32) ? 0 : 0.95;
                    fps = fps*mix + (1-mix) * 30.0/(( now - time_array[time_array_pos]) / (float)CLOCKS_PER_SEC);
                }
                
                time_array[time_array_pos++] = now;
                if (time_array_pos>=30) { time_array_pos = 0; time_array_ready = true; }

                if (!g_bSuppressAllMsg)
                {
                    #if (GRFX==1)
                        if (SHOW_TRACK_MSG>0 && !g_QuitASAP)
                        {
                            SHOW_TRACK_MSG--;
                            Put_Trackmsg_To_Backbuffer();
                        }
                        if (SHOW_LOCKED_MSG>0 && !g_QuitASAP)
                        {
                            SHOW_LOCKED_MSG--;
                            //Put_Lockedmsg_To_Backbuffer();
                            Put_Msg_To_Backbuffer(szLM);
                        }
                        else if (SHOW_UNLOCKED_MSG>0 && !g_QuitASAP)
                        {
                            SHOW_UNLOCKED_MSG--;
                            //Put_Unlockedmsg_To_Backbuffer();
                            Put_Msg_To_Backbuffer(szULM);
                        }
                        else if (SHOW_MISC_MSG>0 && !g_QuitASAP)
                        {
                            SHOW_MISC_MSG--;
                            Put_Msg_To_Backbuffer(szMisc);
                        }
                        else if (SHOW_MODEPREFS_MSG>0 && !g_QuitASAP)
                        {
                            SHOW_MODEPREFS_MSG--;
                            char msg[128];
                            sprintf(msg, " Mode %d: [", mode);
                            for (int i=0; i<modeprefs[mode]; i++)
                                strcat(msg, "*");
                            sprintf(msg + strlen(msg), "] (%d stars) ", modeprefs[mode]);
                            Put_Msg_To_Backbuffer(msg);
                        }

                        else if (SHOW_MOUSECLICK_MSG>0 && !g_bSuppressHelpMsg && !g_QuitASAP)
                        {
                            SHOW_MOUSECLICK_MSG--;
                            //Put_Mouseclickmsg_To_Backbuffer();
                            Put_Msg_To_Backbuffer(szMCM);
                        }
                        else if (SHOW_DEBUG > 0 && !g_QuitASAP)
                            Put_Msg_To_Backbuffer(szDEBUG);
                            //Put_Debugmsg_To_Backbuffer();
                        else if (SHOW_FPS && !g_QuitASAP)
                            Put_FPS_To_Backbuffer();

                        if (SHOW_HELP_MSG && !g_QuitASAP)
                            Put_Helpmsg_To_Backbuffer();
                    #endif
                }

                iStep = 9;
            }

            if (iStep==9 && !g_rush_map && !g_QuitASAP)
            {
                #if (GRFX==1)
                    //----------- flip buffers
                    //clock_t temp_clock = clock();
                    HRESULT ddrval = lpDDSPrimary->Flip( NULL, DDFLIP_WAIT );
                    //flip_clock_time += clock() - temp_clock;
                    if (ddrval != DD_OK) 
                    {
            g_bLost = !Try_To_Recover();
            /*
                        dumpmsg("lpDDSPrimary->Flip() failed.");
                        switch(ddrval)
                        {
                            case DDERR_GENERIC:         dumpmsg("  [DDERR_GENERIC]");         break;
                            case DDERR_INVALIDOBJECT:   dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
                            case DDERR_INVALIDPARAMS:   dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
                            case DDERR_NOFLIPHW:        dumpmsg("  [DDERR_NOFLIPHW]");        break;
                            case DDERR_NOTFLIPPABLE:    dumpmsg("  [DDERR_NOTFLIPPABLE]");    break;
                            case DDERR_SURFACEBUSY:     dumpmsg("  [DDERR_SURFACEBUSY]");     break;
                            case DDERR_SURFACELOST:     dumpmsg("  [DDERR_SURFACELOST]");     break;
                            case DDERR_UNSUPPORTED:     dumpmsg("  [DDERR_UNSUPPORTED]");     break;
                            case DDERR_WASSTILLDRAWING: dumpmsg("  [DDERR_WASSTILLDRAWING]"); break;
                            case DD_OK:                 dumpmsg("  [OK]");                    break;
                            default:                    dumpmsg("  [UNKNOWN]");               break;
                        }
                        /*
                        if (ddrval==DDERR_SURFACELOST)
                        {
                            dumpmsg("Calling ->Restore()...");
                            ddrval = lpDDSPrimary->Restore();
                            switch(ddrval)
                            {
                                case DDERR_INVALIDOBJECT:       dumpmsg("  [DDERR_INVALIDOBJECT]");   break;
                                case DDERR_INVALIDPARAMS:       dumpmsg("  [DDERR_INVALIDPARAMS]");   break;
                                case DDERR_GENERIC:             dumpmsg("  [DDERR_GENERIC]");         break;
                                case DDERR_IMPLICITLYCREATED:   dumpmsg("  [DDERR_IMPLICITLYCREATED]"); break;
                                case DDERR_INCOMPATIBLEPRIMARY: dumpmsg("  [DDERR_INCOMPATIBLEPRIMARY]"); break;
                                case DDERR_OUTOFMEMORY:         dumpmsg("  [DDERR_OUTOFMEMORY]");     break;
                                case DDERR_NOEXCLUSIVEMODE:     dumpmsg("  [DDERR_NOEXCLUSIVEMODE]"); break;
                                case DDERR_UNSUPPORTED:         dumpmsg("  [DDERR_UNSUPPORTED]");     break;
                                case DDERR_WRONGMODE:           dumpmsg("  [DDERR_WRONGMODE]");       break;
                                case DD_OK:                     dumpmsg("  [OK]");                    break;
                                default:                        dumpmsg("  [UNKNOWN]");               break;
                            }
                        }
                        */
                        //Try_To_Recover(lpDDSPrimary, ddrval, true);
/*                    }
                #endif

                iStep = 10;
            }

            if (iStep==10 && !g_rush_map && !g_QuitASAP)
            {
                // frame delay (ms)
                if (iRegistryDelay>0) Sleep(iRegistryDelay);

                iStep = 1;
            }

        }

        /*
        if (g_rush_map)
        {
            Sleep(10);
        }
        */
/*
        if (intframe%2==0)
        {
            if (g_DDRAW_FRAGGED_req)
            {
                g_DDRAW_FRAGGED_ack = true;
            }

            if (g_PassDialogReq) 
            {
                g_PassDialogAck = true;
            }

            if (g_PowerSaveReq)
            {
                g_PowerSaveAck = true; // acknowledgement that we're really ready to pause
                Sleep(10);
                dumpmsg("!");
            }
        }

        /*
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            // heh heh... this should never execute.

            char buf[256];  // pwr mgmt
            sprintf(buf, "[GEISSPROC MESSAGE] frame %d: msg=0x%x, wParam=0x%x", intframe, msg.message, msg.wParam);
            dumpmsg(buf);

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        */
     //}

     /*
     dumpmsg("Normal program termination (end of crunchy thread).");
     g_GeissProcFinished = true;
     //PostQuitMessage(0);
     //DestroyWindow(hMainWnd);

     return;   // THE THREAD ENDS HERE.
     */
/*}
#endif*/


BOOL DoSaver(HWND hparwnd, HINSTANCE hInstance)
{ 

  /*
  WNDCLASS wc;
  wc.style=CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc=SaverWindowProc;
  wc.cbClsExtra=0;
  wc.cbWndExtra=0;
  wc.hInstance=hInstance;
  wc.hIcon=NULL;
  wc.hCursor=NULL;
  wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName=NULL;
  wc.lpszClassName="ScrClass";
  RegisterClass(&wc);
  if (ScrMode==smPreview)
  { RECT rc; GetWindowRect(hparwnd,&rc);
    int cx=rc.right-rc.left, cy=rc.bottom-rc.top;  
    hScrWindow=CreateWindowEx(0,"ScrClass","SaverPreview",WS_CHILD|WS_VISIBLE,0,0,cx,cy,hparwnd,NULL,hInstance,NULL);
  }
  else
  { int cx=GetSystemMetrics(SM_CXSCREEN), cy=GetSystemMetrics(SM_CYSCREEN);
    DWORD exstyle, style;
    if (DEBUG) { cx=cx/3; cy=cy/3; exstyle=0; style=WS_OVERLAPPEDWINDOW|WS_VISIBLE;}
    else {exstyle=WS_EX_TOPMOST; style=WS_POPUP|WS_VISIBLE;}
    hScrWindow=CreateWindowEx(exstyle,"ScrClass","SaverWindow",style,0,0,cx,cy,NULL,NULL,hInstance,NULL);
  }
  if (hScrWindow==NULL) return;
  UINT oldval;
  if (ScrMode==smSaver) SystemParametersInfo(SPI_SCREENSAVERRUNNING,1,&oldval,0);
  MSG msg;
  while (GetMessage(&msg,NULL,0,0))
  { TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  if (ScrMode==smSaver) SystemParametersInfo(SPI_SCREENSAVERRUNNING,0,&oldval,0);
  return;
  */

  if (ScrMode==smSaver)
  {
      //GUID *lpGuid = NULL;

      /*
      if (AUTOMIN)
      {
          HWND hTemp[100];
          int its=0;
          BOOL bOK = TRUE;
          hTemp[0] = ::GetForegroundWindow();
          RECT r;

          while (hTemp[its] && bOK && its<30)
          {
              ::GetWindowRect(hTemp[its], &r);
              if (hTemp[its] != hparwnd && r.bottom - r.top > 100)
              {
                ::SendMessage(hTemp[its], WM_SYSCOMMAND, SC_MINIMIZE, 0);
              }
              its++;
              hTemp[its] = ::GetForegroundWindow();
              
              for (int i=0; i<its; i++) if (hTemp[its]==hTemp[i]) bOK = FALSE;
          }
      }
      */

      
#if SAVER


    // tempsound
    if (SoundEnabled) dumpmsg("SoundEnabled=1"); else dumpmsg("SoundEnabled=0");
    if (SoundReady) dumpmsg("SoundReady=1"); else dumpmsg("SoundReady=0");
    if (SoundActive) dumpmsg("SoundActive=1"); else dumpmsg("SoundActive=0");
    if (SoundEmpty) dumpmsg("SoundEmpty=1"); else dumpmsg("SoundEmpty=0");

      if (SoundEnabled)
      {
          g_lpGuid = NULL;
          HRESULT hr;
          DSCBCAPS caps;

          dumpmsg("DirectSoundCaptureEnumerate()...");

          hr = ::DirectSoundCaptureEnumerate(&DSEnumCallback, NULL);
          if (hr != DS_OK)
          {
              MessageBox( hparwnd, "DirectSoundCaptureEnumerate() failed - disabling sound.", "ERROR", MB_OK );
              dumpmsg("DirectSoundCaptureEnumerate failed - disabling sound.");
              SoundEnabled = FALSE;
              SoundReady = FALSE;
              SoundActive = FALSE;
              //exit(860);        // just for test purposes
          }

          //if (SoundReady)
          //{
          //      caps.dwSize = sizeof(DSCBCAPS);
          //      pDSCB->GetCaps(&caps);
          //}
      }

    // tempsound
    if (SoundEnabled) dumpmsg("SoundEnabled=1"); else dumpmsg("SoundEnabled=0");
    if (SoundReady) dumpmsg("SoundReady=1"); else dumpmsg("SoundReady=0");
    if (SoundActive) dumpmsg("SoundActive=1"); else dumpmsg("SoundActive=0");
    if (SoundEmpty) dumpmsg("SoundEmpty=1"); else dumpmsg("SoundEmpty=0");

#endif

     MSG msg;

     //if( !doInit( hInstance, nCmdShow ) )        // hInstance used to be passed to us by WinMain...
     
     if( !doInit( hInstance, SW_MAXIMIZE ) )
     {
         return false;
     }


     //UINT oldval;
     //SystemParametersInfo(SPI_SCREENSAVERRUNNING,1,&oldval,0);
     /*while (GetMessage(&msg,NULL,0,0))
     {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
     }*/





    // ---------------- FOR THE SCREENSAVER ONLY -----------------------

#if SAVER

    // tempsound
    if (SoundEnabled) dumpmsg("SoundEnabled=1"); else dumpmsg("SoundEnabled=0");
    if (SoundReady) dumpmsg("SoundReady=1"); else dumpmsg("SoundReady=0");
    if (SoundActive) dumpmsg("SoundActive=1"); else dumpmsg("SoundActive=0");
    if (SoundEmpty) dumpmsg("SoundEmpty=1"); else dumpmsg("SoundEmpty=0");

    if (SoundReady)
    {
        HRESULT hr;
        dumpmsg("starting capture buffer...");
        hr = pDSCB->Start(DSCBSTART_LOOPING);
        if (hr != DS_OK)
        {
            dumpmsg("Capture Buffer start failed... aborting.");
            //exit(830);
            SoundEnabled = FALSE;
            SoundActive = FALSE;
            SoundReady = FALSE;
        }
    }

    // tempsound
    if (SoundEnabled) dumpmsg("SoundEnabled=1"); else dumpmsg("SoundEnabled=0");
    if (SoundReady) dumpmsg("SoundReady=1"); else dumpmsg("SoundReady=0");
    if (SoundActive) dumpmsg("SoundActive=1"); else dumpmsg("SoundActive=0");
    if (SoundEmpty) dumpmsg("SoundEmpty=1"); else dumpmsg("SoundEmpty=0");

    if (!g_bDisableCdControls)
    {
        dumpmsg("checking cd audio...");
        mciSendString("open cdaudio wait",NULL,NULL,NULL);
        mciSendString("set cdaudio time format tmsf wait",NULL,NULL,NULL);

        CheckForNewCD();
    }

    // tempsound
    if (SoundEnabled) dumpmsg("SoundEnabled=1"); else dumpmsg("SoundEnabled=0");
    if (SoundReady) dumpmsg("SoundReady=1"); else dumpmsg("SoundReady=0");
    if (SoundActive) dumpmsg("SoundActive=1"); else dumpmsg("SoundActive=0");
    if (SoundEmpty) dumpmsg("SoundEmpty=1"); else dumpmsg("SoundEmpty=0");


    //dumpmsg("starting GeissProc thread...");

    //ThreadNr++;            
    //_beginthread( &GeissProc, 0, 0 );
    msg.message = WM_ACTIVATE; // just to get inside the main loop.

    while (msg.message != WM_QUIT && !g_QuitASAP) //&& msg.message != WM_SYSCOMMAND  //(GetMessage(&msg, NULL, 0, 0))
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)==0)
        {
              //GeissProc(0); // ~Tick()
        render1(NULL);

            //Sleep(10); // milliseconds
        }

        char buf[256];  // pwr mgmt
        sprintf(buf, "frame %d: msg=0x%x, wParam=0x%x", intframe, msg.message, msg.wParam);
        dumpmsg(buf);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //_endthread();   // not necessary because exit() from thread's running function kills it.
    g_QuitASAP = true;
    /*
    dumpmsg("Waiting for GeissProc thread to end.");

    while (!g_GeissProcFinished)
    {
        Sleep(10);  // wait for GeissProc to finish...
    }*/

    dumpmsg("GeissProc ended.  Calling finiObjects.");
    finiObjects();
    dumpmsg("FiniObjects complete - ending program.");

            /*
            // finish up message queue...
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                char buf[256];  // pwr mgmt
                sprintf(buf, "frame %d: msg=0x%x, wParam=0x%x", intframe, msg.message, msg.wParam);
                dumpmsg(buf);

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            dumpmsg("End.");
            */


    exit(0);
#endif

  }
  return true;
}

/*
BOOL CALLBACK AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( message == WM_COMMAND && LOWORD(wParam) == IDOK )
    {
        EndDialog( hDlg, FALSE );
        return TRUE;
    }
    else return FALSE;
}
*/

void AddQuotes(TCHAR* s)
{
    if (s[0]=='\"')
        return;

    TCHAR buf[8192];
    strcpy(buf, s);
    sprintf(s, ("\"%s\""), buf);
}

void RemoveBackslash(TCHAR* s)
{
    #ifdef UNICODE
        if (s[0]==0)    // string was empty
            return;
        // string has at least one TCHAR
        TCHAR *final_char = GetPointerToFinalChar(s);
        if (*final_char == '\\')
            *final_char = 0;
    #else   
        int len = strlen(s);
        if (len > 0 && s[len-1] == '\\')
            s[len-1] = 0;
    #endif
}

void RemoveQuotes(TCHAR* s)
{
    if (*s != '\"')
        return;
    char* final_char = s + strlen(s)-1;
    if (*final_char != '\"')
        return;

    // the following should be safe for both MBCS and regular:
    int len = strlen(s); 
    for (int i=0; i<len-2; i++)
        s[i] = s[i+1];
    s[len-2] = 0;
}

void GuessDefaultDisplayMode()
{
    if (FXW > 0)
        return;

    DEVMODE active_vid_mode;
    EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &active_vid_mode );

    FXW = active_vid_mode.dmPelsWidth;
    FXH = active_vid_mode.dmPelsHeight;
    iDispBits = active_vid_mode.dmBitsPerPel;

    OnFxwFxhUpdated();
}

bool EnumVideoModes(GUID* pGUID, HWND hwnd)
{
    //LPDIRECTDRAW   lpDD = NULL;           // DirectDraw object
    //LPDIRECTDRAW2  lpDD2 = NULL;          // DirectDraw object
    char buf[1024];

    dumpmsg("calling DirectDrawCreate() for config panel...");

    HRESULT ddrval = DirectDrawCreate( pGUID, &lpDD, NULL );
    if (ddrval != DD_OK)
    {
        wsprintf(buf, "Direct Draw Init Failed for config (DirectDrawCreate) - you probably don't have DirectX installed! (%08lx)\n", ddrval );
        MessageBox( hwnd, buf, "ERROR", MB_OK );
          dumpmsg(buf); 
        return false;
    }

    dumpmsg("calling SetCooperativeLevel() for config panel...");
    ddrval = lpDD->SetCooperativeLevel( hwnd, DDSCL_NORMAL );//EXCLUSIVE | DDSCL_FULLSCREEN );
    if (ddrval != DD_OK)
    {
        wsprintf(buf, "Direct Draw Init Failed for config (SetCooperativeLevel) - you probably don't have DirectX installed! (%08lx)\n", ddrval );
        MessageBox( hwnd, buf, "ERROR", MB_OK );
          dumpmsg(buf); 
            if (lpDD) 
          lpDD->Release();
        return false;
    }

    // DO THIS PRIOR TO CALLING EnumDisplayModes / EnumModesCallback:
    GuessDefaultDisplayMode();

    dumpmsg("calling EnumDisplayModes()...");
    iNumVidModes = 0;
    VidMode = -1;
    HRESULT hr = lpDD->EnumDisplayModes(0, NULL, NULL, &EnumModesCallback);
    if (hr == DD_OK && iNumVidModes>0) 
    {
        SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_RESETCONTENT, 0, 0);
        for (int i=0; i<iNumVidModes; i++)
        {
            int x = SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_ADDSTRING, 0, (LPARAM)VidList[i].name);
            SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_SETITEMDATA, x, i);
        }
        // set prior selection:
        SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_SETCURSEL, 0, 0);
        for (int i=0; i<iNumVidModes; i++)
        {
            int y = SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_GETITEMDATA, i, 0);
            if (y==VidMode)
                SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_SETCURSEL, i, 0);
        }
    }
    else
    {
          SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_ADDSTRING, 0, (LPARAM)"(enum failed!)");
          SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_SETITEMDATA, 0, -1);
          SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_SETCURSEL, 0, 0);
    }

    if (lpDD)
    {
        lpDD->Release();
        lpDD = NULL;
    }

    return true;
}

void SaveVidMode()
{
    FXW = VidList[VidMode].FXW;
    FXH = VidList[VidMode].FXH;
    iDispBits = VidList[VidMode].iDispBits;
    VIDEO_CARD_FUCKED = VidList[VidMode].VIDEO_CARD_FUCKED;
    //bModeX = VidList[VidMode].bModeX;
}

BOOL CALLBACK	ConfigDialogProc(HWND	hwnd,UINT	msg,WPARAM wParam,LPARAM lParam)
{
	char buf[2048];
	int	x;

	switch (msg)
	{	
		/*
		case WM_KEYDOWN:
				if (wParam==VK_ESCAPE)
				{
						//EndDialog(hwnd,IDCANCEL);
						//exit(0);
						SendMessage( hwnd, WM_COMMAND, IDCANCEL, 0);
				}
				break;
		*/
		case WM_INITDIALOG:
		{	

			HRESULT	hr;

			//-------------- DirectSound mode	detection	---------------------
#if	SAVER
			g_lpGuid = NULL;

			dumpmsg("calling DirectSoundCaptureEnumerate from	startup	of config	panel...");
			hr = ::DirectSoundCaptureEnumerate(&DSEnumCallback,	NULL);
			dumpmsg("back	from DirectSoundCaptureEnumerate.");
			if (hr ==	DS_OK	&& iNumSoundDrivers>0) 
			{
					for	(int i=0;	i<iNumSoundDrivers;	i++)
					{
							SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_ADDSTRING,	0, (LPARAM)szSoundDrivers[i]);
							SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_SETITEMDATA,	i, i);
					}
					SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_SETCURSEL,	iCurSoundDriver, 0);
			}
			else
			{
					// no	sound	card found,	disabling	sound:
					SoundEnabled = FALSE;
					CheckDlgButton(hwnd, IDC_SOUNDON,	SoundEnabled);

				  SendMessage( GetDlgItem( hwnd, IDC_SOUND ),  CB_ADDSTRING,  0, (LPARAM)"n/a");
          SendMessage( GetDlgItem( hwnd, IDC_SOUND ),  CB_SETITEMDATA,  0, -1);
          SendMessage( GetDlgItem( hwnd, IDC_SOUND ),  CB_SETCURSEL,  0, 0);
			}
#endif
#if	PLUGIN
					SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_ADDSTRING,	0, (LPARAM)"Winamp");
					SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_SETITEMDATA,	0, -1);
					SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_SETCURSEL,	0, 0);

					//potato
					//::SetWindowPos(this_mod_hwndParent,	
					//			HWND_TOPMOST,	0, 0,	0, 0,	SWP_NOMOVE | SWP_NOSIZE	);
							 



#endif

				//-------------- DirectDraw	video	mode detection ---------------------
		g_nDevices = 0;
		HRESULT	ddrval = DirectDrawEnumerateExA(EnumDevicesCallback, NULL, DDENUM_ATTACHEDSECONDARYDEVICES|DDENUM_DETACHEDSECONDARYDEVICES);
        if (ddrval !=	DD_OK)
				{
						wsprintf(buf,	"DirectDrawEnumerateEx failed	-	you	probably don't have	DirectX	installed! (%08lx)\n", ddrval	);
						MessageBox(	hwnd,	buf, "ERROR",	MB_OK	);
							dumpmsg(buf);	
						//finiObjects(); //3.40
			
						int	id=LOWORD(wParam);
						EndDialog(hwnd,id);
						//exit(66);
				}

		// Add the GUIDs to	the	combo	box, and select	the	right	one
		int	nGuidSel = -1;
    for	(int k=0;	k<g_nDevices;	k++)
    {
      if (strcmp(g_device[k].szName, "display")==0 && strcmp(g_device[k].szDesc, "Primary Display Driver")==0)
        strcpy(buf, "(Default Monitor)");
      else if (strncmp(g_device[k].szName, "\\\\.\\DISPLAY", 11)==0)
        sprintf(buf, "Display %s on %s", &g_device[k].szName[11], g_device[k].szDesc);
      else 
      {
        char* szName = g_device[k].szName;
        if (strncmp(szName, "\\\\.\\", 4)==0)
          szName += 4;
			  sprintf(buf, "%s  (...%s)", szName, g_device[k].szDesc);
      }
			SendMessage( GetDlgItem( hwnd, IDC_ADAPTER ),	CB_ADDSTRING,	k, (LPARAM)buf );
			if (memcmp(&g_device[k].guid,	&g_DDrawDeviceGUID,	sizeof(GUID))==0)
				nGuidSel = k;
		}
		if (nGuidSel==-1)	
    {
			nGuidSel = 0;
			g_DDrawDeviceGUID	=	g_device[0].guid;
		}
		SendMessage( GetDlgItem(hwnd,	IDC_ADAPTER),	CB_SETCURSEL,	nGuidSel,	0);

		EnumVideoModes(&g_DDrawDeviceGUID, hwnd);


				//---------------------------------------------------------------

				dumpmsg("Setting initial states	of controls	for	config panel...");

			/*
			SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER),	TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER),	TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(25) );
			SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER),	TBM_SETPOS,
								 TRUE, (LPARAM)(iRegistryDelay)	);
			
			SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER), TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(100)	);
			SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER), TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(1000) );
			SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER), TBM_SETPOS,
								 TRUE, (LPARAM)(frames_til_auto_switch)	);
			*/
			SendMessage( GetDlgItem( hwnd, IDC_GAMMA_SLIDER),	TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_GAMMA_SLIDER),	TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(100)	);
			SendMessage( GetDlgItem( hwnd, IDC_GAMMA_SLIDER),	TBM_SETPOS,
								 TRUE, (LPARAM)(gamma) );

			SendMessage( GetDlgItem( hwnd, IDC_COARSE_SLIDER), TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_COARSE_SLIDER), TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(5)	);
			SendMessage( GetDlgItem( hwnd, IDC_COARSE_SLIDER), TBM_SETPOS,
								 TRUE, (LPARAM)(coarse_pal_freq) );

			SendMessage( GetDlgItem( hwnd, IDC_SOLAR_SLIDER),	TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_SOLAR_SLIDER),	TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(5)	);
			SendMessage( GetDlgItem( hwnd, IDC_SOLAR_SLIDER),	TBM_SETPOS,
								 TRUE, (LPARAM)(solar_pal_freq)	);

			SendMessage( GetDlgItem( hwnd, IDC_WAVESCALE), TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_WAVESCALE), TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(20) );
			SendMessage( GetDlgItem( hwnd, IDC_WAVESCALE), TBM_SETPOS,
								 TRUE, (LPARAM)(volpos)	);
/*
		#if	PLUGIN
			SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER), TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER), TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(15) );
			SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER), TBM_SETPOS,
								 TRUE, (LPARAM)(g_random_songtitle_freq) );
		#endif
*/
			//CheckDlgButton(hwnd, IDC_AUTOMIN,	AUTOMIN);
			//CheckDlgButton(hwnd, IDC_VIDEOFIX, VIDEO_CARD_FUCKED);
			//dither//CheckDlgButton(hwnd, IDC_DITHER, bDither);
			//dither//CheckDlgButton(hwnd, IDC_RANDOMDITHER, bRandomDither);
			CheckDlgButton(hwnd, IDC_SUPPRESSALLMSG, g_bSuppressAllMsg);
			CheckDlgButton(hwnd, IDC_SOUNDON,	SoundEnabled);
			CheckDlgButton(hwnd, IDC_DEBUGINFO,	g_bDebugMode);
			CheckDlgButton(hwnd, IDC_BEATDET,	g_bUseBeatDetection);
			CheckDlgButton(hwnd, IDC_WAVECOLOR,	!g_bSyncColorToSound);
			CheckDlgButton(hwnd, IDC_HELPMSG,	g_bSuppressHelpMsg);
			#if	PLUGIN
					CheckDlgButton(hwnd, IDC_MAGIC,	g_bDisableSongTitlePopups);
					CheckDlgButton(hwnd, IDC_MAGIC2, g_bMinimizeWinamp);
			#endif
			#if	SAVER
						CheckDlgButton(hwnd, IDC_MAGIC,	g_bDisableCdControls);
					CheckDlgButton(hwnd, IDC_MAGIC2, g_bAutostartCD);
			#endif
			//CheckDlgButton(hwnd, IDC_EXIT_ON_MOUSE,	bExitOnMouse);
			

			// initialize	slider bar
			SendMessage( GetDlgItem( hwnd, IDC_SIZESLIDER	), TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_SIZESLIDER	), TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(100)	);
			SendMessage( GetDlgItem( hwnd, IDC_SIZESLIDER	), TBM_SETPOS,
								 TRUE, (LPARAM)(iVSizePercent) );

			SendMessage( GetDlgItem( hwnd, IDC_SHIFT_SLIDER	), TBM_SETRANGEMIN,
								 FALSE,	(LPARAM)(0)	);
			SendMessage( GetDlgItem( hwnd, IDC_SHIFT_SLIDER	), TBM_SETRANGEMAX,
								 FALSE,	(LPARAM)(100)	);
			SendMessage( GetDlgItem( hwnd, IDC_SHIFT_SLIDER	), TBM_SETPOS,
								 TRUE, (LPARAM)(g_SlideShiftFreq)	);

#if	PLUGIN
			SetWindowText( GetDlgItem( hwnd, IDC_TEXT1 ),	"	"	);
			SetWindowText( GetDlgItem( hwnd, IDC_TEXT2 ),	"Tip: to use live audio input, hit CTRL+L in Winamp, and type in \"linein://\" (without the quotes).\nIf you still can't 'see' any sound, check your cables, and run \"sndvol32 /r\" (see button below) to configure your system's sound input settings." );
			SetWindowText( GetDlgItem( hwnd, IDC_MAGIC ),	"Suppress song title popups" );
			SetWindowText( GetDlgItem( hwnd, IDC_MAGIC2),	"Minimize	Winamp before	running	(FIXES FLICKER)" );
			//SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT),	"Random	song title popups:"	);
			::EnableWindow(GetDlgItem( hwnd, IDC_SOUNDON ),	false	);
#endif
#if	SAVER
			SetWindowText( GetDlgItem( hwnd, IDC_TEXT1 ),	"SOUND ENABLED will	make Geiss react to	music.	Play an	audio	CD,	pipe music into	your sound card, or	speak	into your	microphone to	see	the	effect." );
			//SetWindowText( GetDlgItem( hwnd, IDC_TEXT2 ),	"NOTE: Geiss was designed	to respond best	to sound in	a	certain	volume range.	 For best	response,	use	Windows' built-in	volume controls	to set the input volumes (line-in, cd-audio, and microphone) to	about	50%."	);
			SetWindowText( GetDlgItem( hwnd, IDC_TEXT2 ),	"If	you	can't	'see'	any	sound, check your	cables,	and	run	\"sndvol32 /r\"	(see button	below) to	configure	your system's	sound	input	settings.	 Make	sure the correct audio source(s) are selected, NOT muted,	and	that the volumes are in	the	50-80% range." );
			SetWindowText( GetDlgItem( hwnd, IDC_MAGIC ),	"Disable CD-ROM	drive	controls"	);
			SetWindowText( GetDlgItem( hwnd, IDC_MAGIC2),	"Auto-start	CD"	);
			//SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT),	"	"	);
			//SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT2), " " );
			//SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT3), " " );
			//::EnableWindow(GetDlgItem( hwnd, IDC_MAGICSLIDER), false );
			//::ShowWindow(GetDlgItem( hwnd, IDC_MAGICSLIDER), SW_HIDE );
#endif

			// initialize	res-selection	combo	box
			SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ),	CB_INITSTORAGE,
								 num_vid_modes,	256 );	 //	save space for 10	strings, each	64 bytes long.
			//for	(x=0;	x<num_vid_modes; x++)
			//			SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ),	CB_ADDSTRING,	
			//					0, (LPARAM)szVidMode[x]	);
			//SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ),	CB_SETCURSEL,	
			//				VidMode, 0 );

//#if	PLUGIN
//			SendMessage( GetDlgItem( hwnd, IDC_SOUNDON ),	WM_SHOWWINDOW, 0,	0	);
//			SendMessage( GetDlgItem( hwnd, IDC_SOUNDON ),	WM_ENABLE, 0,	0	);
//#endif

			return TRUE;
		}
		/*
		case WM_NOTIFY:
			{
				NMHDR* pnmh	=	(NMHDR*)lParam;
				if (pnmh->idFrom ==	IDC_ADAPTER)
				{
					// they	chose	a	new	adapter	-> refresh the video modes dialog	box.
						VidMode					=	SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ),	CB_GETCURSEL,	0, 0);
					SaveVidMode();
					EnumVideoModes(&g_DDrawDeviceGUID, hwnd);
				}
			}
			break;
			*/
		case WM_COMMAND:
		{	
			int	id=LOWORD(wParam);
			if (id==IDC_ADAPTER)
			{
					// they	chose	a	new	adapter	-> refresh the video modes dialog	box.

					// 2.	figure out what	the	NEW	device is
					int	nDevice			=	SendMessage( GetDlgItem( hwnd, IDC_ADAPTER ),	CB_GETCURSEL,	0, 0);
					if (nDevice	>= 0 &&	nDevice	<	g_nDevices)
					{
						g_DDrawDeviceGUID	=	g_device[nDevice].guid;

						// 1.	save selected	video	mode
						int sel = SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ),	CB_GETCURSEL,	0, 0);
            VidMode = SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_GETITEMDATA, sel, 0);
						SaveVidMode();

						// 3.	enum the video modes for the new device
						EnumVideoModes(&g_DDrawDeviceGUID, hwnd);
					}
			}
			if (id==IDOK)
			{	
				//if (DlgItemIsChecked(hwnd, IDC_VIDEOFIX))
				//		VIDEO_CARD_FUCKED	=	1;
				//else
				//		VIDEO_CARD_FUCKED	=	0;

				//dither//bDither						 = DlgItemIsChecked(hwnd,	IDC_DITHER);
				//dither//bRandomDither			 = DlgItemIsChecked(hwnd,	IDC_RANDOMDITHER);
				g_bDebugMode			 = DlgItemIsChecked(hwnd,	IDC_DEBUGINFO);
				g_bUseBeatDetection= DlgItemIsChecked(hwnd,	IDC_BEATDET);
				g_bSyncColorToSound= !DlgItemIsChecked(hwnd, IDC_WAVECOLOR);

				g_bSuppressHelpMsg = DlgItemIsChecked(hwnd,	IDC_HELPMSG);
				SoundEnabled			 = DlgItemIsChecked(hwnd,	IDC_SOUNDON);
				g_bSuppressAllMsg	 = DlgItemIsChecked(hwnd,	IDC_SUPPRESSALLMSG);

				#if	PLUGIN
						g_bDisableSongTitlePopups	=	DlgItemIsChecked(hwnd, IDC_MAGIC);
						g_bMinimizeWinamp					=	DlgItemIsChecked(hwnd, IDC_MAGIC2);
				#endif
				#if	SAVER
						g_bDisableCdControls			=	DlgItemIsChecked(hwnd, IDC_MAGIC);
						g_bAutostartCD						=	DlgItemIsChecked(hwnd, IDC_MAGIC2);
				#endif

				//AUTOMIN						 = DlgItemIsChecked(hwnd,	IDC_AUTOMIN);

		int	nDevice			=	SendMessage( GetDlgItem( hwnd, IDC_ADAPTER ),	CB_GETCURSEL,	0, 0);
		if (nDevice	>= 0 &&	nDevice	<	g_nDevices)
			g_DDrawDeviceGUID	=	g_device[nDevice].guid;
		else
			memset(&g_DDrawDeviceGUID, 0,	sizeof(GUID));

				int sel = SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ),	CB_GETCURSEL,	0, 0);
        VidMode = SendMessage( GetDlgItem( hwnd, IDC_BITDEPTHCOMBO ), CB_GETITEMDATA, sel, 0);
        SaveVidMode();
				iVSizePercent		=	SendMessage( GetDlgItem( hwnd, IDC_SIZESLIDER	), TBM_GETPOS, FALSE,	0	);
				g_SlideShiftFreq=	SendMessage( GetDlgItem( hwnd, IDC_SHIFT_SLIDER	), TBM_GETPOS, FALSE,	0	);
				//#if	PLUGIN
					//g_random_songtitle_freq	=	SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER ),	TBM_GETPOS,	FALSE, 0 );
				//#endif
				int	t						=	SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_GETCURSEL,	0, 0);
				iCurSoundDriver	=	SendMessage( GetDlgItem( hwnd, IDC_SOUND ),	CB_GETITEMDATA,	t, 0);
				//iRegistryDelay	=	SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER	), TBM_GETPOS, FALSE,	0	);
				solar_pal_freq	=	SendMessage( GetDlgItem( hwnd, IDC_SOLAR_SLIDER	), TBM_GETPOS, FALSE,	0	);
				coarse_pal_freq	=	SendMessage( GetDlgItem( hwnd, IDC_COARSE_SLIDER ),	TBM_GETPOS,	FALSE, 0 );
				gamma						=	SendMessage( GetDlgItem( hwnd, IDC_GAMMA_SLIDER	), TBM_GETPOS, FALSE,	0	);
				volpos					=	SendMessage( GetDlgItem( hwnd, IDC_WAVESCALE ),	TBM_GETPOS,	FALSE, 0 );
				//bExitOnMouse		=	SendMessage( GetDlgItem( hwnd, IDC_EXIT_ON_MOUSE ),	TBM_GETPOS,	FALSE, 0 );
				//frames_til_auto_switch = SendMessage(	GetDlgItem(	hwnd,	IDC_FRAMES_SLIDER	), TBM_GETPOS, FALSE,	0	);

				if (iCurSoundDriver<0	|| SendMessage(	GetDlgItem(	hwnd,	IDC_SOUND	), CB_GETITEMDATA, iCurSoundDriver,	0) ==	-1)
				{
						SoundEnabled = FALSE;
				}

				WriteConfigRegistry();
			}
			if (id==IDOK ||	id==IDCANCEL)	
			{
					g_ConfigAccepted = id==IDOK;
					EndDialog(hwnd,id);
					//exit(0); //beely
			}
			if (id==IDADVANCED)
			{
						DialogBox( hInstance,	MAKEINTRESOURCE(IDD_CONFIG2),	
							hwnd,	(DLGPROC)Config2DialogProc );	
			}
		if (id==ID_SNDVOL)
		{
				STARTUPINFO	si;
				PROCESS_INFORMATION	pi;
				ZeroMemory(&si,	sizeof(si));
				ZeroMemory(&pi,	sizeof(pi));
				si.cb	=	sizeof(si);
				//si.dwFlags = STARTF_USESHOWWINDOW;
				//si.wShowWindow = SW_SHOWNORMAL;
								
				char szSysDir[MAX_PATH + 64];
				GetSystemDirectory(szSysDir, sizeof(szSysDir));
				if (szSysDir[0]=='\"')
					RemoveQuotes(szSysDir);
				RemoveBackslash(szSysDir);

				char szAppName[MAX_PATH	+	64];
				char szCmdLine[MAX_PATH	+	64];
				sprintf(szAppName, "%s\\sndvol32.exe", szSysDir);
				sprintf(szCmdLine, "%s\\sndvol32.exe /r",	szSysDir);

				//CreateProcess("c:\\windows\\system32\\sndvol32.exe", "c:\\windows\\system32\\sndvol32.exe	/r", NULL, NULL, FALSE,	CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si,	&pi);
				CreateProcess(szAppName, szCmdLine,	NULL,	NULL,	FALSE, CREATE_DEFAULT_ERROR_MODE,	NULL,	NULL,	&si, &pi);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
		}
			if (id==IDABOUT)
			{
					DialogBox( hInstance,	MAKEINTRESOURCE(IDD_ABOUT),
							hwnd,	(DLGPROC)AboutDialogProc );
				 /*
					 wsprintf(buf, "\n"
											 "					'Geiss'	is copyright (c) 1998	Ryan M.	Geiss.	All	rights reserved.					\n"
											 "					Requires Microsoft DirectX 5.0 or	later.	Enjoy!					\n\n"
											 "					Comments/bugs: geiss.3@osu.edu					\n"
											 "					Latest version:	http://www.geisswerks.com/					\n\n"
											 "					Please remember	to look	away every 15	minutes.\n"
											 );
				 MessageBox( hwnd, buf,	"About Geiss 3.0", MB_OK );
				 */
			}
			if (id==IDREG)
			{
					dumpmsg("showing the disclaimer");
					DialogBox( hInstance,	MAKEINTRESOURCE(IDD_DISC), hwnd, (DLGPROC)DisclaimerDialogProc );	
					dumpmsg("done	with disclaimer");

				/*
					 wsprintf(buf, "					Q/S/E/U/G/D/A: control effects\n"
											 "					I: toggle	dither on/off\n"
#if	SAVER
											 "					O: toggle	sound	on/off\n"
#endif
											 "					N: New screen	(random)\n"
											 "					L: lock/unlock current screen\n"
											 "					W: next	waveform\n"
											 "					0..9:	select specific	map\n\n"
											 "					F: toggles Fps display\n"
											 "					H: toggles Help	display\n\n"
#if	PLUGIN
											 "					T: toggles song	Title	display\n"
											 "					SPACE: show	current	song title\n\n"
											 "					Z	X	C	V	B: <<	play pause stop	>> (for	Winamp)\n\n"
#endif
#if	SAVER
											 "					Z	X	C	V	B: <<	play pause stop	>> (for	audio	CDs)\n\n"
#endif
//											 "					F11: screen	capture	to FX??????.TGA			\n"
//											 "					F12: display FX.TGA	(must	be a 24-bit,		 \n"
//											 "											320x???, uncompressed	Targa			\n"
//											 "											in your	windows	directory)		 \n"
											 "					ESC	or mouse click:	exit\n");
				 MessageBox( hwnd, buf,	"Keys",	MB_OK	);
				*/
			}
		}	
		break;
	}
	return FALSE;

	//return ScreenSaverConfigureDialog(hwnd,	msg, wParam, lParam);

}



BOOL CALLBACK Config2DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  char buf[2048];
  int x;

  switch (msg)
  { 
    case WM_INITDIALOG:
    { 
        dumpmsg("Setting initial states of controls for config2 (timing) panel...");

        SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER), TBM_SETRANGEMIN,
                 FALSE, (LPARAM)(0) );
        SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER), TBM_SETRANGEMAX,
                 FALSE, (LPARAM)(25) );
        SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER), TBM_SETPOS,
                 TRUE, (LPARAM)(iRegistryDelay) );

        SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER), TBM_SETRANGEMIN,
                 FALSE, (LPARAM)(30) );
        SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER), TBM_SETRANGEMAX,
                 FALSE, (LPARAM)(2000) );
        SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER), TBM_SETPOS,
                 TRUE, (LPARAM)(frames_til_auto_switch__registry) );

        SendMessage( GetDlgItem( hwnd, IDC_SHIFTFRAMES_SLIDER), TBM_SETRANGEMIN,
                 FALSE, (LPARAM)(2) );
        SendMessage( GetDlgItem( hwnd, IDC_SHIFTFRAMES_SLIDER), TBM_SETRANGEMAX,
                 FALSE, (LPARAM)(50) );
        SendMessage( GetDlgItem( hwnd, IDC_SHIFTFRAMES_SLIDER), TBM_SETPOS,
                 TRUE, (LPARAM)(g_SlideShiftMinFrames) );

        /*
        SendMessage( GetDlgItem( hwnd, IDC_SHIFTFREQ_SLIDER), TBM_SETRANGEMIN,
                 FALSE, (LPARAM)(0) );
        SendMessage( GetDlgItem( hwnd, IDC_SHIFTFREQ_SLIDER), TBM_SETRANGEMAX,
                 FALSE, (LPARAM)(100) );
        SendMessage( GetDlgItem( hwnd, IDC_SHIFTFREQ_SLIDER), TBM_SETPOS,
                 TRUE, (LPARAM)(g_SlideShiftFreq) );
        */

        SendMessage( GetDlgItem( hwnd, IDC_DISSOLVE_SLIDER), TBM_SETRANGEMIN,
                 FALSE, (LPARAM)(2) );
        SendMessage( GetDlgItem( hwnd, IDC_DISSOLVE_SLIDER), TBM_SETRANGEMAX,
                 FALSE, (LPARAM)(100) );
        SendMessage( GetDlgItem( hwnd, IDC_DISSOLVE_SLIDER), TBM_SETPOS,
                 TRUE, (LPARAM)(g_song_tooltip_frames) );

        #if PLUGIN
            SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER), TBM_SETRANGEMIN,
                     FALSE, (LPARAM)(0) );
            SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER), TBM_SETRANGEMAX,
                     FALSE, (LPARAM)(15) );
            SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER), TBM_SETPOS,
                     TRUE, (LPARAM)(g_random_songtitle_freq) );
        #endif

        #if PLUGIN
            SetWindowText( GetDlgItem( hwnd, IDC_DISSOLVETIME), "No. of frames to show custom msgs / songtitles before dissolve:" );
        #endif

        #if SAVER    // hide "random song title popups" slider
            SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT), " " );
            SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT2), " " );
            SetWindowText( GetDlgItem( hwnd, IDC_SLIDERTEXT3), " " );
            ::EnableWindow(GetDlgItem( hwnd, IDC_MAGICSLIDER), false );
            ::ShowWindow(GetDlgItem( hwnd, IDC_MAGICSLIDER), SW_HIDE );
        #endif


        return TRUE;
    }
    case WM_COMMAND:
    { 
      int id=LOWORD(wParam);
      if (id==IDOK)
      { 
        #if PLUGIN
          g_random_songtitle_freq = SendMessage( GetDlgItem( hwnd, IDC_MAGICSLIDER ), TBM_GETPOS, FALSE, 0 );
        #endif
        iRegistryDelay            = SendMessage( GetDlgItem( hwnd, IDC_DELAY_SLIDER ), TBM_GETPOS, FALSE, 0 );
        frames_til_auto_switch__registry = SendMessage( GetDlgItem( hwnd, IDC_FRAMES_SLIDER ), TBM_GETPOS, FALSE, 0 );
        g_SlideShiftMinFrames     = SendMessage( GetDlgItem( hwnd, IDC_SHIFTFRAMES_SLIDER ), TBM_GETPOS, FALSE, 0 );
        //g_SlideShiftFreq          = SendMessage( GetDlgItem( hwnd, IDC_SHIFTFREQ_SLIDER ), TBM_GETPOS, FALSE, 0 );
        g_song_tooltip_frames     = SendMessage( GetDlgItem( hwnd, IDC_DISSOLVE_SLIDER ), TBM_GETPOS, FALSE, 0 );
      }
      if (id==IDOK || id==IDCANCEL) 
      {
          EndDialog(hwnd,id);
      }
    } 
    break;
  }
  return FALSE;

  //return ScreenSaverConfigureDialog(hwnd, msg, wParam, lParam);

}






BOOL CALLBACK DisclaimerDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  char buf[2048];
  int x;

  switch (msg)
  { 
    case WM_INITDIALOG:
    { 
        char buf[12000];

        SendMessage( GetDlgItem( hwnd, IDC_DISC ), EM_SETLIMITTEXT, 16000, 0 );

          strcpy(buf,
                "Geiss is entirely copyright (c) 1998-2022 by Ryan M. Geiss.\r\n"
                "It is licensed under the 3-Clause BSD License.\r\n"
                "\r\n"
                "SYSTEM REQUIREMENTS\r\n"
                "-------------------\r\n"
                "1. Microsoft Windows 95 or later.  NT is not officially supported.\r\n"
                "2. *** Microsoft DirectX 3.0 or later. ***\r\n"
                "3. Pentium-200 or faster processor.\r\n"
                "\r\n"
                );
        strcat(buf,
                "LICENSE AGREEMENT\r\n"
                "-----------------\r\n"
                "3-Clause BSD License\r\n"
                "\r\n"
                "Copyright (c) 1998-2022 Ryan Geiss (@geissomatik)\r\n"
                "\r\n"
                "Redistribution and use in source and binary forms, with or without\r\n"
                "modification, are permitted provided that the following conditions are met:\r\n"
                "\r\n"
                "1. Redistributions of source code must retain the above copyright notice, this\r\n"
                "   list of conditions and the following disclaimer.\r\n"
                "\r\n"
                "2. Redistributions in binary form must reproduce the above copyright notice,\r\n"
                "   this list of conditions and the following disclaimer in the documentation\r\n"
                "   and/or other materials provided with the distribution.\r\n"
                "\r\n"
                "3. Neither the name of the copyright holder nor the names of its\r\n"
                "   contributors may be used to endorse or promote products derived from\r\n"
                "   this software without specific prior written permission.\r\n"
                "\r\n"
                "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\r\n"
                "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\r\n"
                "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\r\n"
                "DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\r\n"
                "FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\r\n"
                "DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\r\n"
                "SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\r\n"
                "CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\r\n"
                "OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\r\n"
                "OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\r\n"
                "\r\n"
                );
        strcat(buf,
                "DISCLAIMER\r\n"
                "----------\r\n"
                "The author is not responsible for any damages or loss of data that\r\n"
                "occur to your system for using Geiss.  Geiss is distributed freely, and\r\n"
                "if you choose to use Geiss, you take all risk on yourself.  If your\r\n"
                "system appears to have problems when Geiss is run, you should immediately\r\n"
                "discontinue using Geiss.\r\n"
                "\r\n"
            );
#if SAVER
        strcat(buf,
                "USE PASSWORD FEATURE AT OWN RISK\r\n"
                "--------------------------------\r\n"
                "Passwords are supported since version 4.0, but might not be\r\n"
                "bulletproof.  It is highly recommended that you do not use Geiss\r\n"
                "as your screensaver if you have sensitive or private data on your\r\n"
                "machine.  The author is in NO WAY responsible for any loss of or\r\n"
                "damage to data due to the failure of passwords related to the use\r\n"
                "of Geiss.\r\n"
                "\r\n"
                /*
                "NO PASSWORD SUPPORT\r\n"
                "-------------------\r\n"
                "It is especially important that you know that GEISS DOES NOT SUPPORT PASSWORDS\r\n"
                "like normal screensavers.  If you want a screensaver with a password,\r\n"
                "use one of Windows' built-in savers.  The author is not responsible\r\n"
                "for any security issues that come about because Geiss does not support\r\n"
                "passwords.\r\n"
                "\r\n"
                */
            );
#endif
        SendMessage( GetDlgItem( hwnd, IDC_DISC ), WM_SETTEXT, 0, (LPARAM)buf );

        return TRUE;
    }
    case WM_COMMAND:
    { 
      int id=LOWORD(wParam);
      if (id==IDCANCEL)
      { 
          EndDialog(hwnd,id);
          return FALSE;
          //exit(0);
      }
      else if (id==IDOK)
      {
          EndDialog(hwnd,id);
          g_DisclaimerAgreed = true;
          return TRUE;
      }
    } 
    break;
  }
  return FALSE;

  //return ScreenSaverConfigureDialog(hwnd, msg, wParam, lParam);

}

BOOL CALLBACK AboutDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  char buf[2048];
  int x;

  switch (msg)
  { 
    case WM_INITDIALOG:
    { 
        SendMessage( GetDlgItem( hwnd, IDC_EDIT1 ), WM_SETTEXT, 0, (LPARAM)"guava@geisswerks.com" );
        SendMessage( GetDlgItem( hwnd, IDC_EDIT2 ), WM_SETTEXT, 0, (LPARAM)"http://www.geisswerks.com/" );
        return TRUE;
    }
    break;
    case WM_COMMAND:
    { 
      int id=LOWORD(wParam);
      if (id==IDOK)
      {
          EndDialog(hwnd,id);
          return TRUE;
      }
    }
    break;
  }
  return FALSE;
}



// This routine is for using ScrPrev. It's so that you can start the saver
// with the command line /p scrprev and it runs itself in a preview window.
// You must first copy ScrPrev somewhere in your search path
HWND CheckForScrprev()
{ HWND hwnd=FindWindow("Scrprev",NULL); // looks for the Scrprev class
  if (hwnd==NULL) // try to load it
  { STARTUPINFO si; PROCESS_INFORMATION pi; ZeroMemory(&si,sizeof(si)); ZeroMemory(&pi,sizeof(pi));
    si.cb=sizeof(si);
    si.lpReserved=NULL; si.lpTitle=NULL; si.dwFlags=0; si.cbReserved2=0; si.lpReserved2=0; si.lpDesktop=0;
    BOOL cres=CreateProcess(NULL,"Scrprev",0,0,FALSE,CREATE_NEW_PROCESS_GROUP | CREATE_DEFAULT_ERROR_MODE,
                            0,0,&si,&pi);
    if (!cres) {Debug("Error creating scrprev process"); return NULL;}
    DWORD wres=WaitForInputIdle(pi.hProcess,2000);
    if (wres==WAIT_TIMEOUT) {Debug("Scrprev never becomes idle"); return NULL;}
    if (wres==0xFFFFFFFF) {Debug("ScrPrev, misc error after ScrPrev execution");return NULL;}
    hwnd=FindWindow("Scrprev",NULL);
  }
  if (hwnd==NULL) {Debug("Unable to find Scrprev window"); return NULL;}
  ::SetForegroundWindow(hwnd);
  hwnd=GetWindow(hwnd,GW_CHILD);
  if (hwnd==NULL) {Debug("Couldn't find Scrprev child"); return NULL;}
  return hwnd;
}




int WINAPI WinMain(HINSTANCE h,HINSTANCE,LPSTR,int)
{ 
    dumpmsg(" ");
    dumpmsg(" ");
    dumpmsg("begin");
    //start_clock = clock();


    REMAP = (unsigned char *)(((((DWORD)_REMAP_VALUES)+255)/256)*256);
    REMAP2 = &REMAP[256];
    REMAP3 = &REMAP[512];

    g_nCores = GetNumCores();
   

    //OSVERSIONINFO osvi; 
    //osvi.dwOSVersionInfoSize = sizeof(osvi); 
    //fOnWin95 = (GetVersionEx(&osvi) && osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

    hInstance=h;
    HWND hwnd=NULL;
//#if PLUGIN
//    hwnd = this_mod_hwndParent;
//#endif

    GetDesktopDisplayMode();

    if (!QueryPerformanceFrequency(&m_high_perf_timer_freq))
        m_high_perf_timer_freq.QuadPart = 0;
    m_prev_end_of_frame.QuadPart = 0;


#if SAVER
    char *c=GetCommandLine();

    ScrMode = smNone;
/*
  while (*c != 0 && *c != '/') c++;
  if (*c=='/')
  {
      c++;
      if (*c=='s' || *c=='S')
      {
          ScrMode = smSaver;
      }
      else if (*c=='c' || *c=='C' || *c=='?')
      {
          ScrMode = smConfig;
      }
  }
*/

  if (*c=='\"') {c++; while (*c!=0 && *c!='\"') c++;} else {while (*c!=0 && *c!=' ') c++;}
  if (*c!=0) c++;
  while (*c==' ') c++;
  if (*c==0) {ScrMode=smConfig; hwnd=NULL;}
  else
  { 
    if (*c=='-' || *c=='/') c++;
    if (*c=='p' || *c=='P' || *c=='l' || *c=='L')
    { 
      c++; while (*c==' ' || *c==':') c++;
      if ((strcmp(c,"scrprev")==0) || (strcmp(c,"ScrPrev")==0) || (strcmp(c,"SCRPREV")==0)) hwnd=CheckForScrprev();
      else hwnd=(HWND)atoi(c);
      ScrMode=smPreview;
    }
    else if (*c=='s' || *c=='S') {ScrMode=smSaver; }
    else if (*c=='c' || *c=='C') {c++; while (*c==' ' || *c==':') c++; if (*c==0) hwnd=GetForegroundWindow(); else hwnd=(HWND)atoi(c); ScrMode=smConfig;}
    else if (*c=='a' || *c=='A') {c++; while (*c==' ' || *c==':') c++; hwnd=(HWND)atoi(c); ScrMode=smPassword;}
  }


    //if (fOnWin95)
    //{
        // Local reboot and hotkey control (on Win95) 
        BOOL dummy;
        SystemParametersInfo( SPI_SCREENSAVERRUNNING, TRUE, &dummy, 0 );

        atexit( RestoreCtrlAlt );
    //}

  if (ScrMode==smPassword) 
  {
     ChangePassword(hwnd);   
      //char buf[256];
     //wsprintf(buf, "Sorry, no password support with this screensaver!");
     //MessageBox( NULL, buf, "SORRY!", MB_OK );
  }
  if (ScrMode != smConfig && ScrMode != smSaver)
  {
      exit(0);
  }
#endif

    memset(&g_DDrawDeviceGUID, 0, sizeof(GUID));

//#if SAVER
    GetWindowsDirectory(inifile, 511);
//#endif
/*
#if PLUGIN
  {
    // get path to \PLUGINS dir

    if (strlen(GetCommandLine()) > 500)
    {
        dumpmsg("ERROR: couldn't get path to winamp [too long]");
        inifile[0] = 0;
    }
    else
    {
        strcpy(inifile, GetCommandLine());
        int i = strlen(inifile)-1;
        while (inifile[i] != '\\' && i>=0)
            i--;

        if (i==0)
        {
            dumpmsg("ERROR: couldn't get path to winamp!");
            inifile[0] = 0;
        }
        else
        {
            inifile[i+1] = 0;
            strcat(inifile, "plugins");
            // get rid of initial " in commandline
            int L=strlen(inifile);
            for (i=0; i<L; i++)     // includes terminating NULL char.
                inifile[i] = inifile[i+1];
        }
    }
  }
#endif
*/

  //---------- if they've never run Geiss before, or if they just upgraded,
  //                                          give them the Settings screen -------------
  LONG res, res2; 
  HKEY skey; 
  DWORD valtype, valsize, val;
  int version = -1;
  valsize=sizeof(val); 
  char buf[128];

  res=RegOpenKeyEx(HKEY_CURRENT_USER,APPREGPATH,0,KEY_ALL_ACCESS,&skey);

  if (res==ERROR_SUCCESS)
  {
    res2=RegQueryValueEx(skey,"Version",0,&valtype,(LPBYTE)&val,&valsize);   
    if (res2==ERROR_SUCCESS) version=val;
  }

  RegCloseKey(skey);

  if (res!=ERROR_SUCCESS)    // give them the disclaimer
  {
      InitCommonControls(); // loads common controls DLL 
      ScrMode = smConfig;
      dumpmsg("showing the disclaimer");
      g_DisclaimerAgreed = false;
      DialogBox( h, MAKEINTRESOURCE(IDD_DISC), hwnd, (DLGPROC)DisclaimerDialogProc ); 
      dumpmsg("done with disclaimer");
      g_bFirstRun = true;
      if (!g_DisclaimerAgreed)
      {
          return 1;
      }
  }

  if (version != CURRENT_GEISS_VERSION)         // bring up new config dialog
  {
      ScrMode = smConfig;
      g_bFirstRun = true;   // so it will auto-pick 640x480x8.
  }

  if (g_bFirstRun)             // if no reg. settings were found
  {
    FXW = g_desktop_w;
    FXH = g_desktop_h;
  }

  // if this guy has problems w/reading the registry, (like FXW or FXH or iDispBits is 
  //  screwy) he'll set scrMode to smConfig.
  dumpmsg("Reading registry");

  if (ScrMode==smConfig) dumpmsg(" mode before ReadConfigRegistry() is config");
  if (ScrMode==smSaver) dumpmsg(" mode before ReadConfigRegistry() is normal");
  //-------------------
  ReadConfigRegistry();
  //-------------------

  #if PLUGIN
      sprintf(buf, "Geiss for Winamp debug file, version %d", CURRENT_GEISS_VERSION);
  #endif
  #if SAVER
      sprintf(buf, "Geiss Screensaver debug file, version %d", CURRENT_GEISS_VERSION);
  #endif
  dumpmsg(buf);

  dumpmsg("Command Line parameters follow:");
  dumpmsg(GetCommandLine());

  if (ScrMode==smConfig) dumpmsg(" mode after ReadConfigRegistry() is config");
  else if (ScrMode==smSaver) dumpmsg(" mode after ReadConfigRegistry() is normal");
  else if (ScrMode==smNone) dumpmsg(" mode after ReadConfigRegistry() is none");
  else if (ScrMode==smPreview) dumpmsg(" mode after ReadConfigRegistry() is preview");
  else if (ScrMode==smPassword) dumpmsg(" mode after ReadConfigRegistry() is password");

  if (ScrMode==smConfig) //DialogBox(hInstance,MAKEINTRESOURCE(DLG_CONFIG),hwnd,ConfigDialogProc);
  {
    /*
    DWORD dwDXVersion;//, dwDXPlatform;
    dumpmsg("GetDXVersion()...");
    GetDXVersion(&dwDXVersion);//, &dwDXPlatform);
    char buf[128];
    sprintf(buf, "DirectX version is %x.", dwDXVersion);
    dumpmsg(buf);

    if (dwDXVersion == 0)
    {
        dumpmsg("---------COULDN'T DETECT DIRECTX VERSION NUMBER--------");
    }
    else if (dwDXVersion < 0x300)
    {
        char bufmsg[512];
        sprintf(bufmsg, "You either don't have Microsoft DirectX installed or your version (%x) is too old (you need at least 3.00).  You can find it at http://www.microsoft.com/directx/download.asp .  Geiss will now attempt to run anyway.", dwDXVersion);

        dumpmsg("WARNING: DirectX version detected is less than 3!");
        dumpmsg(bufmsg);
        MessageBox( NULL, bufmsg, "DIRECTX PROBLEM", MB_OK );
        
        //return 1;
    }
    */

    //make sure the sliderbar, etc. controls are loaded:
    dumpmsg("InitCommonControls()...");
    InitCommonControls(); // loads common control’s DLL 

    dumpmsg("Bringing up Config Dialog...");
    DialogBox( h, MAKEINTRESOURCE(IDD_CONFIG),
        hwnd, (DLGPROC)ConfigDialogProc );

    return 1;
  }
  
  if (ScrMode==smSaver) 
  {
      if (DoSaver(hwnd, h))  // enters main loop for saver; just does the init & returns for the plugin.
          return 0;
      else
          return 1;
  }

  return 0;
}

void EnforceMaxFPS(int max_fps, int bSaveCpu)
{
    if (max_fps <= 0)
        return;

    float fps_lo = (float)max_fps;
    float fps_hi = (float)max_fps;

    if (bSaveCpu)
    {
        // Find the optimal lo/hi bounds for the fps
        // that will result in a maximum difference,
        // in the time for a single frame, of 0.003 seconds -
        // the assumed granularity for Sleep(1) -

        // Using this range of acceptable fps
        // will allow us to do (sloppy) fps limiting
        // using only Sleep(1), and never the
        // second half of it: Sleep(0) in a tight loop,
        // which sucks up the CPU (whereas Sleep(1)
        // leaves it idle).

        // The original equation:
        //   1/(max_fps*t1) = 1/(max*fps/t1) - 0.003
        // where:
        //   t1 > 0
        //   max_fps*t1 is the upper range for fps
        //   max_fps/t1 is the lower range for fps

        float a = 1;
        float b = -0.003f * max_fps;
        float c = -1.0f;
        float det = b*b - 4*a*c;
        if (det>0)
        {
            float t1 = (-b + sqrtf(det)) / (2*a);
            //float t2 = (-b - sqrtf(det)) / (2*a);

            if (t1 > 1.0f)
            {
                fps_lo = max_fps / t1;
                fps_hi = max_fps * t1;
                // verify: now [1.0f/fps_lo - 1.0f/fps_hi] should equal 0.003 seconds.
                // note: allowing tolerance to go beyond these values for
                // fps_lo and fps_hi would gain nothing.
            }
        }
    }

    if (m_high_perf_timer_freq.QuadPart > 0)
    {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);

        if (m_prev_end_of_frame.QuadPart != 0)
        {
            int ticks_to_wait_lo = (int)((float)m_high_perf_timer_freq.QuadPart / (float)fps_hi);
            int ticks_to_wait_hi = (int)((float)m_high_perf_timer_freq.QuadPart / (float)fps_lo);
            int done = 0;
            int loops = 0;
            do
            {
                QueryPerformanceCounter(&t);

                __int64 t2 = t.QuadPart - m_prev_end_of_frame.QuadPart;
                if (t2 > 2147483000)
                    done = 1;
                if (t.QuadPart < m_prev_end_of_frame.QuadPart)    // time wrap
                    done = 1;

                // this is sloppy - if your freq. is high, this can overflow (to a (-) int) in just a few minutes
                // but it's ok, we have protection for that above.
                int ticks_passed = (int)(t.QuadPart - m_prev_end_of_frame.QuadPart);
                if (ticks_passed >= ticks_to_wait_lo)
                    done = 1;

                if (!done)
                {
                    // if > 0.01s left, do Sleep(1), which will actually sleep some
                    //   steady amount of up to 3 ms (depending on the OS),
                    //   and do so in a nice way (cpu meter drops; laptop battery spared).
                    // otherwise, do a few Sleep(0)'s, which just give up the timeslice,
                    //   but don't really save cpu or battery, but do pass a tiny
                    //   amount of time.

                    //if (ticks_left > (int)m_high_perf_timer_freq.QuadPart/500)
                    if (ticks_to_wait_hi - ticks_passed > (int)m_high_perf_timer_freq.QuadPart/100)
                        Sleep(5);
                    else if (ticks_to_wait_hi - ticks_passed > (int)m_high_perf_timer_freq.QuadPart/1000)
                        Sleep(1);
                    else
                        for (int i=0; i<10; i++)
                            Sleep(0);  // causes thread to give up its timeslice
                }
            }
            while (!done);
        }

        m_prev_end_of_frame = t;
    }
    else
    {
        Sleep(1000/max_fps);
    }
}



#if PLUGIN

    // getmodule routine from the main header. Returns NULL if an invalid module was requested,
    // otherwise returns either mod1, mod2 or mod3 depending on 'which'.
    winampVisModule *getModule(int which)
    {
        switch (which)
        {
            case 0: return &mod1;
            //case 1: return &mod2;
            //case 2: return &mod3;
            default:return NULL;
        }
    }

    // initialization. Registers our window class, creates our window, etc. Again, this one works for
    // both modules, but you could make init1() and init2()...
    // returns 0 on success, 1 on failure.


    int init(struct winampVisModule *this_mod)
    {
        g_this_mod = this_mod;
        this_mod_hwndParent = this_mod->hwndParent;
        this_mod_hDllInstance = this_mod->hDllInstance;

        ////::GetWindowRect(this_mod->hwndParent, &g_WinampWindowRect);
        //this_mod->delayMs = ReadDelayFromRegistry();

        ScrMode = smSaver;
        return WinMain(this_mod->hDllInstance, this_mod->hDllInstance, NULL, 0);
       
    }


    // configuration. Passed this_mod, as a "this" parameter. Allows you to make one configuration
    // function that shares code for all your modules (you don't HAVE to use it though, you can make
    // config1(), config2(), etc...)
    void config(struct winampVisModule *this_mod)
    {
        // when they click 'Config', this gets called.  Init does NOT get called.

        g_this_mod = this_mod;
        this_mod_hwndParent = this_mod->hwndParent;  //potato
        this_mod_hDllInstance = this_mod->hDllInstance;

        ScrMode = smConfig;
        WinMain(this_mod->hDllInstance, this_mod->hDllInstance, NULL, 0);

    }

    // cleanup (opposite of init()). Destroys the window, unregisters the window class
    void quit(struct winampVisModule *this_mod)
    {

        finiObjects();

    }
#endif

// render function - Returns 0 if successful, 1 if visualization should end.
// SHARED BY PLUGIN AND SAVER...
int render1(struct winampVisModule *this_mod)
{
    SoundEnabled = true;
    SoundActive = true;
    SoundReady = true;

    #ifdef PLUGIN
      if (g_this_mod != this_mod)
      {
          char buf[128];
          sprintf(buf, "**********ALERT******** Frame %d: value of this_mod changed", intframe);
          dumpmsg(buf);

          g_this_mod = this_mod;
      }

      if (this_mod_hwndParent != this_mod->hwndParent)
      {
          char buf[128];
          sprintf(buf, "**********ALERT******** Frame %d: this_mod->hwndParent changed", intframe);
          dumpmsg(buf);

          this_mod_hwndParent = this_mod->hwndParent;
      }

      if (this_mod_hDllInstance != this_mod->hDllInstance)
      {
          char buf[128];
          sprintf(buf, "**********ALERT******** Frame %d: this_mod->hDllInstance changed", intframe);
          dumpmsg(buf);

          this_mod_hDllInstance = this_mod->hDllInstance;
      }
    #endif

    if (!g_bLost)
    {
            RenderFX();        // updates VS2

            Process_Map(VS1, VS2);//, lpDDSBack);

            GetWaveData();
            RenderDots(VS2);
            RenderWave(VS2);

            //----------- step 2: swap buffers & put VS2 to the back surface
            TEMPPTR = VS1;    VS1 = VS2;    VS2 = TEMPPTR;
        #ifdef PLUGIN
          Check_For_New_Song_Title();
        #endif
            #if (GRFX==1)
                Plop_Songtitle_Using_Backbuffer_As_Scratch(0);
                Merge_All_VS_To_Backbuffer();
                Plop_Songtitle_Using_Backbuffer_As_Scratch(1);
            #endif

            // fps calculations:
            float now = clock();
            if (time_array_ready)
            {
                float mix = (intframe <= 32) ? 0 : 0.95;
                fps = fps*mix + (1-mix) * 30.0/(( now - time_array[time_array_pos]) / (float)CLOCKS_PER_SEC);
            }
      
            time_array[time_array_pos++] = now;
            if (time_array_pos>=30) { time_array_pos = 0; time_array_ready = true; }

            
            //----------- step 3: fps to back surface
            if (!g_bSuppressAllMsg)
            {
                #if (GRFX==1)
                    if (SHOW_TRACK_MSG>0)
                    {
                        SHOW_TRACK_MSG--;
                        Put_Trackmsg_To_Backbuffer();
                    }
                    if (SHOW_LOCKED_MSG>0)
                    {
                        SHOW_LOCKED_MSG--;
                        //Put_Lockedmsg_To_Backbuffer();
                        Put_Msg_To_Backbuffer(szLM);
                    }
                    else if (SHOW_UNLOCKED_MSG>0)
                    {
                        SHOW_UNLOCKED_MSG--;
                        //Put_Unlockedmsg_To_Backbuffer();
                        Put_Msg_To_Backbuffer(szULM);
                    }
                    else if (SHOW_MISC_MSG>0)
                    {
                        SHOW_MISC_MSG--;
                        Put_Msg_To_Backbuffer(szMisc);
                    }
                    else if (SHOW_MODEPREFS_MSG>0 && !g_QuitASAP)
                    {
                        SHOW_MODEPREFS_MSG--;
                        char msg[128];
                        sprintf(msg, " Mode %d: [", mode);
                        for (int i=0; i<modeprefs[mode]; i++)
                            strcat(msg, "*");
                        sprintf(msg + strlen(msg), "] (%d stars) ", modeprefs[mode]);
                        Put_Msg_To_Backbuffer(msg);
                    }
                    else if (SHOW_MOUSECLICK_MSG>0 && !g_bSuppressHelpMsg)
                    {
                        SHOW_MOUSECLICK_MSG--;
                        //Put_Mouseclickmsg_To_Backbuffer();
                        Put_Msg_To_Backbuffer(szMCM);
                    }
                    //else if (SHOW_TITLE==1) // only for plugin
                    //{
                    //    Put_MP3Title_To_Backbuffer();
                    //}
                    else if (SHOW_DEBUG > 0)
                    {
                        //Put_Debugmsg_To_Backbuffer();
                        Put_Msg_To_Backbuffer(szDEBUG);
                    }
                    else if (SHOW_FPS)
                    {
                        Put_FPS_To_Backbuffer();
                    }

                    if (SHOW_HELP_MSG)
                    {
                        Put_Helpmsg_To_Backbuffer();
                    }
                #endif
            }
    }

    //----------- flip buffers
    //clock_t temp_clock = clock();
#if (GRFX==1)
    HRESULT ddrval = lpDDSPrimary->Flip( NULL, DDFLIP_WAIT );
    if (ddrval != DD_OK)  
      g_bLost = !Try_To_Recover();
#endif        
    
    // frame delay (ms)
//if (iRegistryDelay>0) Sleep(iRegistryDelay);
//EnforceMaxFPS(g_nTargetFps, !g_bTargetFpsStrict);
    

    return 0;   //beely
}


BOOL FX_Init()
{
    unsigned long z, x;
    int d;

    dumpmsg("Starting FX_Init()");

    GetWindowsPath(); // sets winpath

    if (RND==1) srand( (unsigned)time( NULL ) );
    chaser_offset = rand() % 40000L;

    if (iDispBits==8)
    {
        VS1 = (unsigned char *)malloc(FXW*FXH + 1024);       if (VS1 == 0) { dumpmsg("Out of memory"); finiObjects(); return 1; }
        VS2 = (unsigned char *)malloc(FXW*FXH + 1024);       if (VS2 == 0) { dumpmsg("Out of memory"); finiObjects(); return 1; }
        memset(VS1, 0, FXW*FXH);
        memset(VS2, 0, FXW*FXH);
    }
    else
    {
        VS1 = (unsigned char *)malloc(FXW*FXH*4 + 1024);     if (VS1 == 0) { dumpmsg("Out of memory"); finiObjects(); return 1; }
        VS2 = (unsigned char *)malloc(FXW*FXH*4 + 1024);     if (VS2 == 0) { dumpmsg("Out of memory"); finiObjects(); return 1; }
        memset(VS1, 0, FXW*FXH*4);
        memset(VS2, 0, FXW*FXH*4);
    }
    DATA_FX = (unsigned char *)malloc(FXW*FXH*8 + 1024); if (DATA_FX == 0) { dumpmsg("Out of memory"); finiObjects(); return 1; }
    DATA_FX2 = (unsigned char *)malloc(FXW*FXH*8 + 1024); if (DATA_FX2 == 0) { dumpmsg("Out of memory"); finiObjects(); return 1; }


    original_VS[0] = VS1;
    original_VS[1] = VS2;
    original_DATA_FX[0] = DATA_FX;
    original_DATA_FX[1] = DATA_FX2;
    if (((unsigned long)VS1) % 16 != 0)
    {
        dumpmsg("align VS1");
        VS1 = (unsigned char *)((((unsigned long)VS1)/16 + 1) * 16);
    }
    if (((unsigned long)VS2) % 16 != 0)
    {
        dumpmsg("align VS2");
        VS2 = (unsigned char *)((((unsigned long)VS2)/16 + 1) * 16);
    }
    if (((unsigned long)DATA_FX) % 16 != 0)
    {
        dumpmsg("align DATA_FX");
        DATA_FX = (unsigned char *)((((unsigned long)DATA_FX)/16 + 1) * 16);
    }
    if (((unsigned long)DATA_FX2) % 16 != 0)
    {
        dumpmsg("align DATA_FX2");
        DATA_FX2 = (unsigned char *)((((unsigned long)DATA_FX2)/16 + 1) * 16);
    }



    memset(g_power,          0, sizeof(float)*FOURIER_DETAIL);
    memset(g_power_smoothed, 0, sizeof(float)*FOURIER_DETAIL);


//#if PLUGIN
//#endif

    //float s;// = 0.1*((rand()%100)*0.01);      // 0.0: regular palette, 0.4: can get wild palettes
    //s = 0.1;

    if (iDispBits==16)
    {
        for (z=0; z<256; z++) REMAP[z] = min(255, z*2) >> 3;
        for (z=0; z<256; z++) REMAP2[z] = min(255, z*2) >> 2;
    }
    else
    {
        for (z=0; z<256; z++) REMAP[z] = min(255, z*2);
    }
    
    for (z=0; z<SCATTERVALS; z++)
    {
        fScatter[z] = 0.05 - 0.025*((rand()%1000)*0.001);
    }

    frames_since_last_plop = 1;

    for (z=0; z<20; z++)
    {
        chaser_x[z] = 1;
        chaser_y[z] = 1;
    }

    for (x=0; x<=20; x++)
    for (z=0; z<=20; z++)
        sqrt_tab[x][z] = (float)sqrtf((x-10)*(x-10) + (z-10)*(z-10));

    for (z=0; z<10; z++)
    {
        micro_c1[z] = 0.08 + 0.09*(rand()%1000)*0.001;
        micro_c2[z] = 0.08 + 0.09*(rand()%1000)*0.001;
        micro_c3[z] = 0.08 + 0.09*(rand()%1000)*0.001;
        micro_f1[z] = 0.1 + 0.05*(rand()%1000)*0.001;
        micro_f2[z] = 0.1 + 0.05*(rand()%1000)*0.001;
        micro_f3[z] = 0.1 + 0.05*(rand()%1000)*0.001;
        micro_f4[z] = 0.1 + 0.05*(rand()%1000)*0.001;
        micro_rad[0][z] = 2.0 + 2.8*(rand()%1000)*0.001;
        micro_rad[1][z] = 2.0 + 2.8*(rand()%1000)*0.001;
        micro_rad[2][z] = 2.0 + 2.8*(rand()%1000)*0.001;
        micro_rad[3][z] = 2.0 + 2.8*(rand()%1000)*0.001;
    }

    for (z=0; z<6; z++) gF[z] = ((rand()%1000)*0.001)*0.01 + 0.02;

    for (z=0; z<2345; z++) rand_array[z] = (rand() % 100)*0.0005;  // for mode #7

    /*
    for (z=0; z<256; z++) char_to_mode[z] = 5;

    //g_font.load_font("c:\\courier_8bit.tga");

    if (NUM_MODES >= 1) char_to_mode['1'] = 1;
    if (NUM_MODES >= 2) char_to_mode['2'] = 2;
    if (NUM_MODES >= 3) char_to_mode['3'] = 3;
    if (NUM_MODES >= 4) char_to_mode['4'] = 4;
    if (NUM_MODES >= 5) char_to_mode['5'] = 5;
    if (NUM_MODES >= 6) char_to_mode['6'] = 6;
    if (NUM_MODES >= 7) char_to_mode['7'] = 7;
    if (NUM_MODES >= 8) char_to_mode['8'] = 8;
    if (NUM_MODES >= 9) char_to_mode['9'] = 9;
    if (NUM_MODES >= 10) char_to_mode['0'] = 10;
    if (NUM_MODES >= 11) char_to_mode['!'] = 11;
    if (NUM_MODES >= 12) char_to_mode['@'] = 12;
    if (NUM_MODES >= 13) char_to_mode['#'] = 13;
    if (NUM_MODES >= 14) char_to_mode['$'] = 14;
    if (NUM_MODES >= 15) char_to_mode['%'] = 15;
    if (NUM_MODES >= 16) char_to_mode['^'] = 16;
    if (NUM_MODES >= 17) char_to_mode['&'] = 17;
    if (NUM_MODES >= 18) char_to_mode['*'] = 18;
    if (NUM_MODES >= 19) char_to_mode['('] = 19;
    if (NUM_MODES >= 20) char_to_mode[')'] = 20;
    */
    
    z = 1;
    modeInfo[z].effect_freq[CHASERS] = 220;
    modeInfo[z].effect_freq[BAR    ] = 150;
    modeInfo[z].effect_freq[DOTS   ] =  10;
    modeInfo[z].effect_freq[SOLAR  ] = 680;
    modeInfo[z].effect_freq[GRID   ] =   4;
    modeInfo[z].effect_freq[NUCLIDE] = 170;
    modeInfo[z].effect_freq[SHADE  ] = 400;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = (iDispBits==8) ? 400 : 800;
    modeInfo[z].center_dwindle = 1.0;

    z = 2;
    modeInfo[z].effect_freq[CHASERS] = 750;
    modeInfo[z].effect_freq[BAR    ] = 500;
    modeInfo[z].effect_freq[DOTS   ] = 750;
    modeInfo[z].effect_freq[SOLAR  ] = 750;
    modeInfo[z].effect_freq[GRID   ] = 0;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] = 0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 35;
    modeInfo[z].center_dwindle = 1.0;
    modeInfo[z].max_effects = 5;

    z = 3;
    modeInfo[z].effect_freq[CHASERS] = 100;
    modeInfo[z].effect_freq[BAR    ] = 100;
    modeInfo[z].effect_freq[DOTS   ] = 100;
    modeInfo[z].effect_freq[SOLAR  ] = 500;
    modeInfo[z].effect_freq[GRID   ] = 10;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] = 300;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 60;
    modeInfo[z].center_dwindle = 0.99;

    z = 4;
    modeInfo[z].effect_freq[CHASERS] = 500;
    modeInfo[z].effect_freq[BAR    ] = 100;
    modeInfo[z].effect_freq[DOTS   ] = 100;
    modeInfo[z].effect_freq[SOLAR  ] = 100;
    modeInfo[z].effect_freq[GRID   ] = 30;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] = 0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 34;
    modeInfo[z].center_dwindle = 0.98;

    z = 5;
    modeInfo[z].effect_freq[CHASERS] = 100;
    modeInfo[z].effect_freq[BAR    ] = 350;
    modeInfo[z].effect_freq[DOTS   ] = 100;
    modeInfo[z].effect_freq[SOLAR  ] = 500;
    modeInfo[z].effect_freq[GRID   ] =  15;
    modeInfo[z].effect_freq[NUCLIDE] = 180;
    modeInfo[z].effect_freq[SHADE  ] = 500;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 60;
    modeInfo[z].center_dwindle = 0.99;

    z = 6;
    modeInfo[z].effect_freq[CHASERS] = 400;
    modeInfo[z].effect_freq[BAR    ] = 120;
    modeInfo[z].effect_freq[DOTS   ] = 200;
    modeInfo[z].effect_freq[SOLAR  ] =   0;
    modeInfo[z].effect_freq[GRID   ] =   0;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] =   0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 60;
    modeInfo[z].center_dwindle = 1.000;
    //modeInfo[z].min_effects = 0;   // we allow 0 effects because we still have the t/b edge effects.

    z = 7;
    modeInfo[z].effect_freq[CHASERS] =  50;
    modeInfo[z].effect_freq[BAR    ] = 200;
    modeInfo[z].effect_freq[DOTS   ] =   0;
    modeInfo[z].effect_freq[SOLAR  ] = 300;
    modeInfo[z].effect_freq[GRID   ] =   0;
    modeInfo[z].effect_freq[NUCLIDE] = 600;
    modeInfo[z].effect_freq[SHADE  ] = 350;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 65;
    modeInfo[z].center_dwindle = 0.985;

    z = 8;
    modeInfo[z].effect_freq[CHASERS] = 150;
    modeInfo[z].effect_freq[BAR    ] = 150;
    modeInfo[z].effect_freq[DOTS   ] = 150;
    modeInfo[z].effect_freq[SOLAR  ] = 150;
    modeInfo[z].effect_freq[GRID   ] =  25;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] = 
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 60;
    modeInfo[z].center_dwindle = 0.96;

    z = 9;
    modeInfo[z].effect_freq[CHASERS] = 450;
    modeInfo[z].effect_freq[BAR    ] = 200;
    modeInfo[z].effect_freq[DOTS   ] =  50;
    modeInfo[z].effect_freq[SOLAR  ] = 200;
    modeInfo[z].effect_freq[GRID   ] =   0;
    modeInfo[z].effect_freq[NUCLIDE] = 100;
    modeInfo[z].effect_freq[SHADE  ] = 200;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 50;
    modeInfo[z].center_dwindle = 0.985;

    z = 10;
    modeInfo[z].effect_freq[CHASERS] = 150;
    modeInfo[z].effect_freq[BAR    ] =  20;
    modeInfo[z].effect_freq[DOTS   ] =  80;
    modeInfo[z].effect_freq[SOLAR  ] =   0;
    modeInfo[z].effect_freq[GRID   ] =   0;
    modeInfo[z].effect_freq[NUCLIDE] =  80;
    modeInfo[z].effect_freq[SHADE  ] =   0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 0;
    modeInfo[z].center_dwindle = 1.0;   // no dwindle
    modeInfo[z].min_effects = 0;   
    modeInfo[z].max_effects = 2;   

    z = 11;
    modeInfo[z].effect_freq[CHASERS] = 360;
    modeInfo[z].effect_freq[BAR    ] = 200;
    modeInfo[z].effect_freq[DOTS   ] = 230;
    modeInfo[z].effect_freq[SOLAR  ] = 550;
    modeInfo[z].effect_freq[GRID   ] =  10;
    modeInfo[z].effect_freq[NUCLIDE] = 330;
    modeInfo[z].effect_freq[SHADE  ] = 150;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 750;
    modeInfo[z].center_dwindle = 1.0;   // no dwindle
    modeInfo[z].min_effects = 0;   
    modeInfo[z].max_effects = 4;   

    z = 12;  // sideways splitter
    modeInfo[z].effect_freq[CHASERS] = 360;
    modeInfo[z].effect_freq[BAR    ] = 200;
    modeInfo[z].effect_freq[DOTS   ] = 230;
    modeInfo[z].effect_freq[SOLAR  ] =   0;
    modeInfo[z].effect_freq[GRID   ] =   0;
    modeInfo[z].effect_freq[NUCLIDE] = 330;
    modeInfo[z].effect_freq[SHADE  ] =   0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 500;
    modeInfo[z].center_dwindle = 0.915;   // no dwindle
    modeInfo[z].min_effects = 0;   
    modeInfo[z].max_effects = 2;   

    z = 13;
    modeInfo[z].effect_freq[CHASERS] = 500;
    modeInfo[z].effect_freq[BAR    ] =   0;
    modeInfo[z].effect_freq[DOTS   ] = 100;
    modeInfo[z].effect_freq[SOLAR  ] =   0;
    modeInfo[z].effect_freq[GRID   ] = 30;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] = 0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 34;
    modeInfo[z].center_dwindle = 0.98;

    z = 14;
    modeInfo[z].effect_freq[CHASERS] = 500;
    modeInfo[z].effect_freq[BAR    ] =   0;
    modeInfo[z].effect_freq[DOTS   ] = 100;
    modeInfo[z].effect_freq[SOLAR  ] =   0;
    modeInfo[z].effect_freq[GRID   ] = 30;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] = 0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 34;
    modeInfo[z].center_dwindle = 0.98;

    z = 15;
    modeInfo[z].effect_freq[CHASERS] =   0;
    modeInfo[z].effect_freq[BAR    ] =   0;
    modeInfo[z].effect_freq[DOTS   ] =   0;
    modeInfo[z].effect_freq[SOLAR  ] =   0;
    modeInfo[z].effect_freq[GRID   ] =   0;
    modeInfo[z].effect_freq[NUCLIDE] = 200;
    modeInfo[z].effect_freq[SHADE  ] =   0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].center_dwindle = 1.00;
    modeInfo[z].min_effects = 0;
    modeInfo[z].max_effects = 1;

    z = 16;
    modeInfo[z].effect_freq[CHASERS] = 500;
    modeInfo[z].effect_freq[BAR    ] = 100;
    modeInfo[z].effect_freq[DOTS   ] = 100;
    modeInfo[z].effect_freq[SOLAR  ] = 100;
    modeInfo[z].effect_freq[GRID   ] = 30;
    modeInfo[z].effect_freq[NUCLIDE] =   0;
    modeInfo[z].effect_freq[SHADE  ] =   0;
    modeInfo[z].effect_freq[SPECTRAL]=   0;
    modeInfo[z].solar_max = 34;
    modeInfo[z].center_dwindle = 0.98;

    for (z=17; z<=NUM_MODES; z++)
    {
        modeInfo[z].effect_freq[CHASERS] = 150;
        modeInfo[z].effect_freq[BAR    ] = 150;
        modeInfo[z].effect_freq[DOTS   ] = 150;
        modeInfo[z].effect_freq[SOLAR  ] = 150;
        modeInfo[z].effect_freq[GRID   ] =  12;
        modeInfo[z].effect_freq[NUCLIDE] =   0;
        modeInfo[z].effect_freq[SHADE  ] =  50;
        modeInfo[z].effect_freq[SPECTRAL]=   0;
        modeInfo[z].solar_max = 600;
        modeInfo[z].min_effects = 1;
        modeInfo[z].max_effects = 3;
        modeInfo[z].center_dwindle = 1.000;
    }

    for (z=1; z<=NUM_MODES; z++) {
      if (iDispBits > 8) {
          modeInfo[z].effect_freq[NUCLIDE] = max(0, min(900, modeInfo[z].effect_freq[NUCLIDE]*1.3f));
          modeInfo[z].effect_freq[CHASERS] = max(0, min(900, modeInfo[z].effect_freq[CHASERS]- 50));
          modeInfo[z].effect_freq[DOTS]    = max(0, min(900, modeInfo[z].effect_freq[DOTS   ]+220));
          modeInfo[z].effect_freq[BAR ]    = max(0, min(900, modeInfo[z].effect_freq[BAR    ]+220));
          modeInfo[z].effect_freq[SHADE]   = max(0, min(900, modeInfo[z].effect_freq[SHADE  ]+150));
          //modeInfo[z].max_effects          += 1;
      }
        modeInfo[z].effect_freq[GRID]    = min(1000, modeInfo[z].effect_freq[GRID]+8);
    }

    // custom settings for modes 16+ :
    modeInfo[20].center_dwindle = 0.98;
    modeInfo[21].center_dwindle = 0.98; // fragmented (integer move)
    modeInfo[22].center_dwindle = 0.98; // sonic rings #1
    modeInfo[23].center_dwindle = 0.98; // sonic rings #2
    


    
    dumpmsg("  Calling initial FX_Pick_Random_Mode() and FX_Apply_Mode() loop...");

    //this is hanging:
    FX_Pick_Random_Mode();   // sets global 'mode' to a valid random number.
    g_rush_map = true;
    y_map_pos = -1;
    do
    {
        FX_Apply_Mode();         // sets up DATA_X and other misc. stuff based on 'mode'
    }
    while (y_map_pos != -1);

    dumpmsg("Finished with FX_Init().");

    return true;
}

void FX_Fini()
{
    if (original_VS[0] != NULL)      { delete original_VS[0];      original_VS[0] = NULL;      VS1 = NULL; }
    if (original_VS[1] != NULL)      { delete original_VS[1];      original_VS[1] = NULL;      VS2 = NULL; }
    if (original_DATA_FX[0] != NULL) { delete original_DATA_FX[0]; original_DATA_FX[0] = NULL; DATA_FX = NULL; }
    if (original_DATA_FX[1] != NULL) { delete original_DATA_FX[1]; original_DATA_FX[1] = NULL; DATA_FX2 = NULL; }

#if PLUGIN
    if (g_title_font) { DeleteObject(g_title_font); g_title_font = NULL; }
#endif

}

void FX_Pick_Random_Mode()
{
    if (modeprefs_total <= 0)
    {
        new_mode = 1 + rand()%NUM_MODES;       // modes start at 1.
        if (rand()%25==0) new_mode = 7;
        if (rand()%25==0) new_mode = 5;

        y_map_pos = -1;    // start new map generating
    }
    else
    {
        int a = rand() % modeprefs_total;
        int b = 0;
        int i = 1;
        while (i<=NUM_MODES)
        {
            b += modeprefs[i];
            if (a<b) 
            {
                new_mode = i;
                i = 9999;
                y_map_pos = -1;    // start new map generating
            }
            i++;
        }
    }
}







void FX_Apply_Mode(bool bLoadPreset, int iPresetNum)         // sets up DATA_X and other misc. stuff based on 'mode'
{
    // to start a new map background-generating, call this function
    // with y_map_pos = FX_YCUT.

    if (y_map_pos == -1)   // finished a map on last frame...
    {
        y_map_pos = FX_YCUT*FXW;    // so start a new one on this frame.
    }

    if (y_map_pos < FX_YCUT*FXW || y_map_pos > (FXH-FX_YCUT)*FXW)
    {
        dumpmsg("FATAL ERROR: FX_Apply_Mode(): received y_map_pos that was out of range");
        exit(99);
    }


    //if (g_rush_map) 
    //{
    //    temp_clock = clock();
    //}


    unsigned long a, b, addr, z1, z2;
    long k, k1, k2;
    float newx, newy, newx2, newy2;
    long x, y, n;
    unsigned long fx, z, A_number, D_offset;
    float tx, ty, d, e, f, r;
    int A_offset, R_offset_rel;//R_offset, old_R_offset are global;
    float inv_FXW = 2.0/FXW;
    //float inv_FXH = 2.0/FXH;
    float half_FXW = 0.5*FXW;
    //float half_FXH = 0.5*FXH;
    int bytewidth = (iDispBits==8) ? 1 : 4;

//    new_mode = 3;

    //float MAX_R_INV = 1.0/sqrtf(FXW*FXW+FXH*FXH);
    //int flag=0;   // big big

    if (y_map_pos == FX_YCUT*FXW)
    {
        g_bLoadPreset = bLoadPreset;
        g_iPresetNum = iPresetNum;

        if (!bLoadPreset)
        {
            new_gXC = FXW/2-1 + rand()%60 - 30;
            new_gYC = FXH/2-1 + rand()%30 - 15;

            weightsum = 256;
            
            new_damping = suggested_damping;
            if (new_damping < 0.50) new_damping = 0.50;
            if (new_damping > 1.00) new_damping = 1.00;
            if (mode_motion_dampened[new_mode]) new_damping *= 0.5;

            do
            {
                new_waveform = (rand() % (NUM_WAVES*3-1))/3 + 1;    // de-emphasizes last wave
                //waveform = (rand() % NUM_WAVES) + 1;        // all waves equal
            }
            while ((new_mode==6 && new_waveform==5) ||
                   (new_mode==12 && (new_waveform==4 || new_waveform==6)) || 
                   (new_mode==14 && (new_waveform==3 || new_waveform==4)) ||
                   ((new_mode==8 || new_mode==23 || new_mode==24) && new_waveform==6) 
                   );
        }
        fBigBeatThreshold = 1.10;
        //new_bScrollMap = false;


        frames_this_mode = 0;
        frames_crunching_this_mode = 0;

        if (new_mode==1) {                    // ***
            //scale1 = 0.97 + 0.015*((rand()%1000)*0.001);
            scale1 = 0.985 - 0.12*powf((rand()%1000)*0.001, 2);
            scale2 = scale1;
            turn1 = 0.01 + 0.01*((rand()%1000)*0.001);
            turn2 = turn1;

            //if (rand()%3==1) turn1 *= -1;
            if (scale1 > 0.97 && rand()%3==1) turn1 *= -1;

        } else if (new_mode==2) {                    // ****
            scale1 = 1.00 + 0.02*((rand()%1000)*0.001);
            turn1 =  0.02 + 0.07*((rand()%1000)*0.001);
        } else if (new_mode==3) {    // terra-landing new_mode
            scale1 = 0.85 + 0.1*((rand()%1000)*0.001); 
            scale2 = scale1;                           
            turn1 = 0.01 + 0.015*((rand()%1000)*0.001);
            turn2 = turn1;
        } else if (new_mode==4) {
            //scales will be handled by the map generator - change w/dist from center
            turn1 = 0.007 + 0.02*((rand()%1000)*0.001);   // normal
            turn2 = turn1;//0.007 + 0.02*((rand()%1000)*0.001);
        } else if (new_mode==5) {    // *****   SUPER-perspective
            //scales will be handled by the map generator - change w/dist from center
            turn1 = 0.01 + 0.03*((rand()%1000)*0.001);
            turn2 = turn1;//0.01 + 0.01*((rand()%1000)*0.001);
        } else if (new_mode==6) {        // VORTEX THINGY
            f = 0;
            int iNumPushTypes = 3;//((rand()%8)==0) ? 1 : 3;

            for (x=0; x<10; x++)
            {
                cx[x] = (rand() % (FXW*10))*0.1;
                cy[x] = FX_YCUT + (rand() % ((FXH-FX_YCUT*2)*10)  )*0.1;
                d = (rand()%628)*0.01;
                f = 1 + (rand()%80)*0.01;
                ci[x] = cosf(d)*f;
                cj[x] = sinf(d)*f;
                ctype[x] = rand()%iNumPushTypes;
                cr[x] = 80 + (rand() % 1200)*0.1;  // [80:200]
                cr_inv[x] = 1.0/cr[x];
            }

            //new_bScrollMap = rand() % 2;

            //frames_til_auto_switch *= 0.5;

        } else if (new_mode==7) {    // fuzzy
            //scales will be handled by the map generator
            turn1 = 0.01 + 0.01*((rand()%1000)*0.001);
            turn2 = turn1;//0.01 + 0.01*((rand()%1000)*0.001);
        } else if (new_mode==8) {  // ripples
            //scales will be handled by the map generator
            turn1 =  0.05*(0.001*(rand()%1000));
            turn2 = turn1;// 0.05*(0.001*(rand()%1000));    
        } else if (new_mode==9) {    // crazy-ass feedback (flower petals in 320)
            scale1 = 0.8 + 0.25*((rand()%1000)*0.001);
            scale1 = (scale1 - 1)*protective_factor + 1;
            scale2 = scale1;
            turn1 = 0.01 + 0.03*((rand()%1000)*0.001);
            turn2 = turn1;
        } else if (new_mode==10) {
            
            // no init

        } else if (new_mode==11) {
            scale1 = 1.008 + 0.008*((rand()%1000)*0.001);
            scale2 = scale1;
            turn1 = 0.12 + 0.06*((rand()%1000)*0.001);
            turn2 = turn1;

            //if (rand()%2==1) { 
                turn1 *= -0.6; 
                turn2 *= 0.1; 
                scale1 *= 0.99; 
                scale2 *= 1.01; 
            //}
        }
        else if (new_mode==12) {
            // horizontal streaking, out from centerline
            weightsum = weightsum*0.98;
        } 
        else if (new_mode==13) {
            //black hole - in @ center, out @ edges
            //scales will be handled by the map generator - change w/dist from center
            turn1 = 0.007 + 0.02*((rand()%1000)*0.001);
            turn2 = turn1;//0.007 + 0.02*((rand()%1000)*0.001);
        }
        else if (new_mode==14) {
            turn1 = 0.007 + 0.02*((rand()%1000)*0.001);   // normal
            turn2 = turn1;//0.007 + 0.02*((rand()%1000)*0.001);
        }
        else if (new_mode==15) {
            turn1 = 0.04*((rand()%1000)*0.001) + 
                    0.045*((rand()%1000)*0.001);
            turn2 = turn1;//0.01 + 0.01*((rand()%1000)*0.001);
            //weightsum *= 0.95;
        }
        else if (new_mode==16) {
            // Crystal ball
            turn1 = 0.007 + 0.02*((rand()%1000)*0.001);   // normal
            turn2 = turn1;//0.007 + 0.02*((rand()%1000)*0.001);
        }
        else if (new_mode==16) {
            weightsum = weightsum*(248.0/253.0); //240;
        }
        else if (new_mode >= 17)
        {
            turn1 = 0.007 + 0.02*((rand()%1000)*0.001);   // normal
            turn2 = turn1;//0.007 + 0.02*((rand()%1000)*0.001);
        }


        // randomly switch rotation direction
        if (rand()%2==1) { turn1 *= -1; turn2 *= -1; }


        f1 = 0.92 + 0.05*((rand()%1000)*0.001);
        f2 = 0.0009 + 0.0012*((rand()%1000)*0.001);

        if (new_mode==5)
        {
            //f1 = 0.98 + 0.018*((rand()%1000)*0.001);
            //f2 = 0.7 + 0.6 * ((rand()%1000)*0.001);
            //f1 = 0.92 + 0.01*((rand()%1000)*0.001);
            //f2 = 0.0009 + 0.0012*((rand()%1000)*0.001);

            //f1 = 0.05 + 0.05*((rand()%1000)*0.001) + 0.05*((rand()%1000)*0.001);
            //f2 = 0.99 - 0.01*((rand()%1000)*0.001) - 0.01*((rand()%1000)*0.001);
            f1 = 0.05 + 0.05*((rand()%1000)*0.001) + 0.07*((rand()%1000)*0.001);
            f2 = 0.99 - 0.01*((rand()%1000)*0.001) - 0.02*((rand()%1000)*0.001);
        }
        else if (new_mode==7)
        {
            //f1 = 0.96 + 0.01*((rand()%1000)*0.001);  // overall outward push (move) (lower = faster)
            //f2 = 0.03 + 0.025*((rand()%1000)*0.001);
            f1 = 0.92 + 0.01*((rand()%1000)*0.001);
            f2 = 0.0006 + 0.0005*((rand()%1000)*0.001);
        }
        else if (new_mode==8)
        {
               f1 = (0.001*(rand()%1000));
            f1 *= f1;
            f1 *= f1;
            f1 *= 8;        //16
            f1 += 1.5;   //1.5
        }
        else if (new_mode==9)
        {
            f1 = 0.98 + 0.01*((rand()%1000)*0.001);
            f2 = 0.0009 + 0.0012*((rand()%1000)*0.001);
        }
        else if (new_mode==13)
        {
            // linear scaling factor of final 'scale1' value from central value of 1..
            f1 = 0.92 + 0.16*((rand()%1000)*0.001);
        }
        else if (new_mode==15)
        {
            //f1 = -16 + (rand()%2)*(1024 + 32);
            /*
            f1 = 0.02 + 0.05*((rand()%1000)*0.001) + 0.05*((rand()%1000)*0.001);
            f2 = 0.02 + 0.05*((rand()%1000)*0.001) + 0.05*((rand()%1000)*0.001);
            f3 = 0.03 + 0.07*((rand()%1000)*0.001) + 0.07*((rand()%1000)*0.001);
            f4 = 0.03 + 0.07*((rand()%1000)*0.001) + 0.07*((rand()%1000)*0.001);
            */
            f1 = rand()%5 + 2;                // number of petals
            f2 = 0.92 + 0.06*((rand()%1000)*0.001);   // scale, lower bound
            f3 = 0.05 + 0.05*((rand()%1000)*0.001);   // scale, range
        }
        else if (new_mode==17)
        {
            f1 = 0.01 + 0.09*((rand()%1000)*0.001);
            f2 = 0.01 + 0.09*((rand()%1000)*0.001);
            f3 = 0.01 + 0.09*((rand()%1000)*0.001);
            f4 = 0.01 + 0.09*((rand()%1000)*0.001);
        }

        turn1 *= 0.6;
        turn2 *= 0.6;

        R_offset = 0;

        max_rad_inv = 1.0/sqrtf(FXW*FXW+FXH*FXH);
        rmult = 640.0/(float)FXW;
        rdiv = 1.0/rmult;
        protective_factor = (FXW > 640) ? 640.0/(float)FXW : 1;

        if (bLoadPreset)
        {
            char section[64];
            sprintf(section, "PRESET %d", iPresetNum);
            char temp[64];
            GetPrivateProfileString(section,"t1","1",temp,64,inifile); sscanf(temp, "%f", &turn1);
            GetPrivateProfileString(section,"t2","1",temp,64,inifile); sscanf(temp, "%f", &turn2);
            GetPrivateProfileString(section,"s1","1",temp,64,inifile); sscanf(temp, "%f", &scale1);
            GetPrivateProfileString(section,"s2","1",temp,64,inifile); sscanf(temp, "%f", &scale2);
            GetPrivateProfileString(section,"f1","1",temp,64,inifile); sscanf(temp, "%f", &f1);
            GetPrivateProfileString(section,"f2","1",temp,64,inifile); sscanf(temp, "%f", &f2);
            GetPrivateProfileString(section,"f3","1",temp,64,inifile); sscanf(temp, "%f", &f3);
            GetPrivateProfileString(section,"f4","1",temp,64,inifile); sscanf(temp, "%f", &f4);
        }

        cos_turn1 = cosf(turn1);
        sin_turn1 = sinf(turn1);
        cos_turn2 = cosf(turn2);
        sin_turn2 = sinf(turn2);

        if (FXW*FXH <= 320*240)
            weightsum_res_adjusted = weightsum*250/256;
        else if (FXW*FXH <= 400*300)
            weightsum_res_adjusted = weightsum*251/256;
        else if (FXW*FXH <= 512*384)
            weightsum_res_adjusted = weightsum*252/256;
        else if (FXW*FXH <= 800*600)
            weightsum_res_adjusted = weightsum*253/256;
        else if (FXW*FXH <= 1280*960)
            weightsum_res_adjusted = weightsum*254/256;
        else //if (FXW*FXH <= 1440*900)
            weightsum_res_adjusted = weightsum*255/256;
        //else
      //    weightsum_res_adjusted = weightsum*256/256;  //BAD - DON'T USE 256 - IT LANDS ON ZERO!
    

        frames_til_auto_switch = frames_til_auto_switch__registry;
        if (fps >= 10.0f && fps < 120.0f)
            frames_til_auto_switch = frames_til_auto_switch__registry * fps/30.0f;

        if (bLoadPreset) dumpmsg("preset loaded... applying");
    }
    
    


    // generate a portion of new map
    if (y_map_pos < (FXH-FX_YCUT)*FXW)
    {
        // for mode 6 stuff
        float i,j,z;  
        int h;
        float xxyy;
        int lo = 1;//(new_bScrollMap) ? 0 : 1;
        int hi = 3;//(new_bScrollMap) ? 2 : 3;

        frames_crunching_this_mode++;

        float new_damping_temp = new_damping;
        // when FPS is high, slow down the motion to match:
        if (fps_at_last_mode_switch >= 10.0f && fps_at_last_mode_switch <= 120.0f)
            new_damping_temp *= 30.0f / fps_at_last_mode_switch;
        // when screen is large, in modes 5 and 7, speed up the motion a bit:
        //if (FXW > 640 && (new_mode==5 || new_mode==7))
        //    new_damping_temp *= powf(FXW/640.0f, 0.23f);
        //else 
        //if (FXW > 800) // slow down the other modes, @ higher resolutions.
        //    new_damping_temp *= powf(800.0f/FXW, 0.25f);

        //if (frames_crunching_this_mode > 305) exit(15);

        // note: 'frames_til_auto_switch' has been fudged, if we aren't running at 30fps, based on 'frames_til_auto_switch__registry'.
        int end_pos = FXW*(FX_YCUT + ((float)frames_crunching_this_mode/(float)frames_til_auto_switch)*(float)(FXH-FX_YCUT*2));
        if (g_rush_map || end_pos > FXW*(FXH-FX_YCUT))
        {
            end_pos = FXW*(FXH-FX_YCUT);
        }

        //char buf[64];
        //sprintf(buf, "endpos=%d", end_pos);
        //dumpmsg(buf);

        y = y_map_pos / FXW;
        x = y_map_pos % FXW;

        for ( ; y_map_pos < end_pos; y_map_pos++)
        { 

            A_offset = ((long)y*FXW + (long)x) * 8;
            old_R_offset = R_offset;

            //R_offset to be determined            // dest. read pixel offset: 0..63999, 0 for "null"
            //A_offset                            // for the array:           0..FXW*240-1

            newx2 = (x - new_gXC);   
            newy2 = (y - new_gYC);   

            //---------------------------------------------------------------------------------------------------
        
            if (new_mode <= 16)
            {
                half_FXW = 1.0;   // DANGER!

                if (new_mode==3)
                {
                    scale1 = 0.95 - (newy2*(480.0/FXH))*0.0005;
                    //scale1 = 0.93 - newy2*0.001;
                    //scale2 = scale1;
                }
                else if (new_mode==4)
                {
                    // normal mode 4 sphere:
                    r = sqrtf(newx2*newx2 + newy2*newy2);
                    r *= rmult;
                    scale1 = 0.9 + r*0.0025*0.14;//scale1 = 0.9 + r*0.0025*0.14;

                    // the crystal ball effect:
                    //r = sqrtf(newx2*newx2 + newy2*newy2);
                    //r *= rmult;
                    //scale1 = 1.05 - r*r*0.00025*0.07;//scale1 = 0.9 + r*0.0025*0.14;

                    // the AWESOME up-down split-world warp:
                    //turn1 = 0.3*sinf(newx2*12.0f/(float)FXW);
                    //scale1 = 0.9 + 0.2*cosf(newy2*12.0f/(float)(FXH + (rand() & 1023)/1024.0f));

                    //scale2 = scale1;
                }
                else if (new_mode==5)
                {
                    //r = sqrtf(sqrtf(newx2*newx2 + newy2*newy2))*0.19;
                    //scale1 = f2 - f1*(  r  );      // was 0.98 - 0.1*(  r  ) originally
                    //scale1 = (scale1 - 1)*protective_factor + 1;
                
                

                
                    r = sqrtf(newx2*newx2 + newy2*newy2);
                    r *= (1.0/200.0);
                    r *= rmult;
                    if (effect[NUCLIDE] == -1) 
                        r = sqrtf(r); 
                    else 
                        r *= 1.7;
                    scale1 = f2 - f1*(  r  );      // was 0.98 - 0.1*(  r  ) originally
                    scale1 = (scale1 - 1)*protective_factor + 1;
                    //scale2 = scale1;
                
                    /*
                    // f2 = 1.0 avg;   in [0.7 ... 1.3]
                    r = sqrtf(newx2*newx2 + newy2*newy2)*f2;
                    r *= MAX_R_INV;
                      // now r is from 0..1
                    r = sqrtf(r);
                    scale1 = f1 - r*0.1;
                    scale2 = scale1;
                    */
                }
                else if (new_mode==7)
                {
                    r = sqrtf(newx2*newx2 + newy2*newy2)*f2;
                    r *= rmult;
                    scale1 = (f1 - r);
                    scale1 = (scale1 - 1)*protective_factor + 1;
                    //scale2 = scale1;
                    scale1 += rand_array[rand_array_pos++]; //(rand() % 100)*0.0005;
                    if (rand_array_pos >= 2345) rand_array_pos = 0;

                    //scale2 += (rand() % 100)*0.0005;

                    /*
                    r = sqrtf(newx2*newx2 + newy2*newy2);
                    r *= MAX_R_INV;
                      // now r is from 0..1
                    scale1 = f1 - r*f2;
                    scale2 = scale1;
                    scale1 -= (rand() % 100)*0.0004;
                    scale2 -= (rand() % 100)*0.0004;
                    */
                }            
                else if (new_mode==8)
                {
                    r = sqrtf(newx2*newx2 + newy2*newy2);
                    r *= rmult;
                    scale1 = 0.85 + 0.1*sinf(sqrtf(r)*f1);
                    //scale2 = scale1;
                }
                else if (new_mode==9)
                {
                    r = sqrtf(newx2*newx2 + newy2*newy2)*f2;
                    r *= rmult;
                    scale1 = f1 - r;
                    scale1 = (scale1 - 1)*protective_factor + 1;
                    //scale2 = scale1;

                    // get angle into r
                    //r = atan2(newy2, newx2);
                    //scale1 *= 1.0 + f3*sinf(r*p1);  // p1: number of petals
                    //scale2 = scale1;
                }
                else if (new_mode==13)
                {
                    //r = sqrtf(newx2*newx2 + newy2*newy2);
                    //r *= rmult;
                    //scale1 = 1.1 - r*0.0025*0.14;

                    // AWESOME continuous black hole
                    // sucks in @ center, pushes out @ edges, SUPER SMOOTH!!!!!!!
                    r = sqrtf(newx2*newx2 + newy2*newy2);
                    r *= rmult;
                    scale1 = 1.04 - r*sqrtf(r)*0.00025*0.14;//scale1 = 0.9 + r*0.0025*0.14;
                    scale1 = (scale1 - 1)*f1 + 1;
                }
                else if (new_mode==14)
                {
                    // the AWESOME up-down split-world warp:
                    //turn1 = 0.3*sinf(newx2*12.0f/(float)FXW);
                    //cos_turn1 = cosf(turn1);
                    //sin_turn1 = sinf(turn1);
                    scale1 = 0.9 + 0.2*cosf(newy2*12.0f/(float)(FXH + (rand() & 1023)/1024.0f));
                }
                else if (new_mode==15)
                {
                    scale1 = f2 + f3*sinf(  atan2(newy2, newx2)*f1  );
                }
                else if (new_mode==16)
                {
                    // the crystal ball effect:
                    // UNDER CONSTRUCTION
                    r = sqrtf(newx2*newx2 + newy2*newy2);
                    r *= rmult;///640.0f;

                    //scale1 = 1.05 - r*r*0.00025*0.07;//scale1 = 0.9 + r*0.0025*0.14;
                    scale1 = 1.05 - r*r*0.00025*0.09;//scale1 = 0.9 + r*0.0025*0.14;
                    //if (scale1 < -1.05) scale1 = -1.05;
                    if (scale1 < -1.5) scale1 = -1.5;


                    //scale1 = 1 + (scale1 - 1) * fabs(1-newy2*2.0f/(float)FXH);   // 0..1 - 0 @ center, 1 @ edge
                }
            }
            else // from 17 and up
            {
                //newx2 = (x - new_gXC)*inv_FXW;   // from -1..1, ROUGHLY (can be outside)
                //newy2 = (y - new_gYC)*inv_FXW;   // from -1..1, ROUGHLY (can be outside)
                newx2 *= inv_FXW;   // from -1..1, ROUGHLY (can be outside)
                newy2 *= inv_FXW;   // from -1..1, ROUGHLY (can be outside)

                if (new_mode==17)
                {
                    // mode 1: horz. tunnel
                    scale1 = 0.97 - newy2*newy2*0.40;  
                    //      p1: range 0.96..0.98 (overall speed)
                    //      p2: range 0.30..0.45 (extremeness of curvature)
                }
                else if (new_mode==18)
                {
                    // mode 2: vert. tunnel
                    scale1 = 0.97 - newx2*newx2*0.40;  
                    //      p1: range 0.96..0.98 (overall speed)
                    //      p2: range 0.30..0.45 (extremeness of curvature)
                }
                else if (new_mode==19)
                {
                    // mode 5: vortex (old mode 12)
                    scale1 = 1.04 - 0.25*sqrtf(newx2*newx2 + newy2*newy2);
                    //      p1: range 1.03..1.05 (suck-in speed @ center)
                    //      p2: range 0.15..0.30 (outward speed @ edges)
                }
                else if (new_mode==20)
                {
                    // mode 7: terra''
                    scale1 = 1.15 - sqrtf(newy2 + 1.4)*0.20;
                }
                else if (new_mode==21)
                {
                    // regular zoom-in but w/diced cube look (gritty, dirty electronics; shakes;)
                    scale1 = 0.95 - ((int)(fabsf(newx2)*10))*0.03 - ((int)(fabsf(newy2)*10))*0.03;
                }
                else if (new_mode==22)
                {
                    // ***AWESOME*** - phonic rings - scale is dicey based on r
                    scale1 = 0.95 - ((int)(sqrtf(newx2*newx2 + newy2*newy2)*10))*0.04;
                }
                else if (new_mode==23)
                {
                    // BADASS derivative of phonic rings - quicker response & fadeout!
                    scale1 = 0.95 - (((int)(sqrtf(newx2*newx2 + newy2*newy2)*20)) % 4)*0.12;
                }
                else if (new_mode==24)
                {
                    // mode 11: fast swirl, looks very good
                    scale1 = 0.96;  // range: 0.95 - 0.97
                    turn1 = 0.05;   // range: 0.04 - 0.06
                    cos_turn1 = cosf(turn1);
                    sin_turn1 = sinf(turn1);
                }
                else if (new_mode==25)
                {
                    // WOW: 1/r scaling... VERY good zoom, and fully speed-scalable (no curl!)
                    scale1 = 3.0 / (3.0 + sqrtf(newx2*newx2 + newy2*newy2)); // param1==param2: lower = faster.  1.0 is pretty fast already.  2.0 should probably be min; 6.0, max (very smooth)
                                // -use 0.3,0.3 for ultra-fast zoom, looks killer
                }
            }


                /*
                if (new_mode <= 24)
                {
                    if (new_mode==17)
                    {
                        // mode 0: flat zoom
                        scale1 = 0.99;
                        //      p1: range 0.86..0.99 (overall speed) (prefer 0.99)
                    }
                    else if (new_mode==20)
                    {
                        // mode 3: typical tunnel
                        scale1 = 0.9 - 0.10*sqrtf(newx2*newx2 + newy2*newy2);
                        //      p1: range 0.90..0.99 (overall speed)
                        //      p2: range 0.05..0.20 (extremeness of curvature)
                    }
                    else if (new_mode==21)
                    {
                        // mode 4: tunnel #2
                        scale1 = 0.95 - 0.30*(newx2*newx2 + newy2*newy2);
                        //      p1: range 0.93..0.97 (overall speed @ center)
                        //      p2: range 0.05..0.30 (additional speed @ edges)
                    }
                    else if (new_mode==23)
                    {
                        // mode 6: terra'
                        scale1 = 1.15 - (newy2 + 1.0)*0.20;
                    }
                }
                else // from 25-32
                {
                    if (new_mode==25)
                    {
                        // mode 8: sphere' - more like a sphere
                        scale1 = min(1.05, 0.90 + 0.25*(newx2*newx2 + newy2*newy2));
                    }
                    else if (new_mode==26)
                    {
                        // mode 9: sphere'' - more like a gaussian bump - smaller sphere
                        scale1 = min(1.1, 0.90 + 0.20*sqrtf(newx2*newx2 + newy2*newy2));
                    }
                    else if (new_mode==27)
                    {
                        // alternate #9: a diamond
                        scale1 = min(1.1, 0.90 + 0.15*(fabsf(newx2) + fabsf(newy2)));
                    }
                    else if (new_mode==28)
                    {
                        // hourglass
                        scale1 = min(1.1, 0.90 + 0.15*(fabsf(newx2) - fabsf(newy2)));
                    }
                    else if (new_mode==29)
                    {
                        // hall of mirrors
                        scale1 = 1.0 + 0.5*fabsf(newx2);
                    }
                    else if (new_mode==30)
                    {
                        // alternate to petals; looks incredibly SMOOTH for some reason
                        scale1 = 1.0 - 0.3*fabsf(newx2) - 0.3*fabsf(newy2) + 0.3*sqrtf(newx2*newx2 + newy2*newy2);
                    }
                }
                else if (new_mode==34)
                {
                    // so-so... scale is normal zoom, but turning is sinf(angle * int)
                    scale1 = 12.0 / (12.0 + sqrtf(newx2*newx2 + newy2*newy2)); // param1==param2: lower = faster.  1.0 is pretty fast already.  2.0 should probably be min; 6.0, max (very smooth)
                    turn1 = 0.03 + 0.02 * sinf(atan2(newx2,newy2) * 3.0 + 1.7);  // random phase; w = 2 or 3 (integers!)
                    cos_turn1 = cosf(turn1);
                    sin_turn1 = sinf(turn1);
                }
                else if (new_mode==35)
                {
                    // really good too; central spherical view, then edges get cut & propagated out
                    scale1 = 0.95; if (sqrtf(newx2*newx2 + newy2*newy2) > 0.5) scale1 = 0.9;
                }
                else if (new_mode==37)
                {
                    // mode 10: AWESOME super-fast stretch-to-death zoom
                    scale1 = 1.0 - 0.45*sqrtf(newx2*newx2 + newy2*newy2);
                    //      p1: 1.0
                    //      p2: 0.4..0.5 (extremeness... 0.5 brings slight curling @ edges)
                }
                */
            
            if (custom_motion_vectors[new_mode])
            {
                if (new_mode==6)
                {
                    tx = 0;
                    ty = 0;
                    f = 0;

                    for (n=0; n<5; n++)
                    {
                        if (ctype[n]==0)
                        {
                            newx = cx[n]-x;
                            newy = cy[n]-y;

                            d = (newx*newx + newy*newy);
                            //d *= rdiv;
                            d = 1.0/(d+0.1);
                            f += d;

                            tx += ci[n] * d;
                            ty += cj[n] * d;
                        }
                        else if (ctype[n]==1)
                        {
                            newx = cx[n]-x;
                            newy = cy[n]-y;

                            xxyy = (newx*newx + newy*newy);
                            d = xxyy;//*rdiv;
                            d = 1.0/(d+0.1);
                            f += d;

                            z = sqrtf(xxyy);
                            z = 1.0/(z+0.01);
                            
                            d += d;

                            tx += d*(-newy)*z;
                            ty += d*(newx)*z;
                        }
                        else if (ctype[n]==2)
                        {
                            newx = cx[n]-x;
                            newy = cy[n]-y;

                            xxyy = (newx*newx + newy*newy);
                            d = xxyy;//*rdiv;
                            d = 1.0/(d+0.1);
                            f += d;

                            z = sqrtf(xxyy);
                            z = 1.0/(z+0.01);
                            
                            d += d;

                            tx += d*(newy)*z;
                            ty += d*(-newx)*z;
                        }
                    }


                    if (f > 0.000001)
                    {
                        f = 1.9/f;
                        tx = tx*f;
                        ty = ty*f;
                    }
                    else
                    {
                        tx = 0;
                        ty = 0;
                    }
                
                    //tx -= (newx2*0.005); //0.002
                    //ty -= (newy2*0.005);

                    newx = x + tx - 0.1;
                    newy = y + ty + 0.6;
                    
                }
                else if (new_mode==10)
                {
                    newx = newx2*(1.03 + 0.03*(y/(float)FXH)     ) + new_gXC;
                    newy = y*1.04;
                } 
                else if (new_mode==12)   // sideways splitter - custom vectors!
                {
                    if (newx2 < -0.5)
                        newx = -sqrtf(-newx2) + new_gXC + 0.9;
                    else if (newx2 > 0.5)
                        newx = sqrtf(newx2) + new_gXC - 0.9;
                    else 
                        newx = new_gXC;

                    newy = newy2 + new_gYC;
                }
            }
            else if (rotation_dither[new_mode])
            {
                // vectors based on turn1 and scale1, and turn2 and scale2
                if ((x%2) == (y%2))
                {
                    newx = newx2*(cos_turn1) - newy2*(sin_turn1) ;    //Z rotation
                    newy = newx2*(sin_turn1) + newy2*(cos_turn1) ;
                    newx = (newx)*scale1*half_FXW + new_gXC;// + x_trans;
                    newy = (newy)*scale1*half_FXW + new_gYC;// + y_trans;
                }
                else
                {
                    newx = newx2*(cos_turn2) - newy2*(sin_turn2) ;    //Z rotation
                    newy = newx2*(sin_turn2) + newy2*(cos_turn2) ;
                    newx = (newx)*scale2*half_FXW + new_gXC;// + x_trans;
                    newy = (newy)*scale2*half_FXW + new_gYC;// + y_trans;
                }
            }
            else    // vectors based on turn1 and scale1
            {
                newx = newx2*(cos_turn1) - newy2*(sin_turn1);    //Z rotation
                newy = newx2*(sin_turn1) + newy2*(cos_turn1);
                newx = (newx)*scale1*half_FXW + new_gXC;// + x_trans;
                newy = (newy)*scale1*half_FXW + new_gYC;// + y_trans;
            }




            // DEBUG: point to exact same pixel.
            //newx = newx2 + new_gXC;
            //newy = newy2 + new_gYC;

            // NOTICE: the look-at pixel can be virtually
            // anywhere on X, but it is restrained to within
            // +/- 32767 pixels of the base pixel!
            // (due to storage of R_offset_rel)

          

            //if (SoundActive && !SoundEmpty && new_mode != 6)// && newx >= 0 && newx <= FXW-1)
            //if (new_damping_temp < 0.99)
            {
                newx = x*(1.0f - new_damping_temp) + newx*new_damping_temp;
                newy = y*(1.0f - new_damping_temp) + newy*new_damping_temp;
                //newx = (x + newx)*0.5;
                //newy = (y + newy)*0.5;

                while (newx < 0.0)    newx += FXW-1;
                while (newx > FXW-1) newx -= FXW-1;
            }
            /*else
            {
                while (newx < 0)   newx += FXW;
                while (newx > FXW) newx -= FXW;
            }*/

            /*
            while (newy < FX_YCUT) newy += (FXH-FX_YCUT*2 - 1);
            while (newy > FXH - FX_YCUT - 1) newy -= (FXH-FX_YCUT*2 - 1);
            */

            /*
            if (mode_motion_dampened[new_mode])
            {
                newx = (x + newx)*0.5;
                newy = (y + newy)*0.5;

                while (newx < 0.0)    newx += FXW-1;
                while (newx > FXW-1) newx -= FXW-1;
            }
            else
            {
                while (newx < 0)   newx += FXW;
                while (newx > FXW) newx -= FXW;
            }
            */

            /*
            // New in 4.26:
            if (newy < 0)
                newy = 0;
            if (newy > FXH-1)
                newy = FXH-1;
            if (newx > 0)
                newx -= ((int)newx/FXW)*FXW;
            else
                newx += ((-1 - (int)newx)/FXW + 1)*FXW;
            */

            a = (int)newx;
            b = (int)newy;
            R_offset = b*FXW + a;

            if (R_offset < FXW*2) 
                R_offset = FXW*2;
            if (R_offset >= FXW*(FXH-3)-1)
                R_offset =  FXW*(FXH-3)-1;

            R_offset_rel = R_offset - old_R_offset;    //- old_R_offset;//((long)y*320L+(long)x);        // store offset RELATIVE TO THIS PIXEL

            //int dist_from_center_squared = (a-FXW/2)*(a-FXW/2) + (b-FXH/2)*(b-FXH/2);
            int weightsum_this_pixel = weightsum_res_adjusted;//Diminish_Center() alternate:  - max(0,3 - 3*dist_from_center_squared/300);


            //AHA: must save room for +/- FXW*2 to be added on there, due to sliders...
            //Keep in mind that R_offset starts at 0 at the very top pixel.
            //if ( newy < 0 || newy >= FXH || R_offset_rel < 5 || R_offset_rel > 65530 )
            //if ( newy < 0 || newy > FXH-1 || R_offset_rel < 5 || R_offset_rel > 65530 )
            //if ( newy < FX_YCUT || newy > FXH-FX_YCUT || R_offset_rel < 5 || R_offset_rel > 65530 )
            //if ( newy < 1 || newy >= FXH-1)// || R_offset_rel < 5 || R_offset_rel > 65530 )  // changed so we could do some dynamic vertical shifting w/maps
            /*if (R_offset < FXW*2 || R_offset >= FXW*(FXH-3))
            {
                R_offset = old_R_offset;//A_offset/6;
                R_offset_rel = 0;//32767;// + R_offset - old_R_offset;    //- old_R_offset;//((long)y*320L+(long)x);        // store offset RELATIVE TO THIS PIXEL
                // the following is normally +0 +1 +2 +3
                DATA_FX2[A_offset + 0] = (unsigned char)(weightsum_res_adjusted/4);  // stores (0-255) TOPLEFT's weight
                DATA_FX2[A_offset + 1] = (unsigned char)(weightsum_res_adjusted/4);  // stores (0-255) TOPRIGHT's weight
                DATA_FX2[A_offset + 2] = (unsigned char)(weightsum_res_adjusted/4);  // stores (0-255) BTMLEFT's weight
                DATA_FX2[A_offset + 3] = (unsigned char)(weightsum_res_adjusted/4);  // stores (0-255) BTMRIGHT's weight
                ((int *)(DATA_FX2))[(A_offset + 4)/4] = R_offset_rel * bytewidth;
            }
            else*/
            {
                //R_offset = (long)b*FXW + (long)a;
                //R_offset_rel = 32767 + R_offset - old_R_offset;    //- old_R_offset;//((long)y*320L+(long)x);        // store offset RELATIVE TO THIS PIXEL
                /*if ((R_offset_rel < 0) || (R_offset_rel > 65535L)) 
                {
                    char buf[80];
                    wsprintf(buf, "R_offset_rel overflow!");
                    MessageBox( NULL, buf, "NOTICE", MB_OK );
                }*/
                newx2 = (newx - a);
                newy2 = (newy - b);
                /*
                DATA_FX2[A_offset + 0] = (unsigned char)((1-newx2)*(1-newy2)*weightsum_res_adjusted);  // stores (0-255) TOPLEFT's weight
                DATA_FX2[A_offset + 1] = (unsigned char)((  newx2)*(1-newy2)*weightsum_res_adjusted);  // stores (0-255) TOPRIGHT's weight
                DATA_FX2[A_offset + 2] = (unsigned char)((1-newx2)*(  newy2)*weightsum_res_adjusted);  // stores (0-255) BTMLEFT's weight
                DATA_FX2[A_offset + 3] = (unsigned char)((  newx2)*(  newy2)*weightsum_res_adjusted);  // stores (0-255) BTMRIGHT's weight
                DATA_FX2[A_offset + 4] = (unsigned char)(R_offset_rel & 0xff);
                DATA_FX2[A_offset + 5] = (unsigned char)(R_offset_rel >> 8);
                */
                // the following is normally +0 +1 +2 +3
                DATA_FX2[A_offset + 0] = (unsigned char)((1-newx2)*(1-newy2)*weightsum_this_pixel);  // stores (0-255) TOPLEFT's weight
                DATA_FX2[A_offset + 1] = (unsigned char)((  newx2)*(1-newy2)*weightsum_this_pixel);  // stores (0-255) TOPRIGHT's weight
                DATA_FX2[A_offset + 2] = (unsigned char)((1-newx2)*(  newy2)*weightsum_this_pixel);  // stores (0-255) BTMLEFT's weight
                DATA_FX2[A_offset + 3] = (unsigned char)((  newx2)*(  newy2)*weightsum_this_pixel);  // stores (0-255) BTMRIGHT's weight
                ((int *)(DATA_FX2))[(A_offset + 4)/4] = R_offset_rel * bytewidth;
                //DATA_FX2[A_offset + 4] = (unsigned char)((R_offset_rel & 0x000000ff) >>  0);
                //DATA_FX2[A_offset + 5] = (unsigned char)((R_offset_rel & 0x0000ff00) >>  8);
                //DATA_FX2[A_offset + 6] = (unsigned char)((R_offset_rel & 0x00ff0000) >> 16);
                //DATA_FX2[A_offset + 7] = (unsigned char)((R_offset_rel & 0xff000000) >> 24);
                    //DATA_FX2[A_offset + 4] = (unsigned char)(R_offset_rel & 0xff);
                    //DATA_FX2[A_offset + 5] = (unsigned char)(R_offset_rel >> 8);
            }

            old_R_offset = R_offset;

            x++;
            if (x==FXW)
            {
                x = 0;
                y++;
            }

        }
    }


    // new map is finished generating in background; apply it

    //if (y_map_pos == (FXH-FX_YCUT)*FXW &&
    //    ((g_rush_map)    ||    (!bBeatMode || (bBeatMode && bBigBeat)))
    //    )
    if (y_map_pos == (FXH-FX_YCUT)*FXW &&
        !g_rush_map && 
        bBeatMode && 
        !bBigBeat
        )
    {
        fBigBeatThreshold -= 0.2 / (float)frames_til_auto_switch;
    }
    else if (y_map_pos == (FXH-FX_YCUT)*FXW &&
        ((g_rush_map)    ||    (!bBeatMode || (bBeatMode && bBigBeat)))
        )
    {
        //dumpmsg("applying new map");  //big big

        bLoadPreset = g_bLoadPreset;
        iPresetNum = g_iPresetNum;

        y_map_pos = -1;    // means map is finished & applied.

        mode = new_mode;
        damping = new_damping;
        gXC = new_gXC;
        gYC = new_gYC;
        waveform = new_waveform;
        //initial_map_offset = new_initial_map_offset;
        //bScrollMap = new_bScrollMap;

        if (fps >= 5.0f && fps <= 120.0f)
            fps_at_last_mode_switch = fps;

        if (bLoadPreset) dumpmsg("preset map generated.");

        old_f1 = f1;
        old_f2 = f2;
        old_f3 = f3;
        old_f4 = f4;
        old_weightsum = weightsum;
        old_turn1 = turn1;
        old_turn2 = turn2;
        old_scale1 = scale1;
        old_scale2 = scale2;

        bLocked = FALSE;

        // clear off-screen areas for new mode
        if (mode != 6)
        {
            k = max(0, FX_YCUT_HIDE-6);    // top line to go from
            x = min(6, FX_YCUT_HIDE);      // # of lines to clear
            if (iDispBits==8)
            {
                memset(&VS1[k*FXW],         0, FXW*x);
                memset(&VS1[(FXH-1-k-x)*FXW], 0, FXW*x);
                memset(&VS2[k*FXW],         0, FXW*x);
                memset(&VS2[(FXH-1-k-x)*FXW], 0, FXW*x);
            }
            else 
            {
                memset(&VS1[k*FXW*4],         0, FXW*x*4);
                memset(&VS1[(FXH-1-k-x)*FXW*4], 0, FXW*x*4);
                memset(&VS2[k*FXW*4],         0, FXW*x*4);
                memset(&VS2[(FXH-1-k-x)*FXW*4], 0, FXW*x*4);
            }
        }

        //if (RND==1) srand(time(NULL) + intframe);
        //frames_til_auto_switch = 550;    

        
        if (bLoadPreset)
        {
            char section[64];
            sprintf(section, "PRESET %d", iPresetNum);
            char temp[64];
            effect[CHASERS] = GetPrivateProfileInt(section,"effect_chasers",0,inifile);
            effect[BAR]     = GetPrivateProfileInt(section,"effect_bar"    ,0,inifile);
            effect[DOTS]    = GetPrivateProfileInt(section,"effect_dots"   ,0,inifile);
            effect[SOLAR]   = GetPrivateProfileInt(section,"effect_solar"  ,0,inifile);
            effect[GRID]    = GetPrivateProfileInt(section,"effect_grid"   ,0,inifile);
            effect[NUCLIDE] = GetPrivateProfileInt(section,"effect_nuclide",0,inifile);
            effect[SHADE]   = GetPrivateProfileInt(section,"effect_shadebobs",0,inifile);
            effect[SPECTRAL]= GetPrivateProfileInt(section,"effect_spectral",0,inifile);
            g_bSlideShift   = GetPrivateProfileInt(section,"shift",0,inifile);
        }
        else 
        {
            g_bSlideShift = (rand()%100 + 1) <= g_SlideShiftFreq;

            for (x=0; x<NUM_EFFECTS; x++)
            {
                int thresh = modeInfo[mode].effect_freq[x];
                if (SoundActive && !SoundEmpty) thresh *= 0.7;
                if (rand()%1000 < thresh)
                    effect[x] = 1;
                else
                    effect[x] = -1;
            }
            modeInfo[mode].Clip_Num_Effects();

            if (effect[CHASERS]==1)
            {
                effect[CHASERS] = 1 + rand()%2;
            }

            // if grid is on, turn bar off
            if (effect[GRID]==1) effect[BAR] = -1; /* turn heavy bar off */

            // initial sun effect
            if (mode==1 && rand()%2==0) Drop_Solar_Particles(500);

            grid_dir = (rand()%2)*2-1;
            if (effect[NUCLIDE]>0 && rand()%7>2) waveform = 0;

            #if PLUGIN
                visMode = wave;
            #endif

            if (new_mode==10) 
            {
                waveform = 1;
                #if PLUGIN
                    if (rand()%4 > 0) visMode = spectrum;
                #endif
            }

            if (new_mode==15 && rand()%5==0)
                waveform = 5;
            //if (new_mode==18) 
            //    waveform = rand()%2 + 1;

            if (effect[SPECTRAL]==1)
                waveform = 0;
        }

        if (!g_bSlideShift) { slider1 = 0; }

        solar_max = modeInfo[mode].solar_max;
        center_dwindle = modeInfo[mode].center_dwindle;

        // swap maps
        unsigned char *pFX;
        pFX = DATA_FX;
        DATA_FX = DATA_FX2;
        DATA_FX2 = pFX;

        if (!bLoadPreset && intframe > 0)
        {
            FX_Random_Palette();
        }

        /* more effect[SPECTRAL] STUFF:
        float sum;
        for (int i=0; i<NUM_FREQS; i++)
        do
        {
            g_DotColor[i][0] = (rand()%1000)*0.001;
            g_DotColor[i][1] = (rand()%1000)*0.001;
            g_DotColor[i][2] = (rand()%1000)*0.001;
            sum = g_DotColor[i][0] + g_DotColor[i][1] + g_DotColor[i][2];
        }
        while (sum < 1.2);
        */

        g_rush_map = false;  // finished rushing.

    }


    //if (g_rush_map) 
    //{
    //    clock_debt += (clock() - temp_clock);
    //}

    bLoadPreset = false;

    //g_pause = false;
}







void RenderFX()
{
    unsigned int addr;// a, b, z1, z2;
    long k, k1;// k2;
    float newx, newy;
    unsigned char quitnow=0;
    //unsigned char p1, p2, p3;
    int x, y, dir;
    unsigned int fx, z0, A_offset, A_number;// z, R_offset;
    unsigned long arraypos;// D_offset;
    //unsigned char m;

    //g_FramesSinceShift++;

  SetCursor(NULL); //otherwise, in Winamp, you can hit 'c' or something and the cursor will pop up!

    floatframe += 1.6f * min(1.0f, 47.0f/fps);
    intframe++;
    frames_this_mode++;

    if ((intframe % 11) == 0)
      clearframes = 1;

    //dither/////AMDTEST
    //dither//#if AMDTEST
    //dither//    bDither = 1;
    //dither//    bRandomDither = 0;
    //dither//#endif
    //dither/////AMDTEST


/*
    if (iDispBits>8 && g_hit >= 0)    // g_hit at -1 means 'we got it at zero, don't bother again.'
    {
        for (x=0; x<256; x++) 
        {
            // this is only used for 16-bit non-MMX... disabling it so that g_hit is sync'd to title blits.
            //REMAP[x]  = min(255, x*(2 + g_hit*0.1));

            //REMAP2[x] = REMAP[x];
            //REMAP3[x] = REMAP[x];
        }
    }
*/

    if (iBlendsLeftInPal > 0)
    {
        iBlendsLeftInPal--;
        PutPalette();
    }

    //dither//if (bRandomDither)
    //dither//{
    //dither//    if (/*rand()%600==0 ||*/ (bMEGARandomDither && intframe%(int)(fps*1.5)==0))
    //dither//    {
    //dither//        bDither = !bDither;
    //dither//    }
    //dither//}

    //----------------------------------------------------------------------------------------------------------
    // VS1 is source, VS2 is dest.


    //------EFFECTS----------------------------------------------------------------------------------------------------

    //if ((frames_since_last_plop++ > 200) && (rand()%2500 == 1))
    //{
        //FX_Loadimage("fx.tga");
    //}

    if (effect[SHADE]>0) ShadeBobs();
    if (effect[CHASERS]>=1) Two_Chasers(floatframe + chaser_offset);
    if (effect[BAR] == 1)
    {
        float speed_mult = 0.6f; // 1.0f = what was in geiss 4.25 and before
        float chromatic_dispersion = 4.0f;  //1.0f = what was in geiss 4.25 and before
        if (iDispBits == 8)
        {
            Solid_Line(floatframe + chaser_offset*speed_mult, VS1);
        }
        /*
        if (iDispBits == 16)
        {
            Solid_Line(floatframe + chaser_offset, VS1);
            Solid_Line(floatframe + chaser_offset + 3.5*(sinf(floatframe*0.03+1)+cosf(floatframe*0.04+3)), VS3);
            Solid_Line(floatframe + chaser_offset - 3.5*(cosf(floatframe*0.05+2)+sinf(floatframe*0.06+4)), VS5);
        }
        else if (iDispBits == 32)
        */
        else
        {
            //snackbar
            Solid_Line(floatframe + chaser_offset*speed_mult, VS1);
            Solid_Line(floatframe + chaser_offset*speed_mult + 3.5*chromatic_dispersion*(sinf(floatframe*0.03+1)+cosf(floatframe*0.04+3)), &VS1[1]);
            Solid_Line(floatframe + chaser_offset*speed_mult - 3.5*chromatic_dispersion*(cosf(floatframe*0.05+2)+sinf(floatframe*0.06+4)), &VS1[2]);
        }
    }
    if (effect[DOTS]==1) One_Dotty_Chaser(floatframe);
    if (effect[NUCLIDE]==1) Nuclide();  //SEE ALSO: RenderDots()


    //if (mode==6) Mode6Edges();
    if (effect[GRID] == 1) Grid();
    
    //DoCrystals(1, gXC + rand()%30 - 15, gYC + rand()%30 - 15 );


    if (effect[SOLAR]==1)
    {
        if (FXW==320)
        {
            fx = 3 + solar_max*(2.4 + 0.35*sinf(intframe*0.05) + 0.4*sinf(intframe*0.038+1));
            Drop_Solar_Particles_320(fx*0.01);
        }
        else
        {
            fx = 3 + solar_max*1.6 + solar_max*0.43*sinf(intframe*0.05) + solar_max*0.43*sinf(intframe*0.038+1);
            Drop_Solar_Particles(fx*0.05);
        }
    }

    Diminish_Center(VS1);
    /*
    if (iDispBits == 16)
    {
        Diminish_Center(VS3);
        Diminish_Center(VS5);
    }
    */
}



BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    DDSCAPS             ddscaps;
    HRESULT             ddrval;
    HDC                 hdc;
    char                buf[256];

    #if PLUGIN
        if (this_mod_hwndParent && SendMessage(this_mod_hwndParent,WM_USER,0,104)!=1)    // WM_USER/104 return codes: 1=playing, 3=paused, other=stopped
        {
          MessageBox(this_mod_hwndParent, "This visualizer requires music in order to run.\rPlease play some music and then start the plugin.", "No Music Playing", MB_OK);
          return false;
        }
    #endif

    bMMX = CheckMMXTechnology();    
    if (bMMX) dumpmsg("MMX detected!");
    else dumpmsg("MMX not detected.");

    //bCyrix = CyrixInstead();
    //if (bCyrix) dumpmsg("Using Cyrix Optimizations.");
    //else dumpmsg("Using default processor optimizations.");

    strcat(inifile, "\\geiss.ini");
    dumpmsg(".INI file to use:");
    dumpmsg(inifile);    


#if SAVER
    HWND                hMainWnd;   // for the plugin, hMainWnd is a GLOBAL!
#endif


#if PLUGIN
    //cursor//added in 3.40
    hOldCursor = GetCursor();
#endif

    /*
     * set up and register window class
     */
#if SAVER
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;                    // prcessess messages
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;  // must be 'this_mod->hDllInstance' for plugin!  (okay)
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = NULL;//cursor//LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass(&wc); 
#endif
#if PLUGIN

                  
    
/*
                    WNDCLASS wc2;
                    memset(&wc2,0,sizeof(wc2));
                    wc2.lpfnWndProc = WindowProc2;            // our window procedure
                    wc2.hInstance = hInstance;                // hInstance of DLL
                    wc2.lpszClassName = NAME2;  // our window class name
                    if (!RegisterClass(&wc2)) 
                    {
                        MessageBox(this_mod_hwndParent,"Error registering SECOND window class","blah",MB_OK);
                        return 1;
                    }

                    hSecondWnd = CreateWindow(
                        NAME2,
                        NAME2,
                        WS_CHILD | WS_MINIMIZE,
                        0, 0,
                        0, //GetSystemMetrics( SM_CXSCREEN ),     // beely beely!!!! was 640x80!!!
                        0, //GetSystemMetrics( SM_CYSCREEN ),     // (changed 12/22/98)

                        // This used to point to the Winamp window (parent), but that let Winamp write
                        // over the directdraw surface & would cause flickering at times.  If we don't
                        // specify a parent window, we truly take over the screen.  So we say NULL here.
                        this_mod_hwndParent,        // this_mod->hwndParent - causes flicker bug
                        //NULL,                          // causes no-winamp-keys bug

                        NULL,
                        hInstance, // this_mod->hDllInstance   okay
                        NULL 
                    );
                    if( !hSecondWnd )
                    {
                        MessageBox(NULL, "CreateWindowEx() failed for subwindow.", "ERROR", MB_OK);
                        return 1;
                    }
*/

    memset(&wc,0,sizeof(wc));
    wc.lpfnWndProc = WindowProc;            // our window procedure
    wc.hInstance = this_mod_hDllInstance;    // hInstance of DLL    //3.40 beta4
    wc.lpszClassName = NAME;                  // our window class name

    //TESTING
    //wc.style = CS_PARENTDC;
    //TESTING

    if (!RegisterClass(&wc)) 
    {
        MessageBox(this_mod_hwndParent,"Error registering window class","blah",MB_OK);
        return 1;
    }

#endif

    /*
     * create a window
     */
    dumpmsg("creating main window...");

    // must be hMainWnd for plugin
    hMainWnd = CreateWindowEx(
        #if (GRFX==1)
            WS_EX_TOPMOST,
        #else
            0,
        #endif
        NAME,
        TITLE,
        WS_POPUP | WS_VISIBLE,  // WS_VISIBLE added in 3.40/beta 4
        0, 0,
        #if (GRFX==1)
            FXW, FXH, 
        #else
            512, 512, 
        #endif

        // This used to point to the Winamp window (parent), but that let Winamp write
        // over the directdraw surface & would cause flickering at times.  If we don't
        // specify a parent window, we truly take over the screen.  So we say NULL here.
        // That will keep the window from receiving keyboard input to pass on to Winamp;
        // that's why we make the 2nd window, hSecondWnd, whose parent is Winamp.
        //NULL,
#if SAVER
        NULL, 
#endif
#if PLUGIN
            NULL, //MULTIMON: parent window: set to NULL, otherwise, with multimon, you can't use winamp.       //this_mod_hwndParent,
#endif

        NULL,
#if SAVER
        hInstance,
#endif
#if PLUGIN
        this_mod_hDllInstance,//hInstance,       changed in 3.40/beta 4
#endif
        NULL 
    );

    if( !hMainWnd )
    {
        MessageBox(NULL, "CreateWindowEx() failed.", "ERROR", MB_OK);
        return 1;
    }

    //MessageBox(NULL, "test", "test", MB_OK);

//#if SAVER
    //Outsound_Init(hMainWnd);
//#endif

#if SAVER
    hSaverMainWindow = hMainWnd;
#endif

#if PLUGIN

    SetWindowLong(hMainWnd, GWL_USERDATA, (LONG)g_this_mod); // set our user data to a "this" pointer
    {    // adjust size of window to make the client area exactly width x height
        // gemini: removed 4 following lines for v3.40 beta 2:
        //RECT r;
        //int width=288, height=256;
        //GetClientRect(hMainWnd,&r);
        //SetWindowPos(hMainWnd,0,0,0,width*2-r.right,height*2-r.bottom,SWP_NOMOVE|SWP_NOZORDER);

        SetWindowPos(hMainWnd, NULL, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );  // changed in beta 3.40/beta 
        
        // create our doublebuffer
        //memDC = CreateCompatibleDC(NULL);
        //memBM = CreateCompatibleBitmap(memDC,width,height);
        //oldBM = (HBITMAP)SelectObject(memDC,memBM);

    }

    //::SetWindowPos(this_mod_hwndParent, NULL, g_WinampWindowRect.left, FXH+20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

#endif

    dumpmsg("showwindow & updatewindow...");

#if SAVER
    #if (GRFX==1)
        ShowWindow( hMainWnd, SW_MAXIMIZE /*nCmdShow*/ );
    #else
        ShowWindow( hMainWnd, SW_SHOWNORMAL );
    #endif
    UpdateWindow( hMainWnd );               //beely2: wasn't in plugin
#endif

#if PLUGIN
    // WITHOUT UpdateWindow()
    // ----------------------
    // with nothing here, or with ShowWindow(SW_HIDE), or with ShowWindow(SW_SHOWNORMAL)
    // you get the [special Winamp, if enabled] mouse cursor whenever the mouse is over 
    // the Winamp window, and the window often overwrites the DDSurface.
    //
    // with ShowWindow(SW_MINIMIZE), @ exit you get a "plugin executed illegal op..." message.
    //
    //
    // WITH UpdateWindow()
    // -------------------
    // same stuff


    //minimize the Winamp window, if necessary
    #if (GRFX==1)
        /*if (g_bMinimizeWinamp)
        {
            ShowWindow(this_mod_hwndParent, SW_MINIMIZE);
            g_WinampMinimizedByHand = true;
        }*/
    #endif    

    ShowWindow( hMainWnd, SW_SHOWNORMAL );
    

  
#endif

    //cursor//added in 3.40
    SetCursor(NULL);


//temp//
//#if PLUGIN
    //SetWindowPos(this_mod_hwndParent, hMainWnd, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );  // changed in beta 3.40/beta 
    //SetWindowPos(this_mod_hwndParent, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );  // changed in beta 3.40/beta 
//#endif
//temp//


    if (bMMX) dumpmsg("MMX detected!");
    else dumpmsg("MMX not detected.");

    //if (bCyrix) dumpmsg("Using Cyrix Optimizations.");
    //else dumpmsg("Using default processor optimizations.");


    /*
     * create the main DirectDraw object
     */

    dumpmsg("directdrawcreate()...");
    sprintf(buf, "  guid = %08x %08x", (DWORD*)(&g_DDrawDeviceGUID), (DWORD*)(&g_DDrawDeviceGUID)+1 );
    dumpmsg(buf);

    #if (GRFX==1)
      ddrval = DirectDrawCreate( &g_DDrawDeviceGUID, &lpDD, NULL );  //first param = GUID of adapter to use
      if (FAILED(ddrval))
        ddrval = DirectDrawCreate( NULL, &lpDD, NULL );  //first param = GUID of adapter to use
    #else
        ddrval = DD_OK;
    #endif
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        //ddrval = lpDD->SetCooperativeLevel( hMainWnd,
        //            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        //#if PLUGIN
            //::SetWindowPos(this_mod_hwndParent, NULL, g_WinampWindowRect.left, FXH+30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            //g_WinampWindowMoved = true;
        //#endif

        #if (GRFX==1)
            dumpmsg("setcooplevel()...");
            ddrval = lpDD->SetCooperativeLevel( hMainWnd, 
                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );//| DDSCL_ALLOWREBOOT );//| DDSCL_NOWINDOWCHANGES );
        #else
            ddrval = DD_OK;
        #endif

        /*
        dumpmsg("setcooplevel()...");
        ddrval = lpDD->SetCooperativeLevel( hMainWnd, 
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_ALLOWMODEX );//| DDSCL_NOWINDOWCHANGES );
        if(ddrval != DD_OK)
        {
            // try it again, w/o MODEX
            dumpmsg("  -didn't like mode X cooperative level, trying again without it-");

            lpDD->Release();
            ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
            if (ddrval != DD_OK) 
            {
                dumpmsg("FAILED second call to DirectDrawCreate()");
            }
            else
            {
                ddrval = lpDD->SetCooperativeLevel( hMainWnd, 
                    DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT );//| DDSCL_NOWINDOWCHANGES );
            }
        }
        */

        // DO THIS PRIOR TO CALLING EnumDisplayModes / EnumModesCallback:
        GuessDefaultDisplayMode();

        if(ddrval == DD_OK )
        {
            #if (GRFX==1)
                dumpmsg("setdisplaymode()...");
                ddrval = lpDD->SetDisplayMode( FXW, FXH, iDispBits );
                if (ddrval != DD_OK) 
                {
                  // find closest match and try again.
                  iNumVidModes = 0;
                  VidMode = -1;
                  ddrval = lpDD->EnumDisplayModes(0, NULL, NULL, &EnumModesCallback);

                  if (ddrval==DD_OK && VidMode >= 0)
                  {
                    sprintf(buf, "WARNING: SetDisplayMode failed at %d x %d x %d...", FXW, FXH, iDispBits);
                    dumpmsg(buf);

                    SetWindowPos(hMainWnd, NULL, 0, 0, FXW, FXH, SWP_NOMOVE|SWP_NOZORDER);

                    VIDEO_CARD_FUCKED = VidList[VidMode].VIDEO_CARD_FUCKED;
                    FXW = VidList[VidMode].FXW;
                    FXH = VidList[VidMode].FXH;
                    iDispBits = VidList[VidMode].iDispBits;

                    OnFxwFxhUpdated();

                    sprintf(buf, "...trying again at nearest match: %d x %d x %d...", FXW, FXH, iDispBits);
                    dumpmsg(buf);

                    ddrval = lpDD->SetDisplayMode( FXW, FXH, iDispBits );
                  }
                }
            #else
                ddrval = DD_OK;
            #endif
            if ( ddrval == DD_OK )
            {

                // do this here, while monitor changes display mode
                if (!FX_Init())
                {
                    #if SAVER
                        exit(10);
                    #endif
                    #if PLUGIN
                        return false;
                    #endif
                }

                #if (GRFX==1)
                    // Create the primary surface with 1 back buffer
                    ddsd.dwSize = sizeof( ddsd );
                    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;// | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM; 
                    //if (bModeX) 
                    //{
                    //    ddsd.ddsCaps.dwCaps |= DDSCAPS_MODEX;
                    //}
                    ddsd.dwBackBufferCount = 1;
                    

                    dumpmsg("createsurface()...");

                    ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                #else
                    ddrval = DD_OK;
                #endif
                if( ddrval == DD_OK )
                {
                    #if (GRFX==1)
                        // Get a pointer to the back buffer
                        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                        dumpmsg("getattachedsurface()...");  
                        ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                          &lpDDSBack);
                    #else
                        ddrval = DD_OK;
                    #endif
                    if( ddrval == DD_OK )
                    {
                        // generate some debug info...
                        #if (GRFX==1)
                            lpDDSPrimary->GetSurfaceDesc(&ddsd);
                            if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) dumpmsg("Primary surface is in video memory.");
                            if (ddsd.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) dumpmsg("Primary surface is in LOCAL video memory.");
                            if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) dumpmsg("Primary surface is in system memory.");
                            lpDDSBack->GetSurfaceDesc(&ddsd);
                            if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) dumpmsg("Back surface is in video memory.");
                            if (ddsd.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) dumpmsg("Back surface is in LOCAL video memory.");
                            if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) dumpmsg("Back surface is in system memory.");
                        #endif

                        // READY TO GO!!!!!!
                        dumpmsg("DirectDraw setup complete!");

                        // start up a palette, if necessary
                        if (iDispBits==8)
                        {
                            for (int n=0; n<256; n++)
                            {
                                ape2[n].peRed = 0;
                                ape2[n].peBlue = 0;
                                ape2[n].peGreen = 0;
                            }
                            #if (GRFX==1)
                                lpDD->CreatePalette(DDPCAPS_8BIT,ape,&lpDDPal,NULL);
                                lpDDSPrimary->SetPalette(lpDDPal);
                            #endif
                        }
                        dumpmsg("palette init complete.");
                        FX_Random_Palette();
                        dumpmsg("palette applied.");

                        return TRUE;  // all systems go.
                    }
                }
                else // couldn't create surface
                {
                    MessageBox( hMainWnd, "lpDD->CreateSurface() failed.", "ERROR", MB_OK );
                    dumpmsg(buf); 
                    finiObjects();
                    return FALSE;
                }
            } 
            else // couldn't get video mode
            {
                wsprintf(buf, "Sorry!  Your video card doesn't support the %dx%dx%d video mode.\n", FXW, FXH, iDispBits, ddrval );
                MessageBox( hMainWnd, buf, "ERROR", MB_OK );
                dumpmsg(buf); 
                finiObjects();
                return FALSE;
            }
        } 
        else
        {
#if SAVER
            wsprintf(buf, "DirectX couldn't set EXCLUSIVE and FULLSCREEN mode (ddrval=%08lx).", ddrval );
            MessageBox( hMainWnd, buf, "ERROR INITIALIZING GEISS", MB_OK );
#endif
#if PLUGIN
            wsprintf(buf, "DirectX couldn't set EXCLUSIVE and FULLSCREEN mode (ddrval=%08lx).  If you are using Winamp's DIRECTSOUND output plugin, please try using, instead, the WAVEOUT output plugin.  You can do this from Winamp's 'PREFERENCES' screen.", ddrval );
            MessageBox( hMainWnd, buf, "ERROR INITIALIZING GEISS", MB_OK );
#endif
            dumpmsg(buf); 
            finiObjects();
            return FALSE;
        }
    }
    wsprintf(buf, "Direct Draw Init Failed - you probably don't have DirectX installed! (%08lx)\n", ddrval );
    MessageBox( hMainWnd, buf, "ERROR", MB_OK );
    dumpmsg(buf); 
    finiObjects();
    return FALSE;
}











void finiObjects( void )
{
    dumpmsg("  finiObjects starts...");

//#if SAVER
    //Outsound_Fini();
//#endif

    #if (GRFX==1)
        if( lpDD != NULL )
        {
            lpDD->RestoreDisplayMode();

            if( lpDDSPrimary != NULL )
            {
                lpDDSPrimary->Release();
                lpDDSPrimary = NULL;
            }
            if(lpDDPal!=NULL)
            {
                lpDDPal->Release();
                lpDDPal=NULL;
            }
            if (lpDD != NULL)
            {
                lpDD->Release();
                lpDD = NULL;
            }
        }
    #endif

#if SAVER
    if (SoundEnabled && SoundReady)// && SoundActive)
    {
        HRESULT hr;
        hr = pDSCB->Stop();
        SoundActive = FALSE;
    }
#endif

    //RestoreCtrlAlt();
    FX_Fini();

#if PLUGIN
    if (hMainWnd) 
    {
        DestroyWindow(hMainWnd); // delete our window
        hMainWnd = NULL;
    }
    //if (hSecondWnd)
    //{
    //    DestroyWindow(hSecondWnd); // delete our window
    //    hSecondWnd = NULL;
    //}

    UnregisterClass(NAME /*szAppName*/,this_mod_hDllInstance); // unregister window class
    //UnregisterClass(NAME2 /*szAppName*/,this_mod->hDllInstance); // unregister window class

    // move Winamp window back to where it was
    /*
    if (g_WinampWindowMoved)
    {
        RECT r;
        ::GetWindowRect(this_mod_hwndParent, &r);
        if (r.bottom != g_WinampWindowRect.bottom ||
            r.top != g_WinampWindowRect.top ||
            r.right != g_WinampWindowRect.right ||
            r.left != g_WinampWindowRect.left)
        {
            ::SetWindowPos(this_mod_hwndParent, NULL, g_WinampWindowRect.left, g_WinampWindowRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
    */
    /*if (g_bMinimizeWinamp && g_WinampMinimizedByHand)
    {
            ShowWindow(this_mod_hwndParent, SW_RESTORE);
        g_WinampMinimizedByHand = false;  // just in case finiObjects gets called again.
    }*/

    //cursor//
    /*if (hOldCursor)
    {
        SetCursor(hOldCursor);
        hOldCursor = NULL;
    }*/

#endif

    dumpmsg("  finiObjects completes.");
    
    //gone//RestoreCtrlAlt();   // for threaded app, atexit() won't work?


} /* finiObjects */


#if SAVER
void TryToExit(HWND hWnd)
{
    // temporary palette, so password dialog is nice & visible
    if (iDispBits==8)
    {
        for (int i=0; i<16; i++)
        {
            float z = 16*i;
            apetemp[i].peRed =   z*1.0;
            apetemp[i].peGreen = z*0.9;
            apetemp[i].peBlue =  z*0.8;
        }
        for (int i=0; i<16; i++)
        {
            float z = 16*(i+16);
            float t = (i/16.0f);
            apetemp[i+16].peRed   = min(255, z*1.0*(1-t) + t*ape2[i+16].peRed);
            apetemp[i+16].peGreen = min(255, z*0.9*(1-t) + t*ape2[i+16].peGreen);
            apetemp[i+16].peBlue  = min(255, z*0.8*(1-t) + t*ape2[i+16].peBlue);
        }
        lpDDPal->SetEntries(0,0,256,apetemp);
    }

    bool CanClose = (intframe < 50) ? true : VerifyPassword(hWnd);
    if (CanClose)
    {
        dumpmsg("Ok to close");
        g_QuitASAP = true;
        //DestroyWindow(hWnd);  // taken care of in finiObjects()
    }
    else
    {
        dumpmsg("NOT ok to close");
        //return FALSE;

        // if the surface was lost (say the system went into standby or the monitor
        // powered down), and now they're trying to exit, if when they're done they
        // don't enter the correct password, give the surfaces another chance because
        // they might be ready.
        if (g_DDRAW_FRAGGED_req)
        {
            g_DDRAW_FRAGGED_req = false;
            g_DDRAW_FRAGGED_ack = false;
            g_DDRAW_FRAGGED_req = false;
            g_DDRAW_FRAGGED_ack = false;
            g_DDRAW_ERRORS = 950;
        }
    }

    // restore palette
    if (iDispBits==8)
    {
        lpDDPal->SetEntries(0,0,256,ape2);
    }

}
#endif

#if _DEBUG
  void PrintWindowsMessage(DWORD uMsg, char* szOut)
  {
      szOut[0] = 0;
      //#ifdef _DEBUG
          #define CheckMsg(x) if (uMsg==x) { strcpy(szOut, #x); return; }

          CheckMsg(WM_NULL                      ) //0x0000
          CheckMsg(WM_CREATE                    ) //0x0001
          CheckMsg(WM_DESTROY                   ) //0x0002
          CheckMsg(WM_MOVE                      ) //0x0003
          CheckMsg(WM_SIZE                      ) //0x0005
          CheckMsg(WM_ACTIVATE                  ) //0x0006
          CheckMsg(WM_SETFOCUS                  ) //0x0007
          CheckMsg(WM_KILLFOCUS                 ) //0x0008
          CheckMsg(WM_ENABLE                    ) //0x000A
          CheckMsg(WM_SETREDRAW                 ) //0x000B
          CheckMsg(WM_SETTEXT                   ) //0x000C
          CheckMsg(WM_GETTEXT                   ) //0x000D
          CheckMsg(WM_GETTEXTLENGTH             ) //0x000E
          CheckMsg(WM_PAINT                     ) //0x000F
          CheckMsg(WM_CLOSE                     ) //0x0010
          #ifndef _WIN32_WCE
          CheckMsg(WM_QUERYENDSESSION         ) //0x0011
          CheckMsg(WM_QUERYOPEN               ) //0x0013
          CheckMsg(WM_ENDSESSION              ) //0x0016
          #endif            
          CheckMsg(WM_QUIT                      ) //0x0012
          CheckMsg(WM_ERASEBKGND                ) //0x0014
          CheckMsg(WM_SYSCOLORCHANGE            ) //0x0015
          CheckMsg(WM_SHOWWINDOW                ) //0x0018
          CheckMsg(WM_WININICHANGE              ) //0x001A
          #if(WINVER >= 0x0400)   
          //CheckMsg(WM_SETTINGCHANGE           ) //     WM_WININICHANGE
          #endif // WINVER >= 0x0400   
          CheckMsg(WM_DEVMODECHANGE             ) //0x001B
          CheckMsg(WM_ACTIVATEAPP               ) //0x001C
          CheckMsg(WM_FONTCHANGE                ) //0x001D
          CheckMsg(WM_TIMECHANGE                ) //0x001E
          CheckMsg(WM_CANCELMODE                ) //0x001F
          CheckMsg(WM_SETCURSOR                 ) //0x0020
          CheckMsg(WM_MOUSEACTIVATE             ) //0x0021
          CheckMsg(WM_CHILDACTIVATE             ) //0x0022
          CheckMsg(WM_QUEUESYNC                 ) //0x0023
          CheckMsg(WM_GETMINMAXINFO             ) //0x0024
          CheckMsg(WM_PAINTICON                 ) //0x0026
          CheckMsg(WM_ICONERASEBKGND            ) //0x0027
          CheckMsg(WM_NEXTDLGCTL                ) //0x0028
          CheckMsg(WM_SPOOLERSTATUS             ) //0x002A
          CheckMsg(WM_DRAWITEM                  ) //0x002B
          CheckMsg(WM_MEASUREITEM               ) //0x002C
          CheckMsg(WM_DELETEITEM                ) //0x002D
          CheckMsg(WM_VKEYTOITEM                ) //0x002E
          CheckMsg(WM_CHARTOITEM                ) //0x002F
          CheckMsg(WM_SETFONT                   ) //0x0030
          CheckMsg(WM_GETFONT                   ) //0x0031
          CheckMsg(WM_SETHOTKEY                 ) //0x0032
          CheckMsg(WM_GETHOTKEY                 ) //0x0033
          CheckMsg(WM_QUERYDRAGICON             ) //0x0037
          CheckMsg(WM_COMPAREITEM               ) //0x0039
          #if(WINVER >= 0x0500)        
              #ifndef _WIN32_WCE       
                  CheckMsg(WM_GETOBJECT         ) //0x003D
              #endif                   
          #endif // WINVER >= 0x0500   
          CheckMsg(WM_COMPACTING                ) //0x0041
          CheckMsg(WM_COMMNOTIFY                ) //0x0044  // no longer suported 
          CheckMsg(WM_WINDOWPOSCHANGING         ) //0x0046
          CheckMsg(WM_WINDOWPOSCHANGED          ) //0x0047
          CheckMsg(WM_POWER                     ) //0x0048
          CheckMsg(WM_COPYDATA                  ) //0x004A
          CheckMsg(WM_CANCELJOURNAL             ) //0x004B
          #if(WINVER >= 0x0400)        
              CheckMsg(WM_NOTIFY                ) //0x004E
              CheckMsg(WM_INPUTLANGCHANGEREQUEST) //0x0050
              CheckMsg(WM_INPUTLANGCHANGE       ) //0x0051
              CheckMsg(WM_TCARD                 ) //0x0052
              CheckMsg(WM_HELP                  ) //0x0053
              CheckMsg(WM_USERCHANGED           ) //0x0054
              CheckMsg(WM_NOTIFYFORMAT          ) //0x0055
              CheckMsg(WM_CONTEXTMENU           ) //0x007B
              CheckMsg(WM_STYLECHANGING         ) //0x007C
              CheckMsg(WM_STYLECHANGED          ) //0x007D
              CheckMsg(WM_DISPLAYCHANGE         ) //0x007E
              CheckMsg(WM_GETICON               ) //0x007F
              CheckMsg(WM_SETICON               ) //0x0080
          #endif // WINVER >= 0x0400   
          CheckMsg(WM_NCCREATE                  ) //0x0081
          CheckMsg(WM_NCDESTROY                 ) //0x0082
          CheckMsg(WM_NCCALCSIZE                ) //0x0083
          CheckMsg(WM_NCHITTEST                 ) //0x0084
          CheckMsg(WM_NCPAINT                   ) //0x0085
          CheckMsg(WM_NCACTIVATE                ) //0x0086
          CheckMsg(WM_GETDLGCODE                ) //0x0087
          #ifndef _WIN32_WCE           
              CheckMsg(WM_SYNCPAINT             ) //0x0088
          #endif                       
          CheckMsg(WM_NCMOUSEMOVE               ) //0x00A0
          CheckMsg(WM_NCLBUTTONDOWN             ) //0x00A1
          CheckMsg(WM_NCLBUTTONUP               ) //0x00A2
          CheckMsg(WM_NCLBUTTONDBLCLK           ) //0x00A3
          CheckMsg(WM_NCRBUTTONDOWN             ) //0x00A4
          CheckMsg(WM_NCRBUTTONUP               ) //0x00A5
          CheckMsg(WM_NCRBUTTONDBLCLK           ) //0x00A6
          CheckMsg(WM_NCMBUTTONDOWN             ) //0x00A7
          CheckMsg(WM_NCMBUTTONUP               ) //0x00A8
          CheckMsg(WM_NCMBUTTONDBLCLK           ) //0x00A9
          #if(_WIN32_WINNT >= 0x0500)  
              CheckMsg(WM_NCXBUTTONDOWN         ) //0x00AB
              CheckMsg(WM_NCXBUTTONUP           ) //0x00AC
              CheckMsg(WM_NCXBUTTONDBLCLK       ) //0x00AD
          #endif // _WIN32_WINNT >= 0x0500 
          #if(_WIN32_WINNT >= 0x0501)      
              CheckMsg(WM_INPUT                 ) //0x00FF
          #endif // _WIN32_WINNT >= 0x0501 
          //CheckMsg(WM_KEYFIRST                  ) //0x0100
          CheckMsg(WM_KEYDOWN                   ) //0x0100
          CheckMsg(WM_KEYUP                     ) //0x0101
          CheckMsg(WM_CHAR                      ) //0x0102
          CheckMsg(WM_DEADCHAR                  ) //0x0103
          CheckMsg(WM_SYSKEYDOWN                ) //0x0104
          CheckMsg(WM_SYSKEYUP                  ) //0x0105
          CheckMsg(WM_SYSCHAR                   ) //0x0106
          CheckMsg(WM_SYSDEADCHAR               ) //0x0107
          #if(_WIN32_WINNT >= 0x0501)      
              CheckMsg(WM_UNICHAR               ) //0x0109
              CheckMsg(WM_KEYLAST               ) //0x0109
              CheckMsg(UNICODE_NOCHAR           ) //0xFFFF
          #else                            
              CheckMsg(WM_KEYLAST               ) //0x0108
          #endif // _WIN32_WINNT >= 0x0501 
          #if(WINVER >= 0x0400)            
              CheckMsg(WM_IME_STARTCOMPOSITION  ) //0x010D
              CheckMsg(WM_IME_ENDCOMPOSITION    ) //0x010E
              CheckMsg(WM_IME_COMPOSITION       ) //0x010F
              CheckMsg(WM_IME_KEYLAST           ) //0x010F
          #endif // WINVER >= 0x0400            
          CheckMsg(WM_INITDIALOG                ) //0x0110
          CheckMsg(WM_COMMAND                   ) //0x0111
          CheckMsg(WM_SYSCOMMAND                ) //0x0112
          CheckMsg(WM_TIMER                     ) //0x0113
          CheckMsg(WM_HSCROLL                   ) //0x0114
          CheckMsg(WM_VSCROLL                   ) //0x0115
          CheckMsg(WM_INITMENU                  ) //0x0116
          CheckMsg(WM_INITMENUPOPUP             ) //0x0117
          CheckMsg(WM_MENUSELECT                ) //0x011F
          CheckMsg(WM_MENUCHAR                  ) //0x0120
          CheckMsg(WM_ENTERIDLE                 ) //0x0121
          #if(WINVER >= 0x0500)                 
              #ifndef _WIN32_WCE                
                  CheckMsg(WM_MENURBUTTONUP     ) //0x0122
                  CheckMsg(WM_MENUDRAG          ) //0x0123
                  CheckMsg(WM_MENUGETOBJECT     ) //0x0124
                  CheckMsg(WM_UNINITMENUPOPUP   ) //0x0125
                  CheckMsg(WM_MENUCOMMAND       ) //0x0126
                  #ifndef _WIN32_WCE
                      #if(_WIN32_WINNT >= 0x0500)
                          CheckMsg(WM_CHANGEUISTATE  ) //0x0127
                          CheckMsg(WM_UPDATEUISTATE  ) //0x0128
                          CheckMsg(WM_QUERYUISTATE   ) //0x0129
                      #endif // _WIN32_WINNT >= 0x0500
                  #endif
              #endif
          #endif // WINVER >= 0x0500 
          CheckMsg(WM_CTLCOLORMSGBOX          ) //0x0132
          CheckMsg(WM_CTLCOLOREDIT            ) //0x0133
          CheckMsg(WM_CTLCOLORLISTBOX         ) //0x0134
          CheckMsg(WM_CTLCOLORBTN             ) //0x0135
          CheckMsg(WM_CTLCOLORDLG             ) //0x0136
          CheckMsg(WM_CTLCOLORSCROLLBAR       ) //0x0137
          CheckMsg(WM_CTLCOLORSTATIC          ) //0x0138
          //CheckMsg(WM_MOUSEFIRST              ) //0x0200
          CheckMsg(WM_MOUSEMOVE               ) //0x0200
          CheckMsg(WM_LBUTTONDOWN             ) //0x0201
          CheckMsg(WM_LBUTTONUP               ) //0x0202
          CheckMsg(WM_LBUTTONDBLCLK           ) //0x0203
          CheckMsg(WM_RBUTTONDOWN             ) //0x0204
          CheckMsg(WM_RBUTTONUP               ) //0x0205
          CheckMsg(WM_RBUTTONDBLCLK           ) //0x0206
          CheckMsg(WM_MBUTTONDOWN             ) //0x0207
          CheckMsg(WM_MBUTTONUP               ) //0x0208
          CheckMsg(WM_MBUTTONDBLCLK           ) //0x0209
          #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
              CheckMsg(WM_MOUSEWHEEL          ) //0x020A
          #endif
          #if (_WIN32_WINNT >= 0x0500)
              CheckMsg(WM_XBUTTONDOWN         ) //0x020B
              CheckMsg(WM_XBUTTONUP           ) //0x020C
              CheckMsg(WM_XBUTTONDBLCLK       ) //0x020D
          #endif
          #if (_WIN32_WINNT >= 0x0500)
              CheckMsg(WM_MOUSELAST           ) //0x020D
          #elif (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
              CheckMsg(WM_MOUSELAST           ) //0x020A
          #else
              CheckMsg(WM_MOUSELAST           ) //0x0209
          #endif // (_WIN32_WINNT >= 0x0500) 
          CheckMsg(WM_PARENTNOTIFY            ) //0x0210
          CheckMsg(WM_ENTERMENULOOP           ) //0x0211
          CheckMsg(WM_EXITMENULOOP            ) //0x0212
          #if(WINVER >= 0x0400)
              CheckMsg(WM_NEXTMENU            ) //0x0213
              CheckMsg(WM_SIZING              ) //0x0214
              CheckMsg(WM_CAPTURECHANGED      ) //0x0215
              CheckMsg(WM_MOVING              ) //0x0216
          #endif // WINVER >= 0x0400 
          #if(WINVER >= 0x0400)
              CheckMsg(WM_POWERBROADCAST      ) //0x0218
          #endif // WINVER >= 0x0400 
          #if(WINVER >= 0x0400)
              CheckMsg(WM_DEVICECHANGE        ) //0x0219
          #endif // WINVER >= 0x0400 
          CheckMsg(WM_MDICREATE               ) //0x0220
          CheckMsg(WM_MDIDESTROY              ) //0x0221
          CheckMsg(WM_MDIACTIVATE             ) //0x0222
          CheckMsg(WM_MDIRESTORE              ) //0x0223
          CheckMsg(WM_MDINEXT                 ) //0x0224
          CheckMsg(WM_MDIMAXIMIZE             ) //0x0225
          CheckMsg(WM_MDITILE                 ) //0x0226
          CheckMsg(WM_MDICASCADE              ) //0x0227
          CheckMsg(WM_MDIICONARRANGE          ) //0x0228
          CheckMsg(WM_MDIGETACTIVE            ) //0x0229
          CheckMsg(WM_MDISETMENU              ) //0x0230
          CheckMsg(WM_ENTERSIZEMOVE           ) //0x0231
          CheckMsg(WM_EXITSIZEMOVE            ) //0x0232
          CheckMsg(WM_DROPFILES               ) //0x0233
          CheckMsg(WM_MDIREFRESHMENU          ) //0x0234
          #if(WINVER >= 0x0400)                
              CheckMsg(WM_IME_SETCONTEXT      ) //0x0281
              CheckMsg(WM_IME_NOTIFY          ) //0x0282
              CheckMsg(WM_IME_CONTROL         ) //0x0283
              CheckMsg(WM_IME_COMPOSITIONFULL ) //0x0284
              CheckMsg(WM_IME_SELECT          ) //0x0285
              CheckMsg(WM_IME_CHAR            ) //0x0286
          #endif // WINVER >= 0x0400          
          #if(WINVER >= 0x0500)               
              CheckMsg(WM_IME_REQUEST         ) //0x0288
          #endif // WINVER >= 0x0500          
          #if(WINVER >= 0x0400)               
              CheckMsg(WM_IME_KEYDOWN         ) //0x0290
              CheckMsg(WM_IME_KEYUP           ) //0x0291
          #endif // WINVER >= 0x0400 
          #if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
              CheckMsg(WM_MOUSEHOVER          ) //0x02A1
              CheckMsg(WM_MOUSELEAVE          ) //0x02A3
          #endif
          #if(WINVER >= 0x0500)
              CheckMsg(WM_NCMOUSEHOVER        ) //0x02A0
              CheckMsg(WM_NCMOUSELEAVE        ) //0x02A2
          #endif // WINVER >= 0x0500 
          #if(_WIN32_WINNT >= 0x0501)
              CheckMsg(WM_WTSSESSION_CHANGE   ) //0x02B1
              CheckMsg(WM_TABLET_FIRST        ) //0x02c0
              CheckMsg(WM_TABLET_LAST         ) //0x02df
          #endif // _WIN32_WINNT >= 0x0501
          CheckMsg(WM_CUT                     ) //0x0300
          CheckMsg(WM_COPY                    ) //0x0301
          CheckMsg(WM_PASTE                   ) //0x0302
          CheckMsg(WM_CLEAR                   ) //0x0303
          CheckMsg(WM_UNDO                    ) //0x0304
          CheckMsg(WM_RENDERFORMAT            ) //0x0305
          CheckMsg(WM_RENDERALLFORMATS        ) //0x0306
          CheckMsg(WM_DESTROYCLIPBOARD        ) //0x0307
          CheckMsg(WM_DRAWCLIPBOARD           ) //0x0308
          CheckMsg(WM_PAINTCLIPBOARD          ) //0x0309
          CheckMsg(WM_VSCROLLCLIPBOARD        ) //0x030A
          CheckMsg(WM_SIZECLIPBOARD           ) //0x030B
          CheckMsg(WM_ASKCBFORMATNAME         ) //0x030C
          CheckMsg(WM_CHANGECBCHAIN           ) //0x030D
          CheckMsg(WM_HSCROLLCLIPBOARD        ) //0x030E
          CheckMsg(WM_QUERYNEWPALETTE         ) //0x030F
          CheckMsg(WM_PALETTEISCHANGING       ) //0x0310
          CheckMsg(WM_PALETTECHANGED          ) //0x0311
          CheckMsg(WM_HOTKEY                  ) //0x0312
          #if(WINVER >= 0x0400)
              CheckMsg(WM_PRINT               ) //0x0317
              CheckMsg(WM_PRINTCLIENT         ) //0x0318
          #endif // WINVER >= 0x0400 
          #if(_WIN32_WINNT >= 0x0500)
              CheckMsg(WM_APPCOMMAND          ) //0x0319
          #endif // _WIN32_WINNT >= 0x0500
          #if(_WIN32_WINNT >= 0x0501)
              CheckMsg(WM_THEMECHANGED        ) //0x031A
          #endif // _WIN32_WINNT >= 0x0501
          #if(WINVER >= 0x0400)
              CheckMsg(WM_HANDHELDFIRST       ) //0x0358
              CheckMsg(WM_HANDHELDLAST        ) //0x035F
              CheckMsg(WM_AFXFIRST            ) //0x0360
              CheckMsg(WM_AFXLAST             ) //0x037F
          #endif // WINVER >= 0x0400 
          CheckMsg(WM_PENWINFIRST             ) //0x0380
          CheckMsg(WM_PENWINLAST              ) //0x038F
          #if(WINVER >= 0x0400)
              CheckMsg(WM_APP                 ) //0x8000
          #endif // WINVER >= 0x0400 
          CheckMsg(WM_USER                    ) //0x0400

          if (!szOut[0])
              strcpy(szOut, "[UNKNOWN]");
      //#endif
  }
#endif


long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                WPARAM wParam, LPARAM lParam )
{
    //PAINTSTRUCT ps;
    //RECT        rc;
    //SIZE        size;
    //BYTE phase = 0;
    HDC         hdc;
    HRESULT        hr, ddrval;
    char lpstrCommand[512];
    char szMCIAnswer[128];

    #if _DEBUG
    {
        char szMsg[256];
        szMsg[0] = 0;
        PrintWindowsMessage(message, szMsg);

        char buf[512]; 
        sprintf(buf, "frame %d: %s, msg=0x%x, wParam=0x%x, lParam=0x%x", intframe, szMsg, message, wParam, lParam);
        dumpmsg(buf);
    }
    #endif


    switch( message )
    {
        /*
        case WM_ACTIVATEAPP:
            dumpmsg("WM_ACTIVATEAPP received");
#if PLUGIN
            //bActive = wParam;
            if ((BOOL)wParam==FALSE && !bUserQuit && intframe>20)
            {
                dumpmsg("closing on WM_ACTIVATEAPP");
                g_QuitASAP = true;
            }
#endif
#if SAVER
            //if (g_QuitASAP) dumpmsg("  [g_QuitASAP]");
            //if ((BOOL)wParam==FALSE) dumpmsg("  [wParam false] (deactivate)");
            
            // note: with ALT-TAB, you get a WM_KILLFOCUS and 2 WM_ACTIVATEAPPs, the last 
            // of which is NOT gobbled.  CTRL-ESC is making that last one get gobbled!
            //if (g_QuitASAP) dumpmsg("  [g_QuitASAP]"); else dumpmsg("  [do not g_QuitASAP]");
            //if ((BOOL)wParam) dumpmsg("  [activate]"); else dumpmsg("  [deactivate]");
            if ((BOOL)wParam==FALSE && !g_QuitASAP) 
            {
                dumpmsg("  {gobbled}");
                return 0;
            }
#endif
                break;


        case WM_ACTIVATE:
            dumpmsg("WM_ACTIVATE received");
#if PLUGIN
            if (LOWORD(wParam)==WA_INACTIVE && !bUserQuit && intframe>20)
            {
                dumpmsg("closing on WM_ACTIVATE");
                g_QuitASAP = true;
            }
#endif
#if SAVER
            if ((BOOL)LOWORD(wParam)==FALSE && !g_QuitASAP) 
            {
                dumpmsg("  {gobbled}");
                return 0;
            }
#endif
            break;
            */

/*
#if SAVER
        case WM_PAINT:
            return 0;
            break;
#endif
*/
      /*
    case WM_CANCELMODE:
      return FALSE;
      break;
      */


        case WM_NCACTIVATE:
      //MULTIMON
      // When using multiple monitors, this prevents the plugin from exiting
      // when you click on another window.
      return FALSE;
            break;

        /* pre-password code
        case WM_KILLFOCUS:
            dumpmsg("WM_KILLFOCUS");
            if (!bUserQuit && intframe>20) 
            {
                dumpmsg("Quitting on WM_KILLFOCUS");
                g_QuitASAP = true;
#if PLUGIN
                PostMessage(hWnd, WM_CLOSE, 0, 0);
#endif
            }
            break;
        */
        case WM_KILLFOCUS:
            dumpmsg("WM_KILLFOCUS");
#if SAVER
            if (!g_QuitASAP)
            {
                dumpmsg("  {gobbled}");
                return 0;
            }
#endif
/*#if PLUGIN
            if (!bUserQuit && intframe>20) 
            {
                dumpmsg("Quitting on WM_KILLFOCUS");
                bUserQuit = true;
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
#endif*/
            break;

        case WM_CLOSE:
            dumpmsg("WM_CLOSE");

            //----------------------
#if SAVER
            if (!g_QuitASAP) return false;
            break;
            /*
            {
                bool CanClose = (intframe < 50) ? true : VerifyPassword(hWnd);
                if (CanClose)
                {
                    dumpmsg("Ok to close");
                    g_QuitASAP = true;
                    //DestroyWindow(hWnd);  // taken care of in finiObjects()
                }
                else
                {
                    dumpmsg("NOT ok to close");
                    return FALSE;
                }
            }
            */
#endif
            //----------------------
//#if SAVER
//          g_QuitASAP = true;
//            break;
//#endif
#if PLUGIN
            finiObjects();
            g_QuitASAP = true;
            break;
#endif

        case WM_DESTROY:
            /* pre-password code:
            dumpmsg("WM_DESTROY");
            g_QuitASAP = true;
#if PLUGIN
            PostQuitMessage(0); 
            return 0;
#endif
            */
            dumpmsg("WM_DESTROY");
            g_QuitASAP = true;
            PostQuitMessage(0); 
#if PLUGIN
            return 0;
#endif
            break;

        //cursor//
        //removed in 3.40
/*        case WM_SETCURSOR:      // called when mouse enters our client area
            dumpmsg("WM_SETCURSOR");
            if (!g_QuitASAP)       // condition new in v3.54 ... to fix Pwr Mgmt bug? 
            {
                SetCursor(NULL);
            }
        #if PLUGIN
            return TRUE;        // the #if PLUGIN used to be around the whole message.... now it's just around the return statement.
        #endif
            break;*/
            
        case WM_LBUTTONDOWN: 
        case WM_MBUTTONDOWN: 
        case WM_RBUTTONDOWN: 
            dumpmsg("closing on WM_XBUTTONDOWN");
            //bUserQuit = TRUE;  // only used for plugin
            #if PLUGIN
                //g_QuitASAP = true;  // just quit
            #endif
            #if SAVER
      bUserQuit = TRUE;  // only used for plugin
                //PostMessage(hWnd, WM_CLOSE, 0, 0);  // don't just quit... trigger password dialog
                TryToExit(hWnd);
            #endif
            break;


        case WM_MOUSEMOVE:
            #if SAVER
                SHOW_MOUSECLICK_MSG = 30;
                /*if (!g_QuitASAP && !g_PassDialogReq)       // condition new in v3.54 ... to fix Pwr Mgmt bug? 
                {
                    SetCursor(NULL);
                }*/
            #endif
            /*
            #if SAVER
                if (intframe > 50)
                {
                    if (last_mousemove_frame==intframe-1)
                    {
                        #if PLUGIN
                            PostMessage(hWnd, WM_CLOSE, 0, 0);  // trigger password dialog
                        #endif
                        #if SAVER
                            TryToExit(hWnd);
                        #endif
                        last_mousemove_frame = -5;
                    }
                    else
                    {
                        last_mousemove_frame = intframe;
                    }
                }
            #endif
            #if PLUGIN
            */
                {
                    int xPos;
                    int yPos;
                    xPos = LOWORD(lParam);  // horizontal position of cursor 
                    yPos = HIWORD(lParam);  // vertical position of cursor 
                    gXC = xPos;
                    gYC = yPos;
                    if (gXC < FXW/2-40) gXC = FXW/2-40;
                    if (gXC > FXW/2+40) gXC = FXW/2+40;
                    if (gYC < FXH/2-30) gYC = FXH/2-30;
                    if (gYC > FXH/2+30) gYC = FXH/2+30;
                    //POINT pt; GetCursorPos(&pt);
                }
            //#endif
            break;


#if SAVER

      /*
      //MULTIMON: forget this...
        case WM_SYSKEYDOWN:         // be careful!  just recently added!
        case WM_SYSKEYUP:           // be careful!  just recently added!
            // this protects against ALT-TAB and other ALT-keys
            // by bringing up the built-in password dialog to absorb them.
            
            // CTRL+ESC also shows up here, but we just throw up the
            // password dialog right when we see CTRL (wm_keydown, wparam=0x11)
            // and CTRL+ESC never has a chance to get to Geiss!  (eaten by pwdialog)
            if (intframe>0)  // sometimes saver gets WM_SYSKEYUP on frame 0 when it autostarts... then it exits right away... strange.  Hopefully this will fix it.
            {
                // otherwise, throw the password dialog box at them
                dumpmsg("bringing up password dialog on WM_SYSKEY????");
                bool CanClose = (intframe < 50) ? true : VerifyPassword(hWnd);
                dumpmsg("done with password dialog for WM_SYSKEY????");
                if (CanClose)
                {
                    dumpmsg("Ok to close");
                    g_QuitASAP = true;
                    //DestroyWindow(hWnd);
                }
                else
                {
                    dumpmsg("NOT ok to close");
                    return FALSE;
                }
            }
            break;
            */
#endif

#if PLUGIN
            /*
            //MULTIMON: nix these.  We should be able to ALT+TAB in and out w/o *exiting*.
        case WM_SYSKEYDOWN:         // be careful!  just recently added!
        case WM_SYSKEYUP:
            return 0;
            break;
            */
#endif

            /*
        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == SC_SCREENSAVE) 
            {
                //--on Ryan Bozis' Win95 machine, this message STOPS IT DEAD.
                dumpmsg("[blocking SC_SCREENSAVE]");
                return false;   // eat up the request to activate a screensaver
            }
            #if SAVER
                else if ((wParam & 0xFFF0) == SC_MONITORPOWER) 
                {
                    // here, the monitor won't actually shut off until the
                    // message is processed, BUT effective immediately,
                    // our directdraw surface is lost / screwed up.

                    if (!g_PowerSaveReq) // enter powersave mode
                    {
                        dumpmsg("Monitor wants to shut off... flagging a quit request.");
                        //g_QuitASAP = true;
                        g_PowerSaveReq = true;
                        while (!g_PowerSaveAck)
                        {
                            dumpmsg(".");
                            Sleep(1);
                        };
                        //Sleep(1000);
                        dumpmsg("PowerSaveAck acknowledged.");
                        
                        dumpmsg("...killing surfaces...");
                        #if (GRFX==1)
                            lpDDSPrimary->Release();
                            lpDDSPrimary = NULL;
                            //lpDDSBack->Release();
                            //lpDDSBack = NULL;
                        #endif
                    }
                    else  // exit powersave mode
                    {
                        dumpmsg("exiting powersave mode... ");

                        #if (GRFX==1)
                            // (Re)create the primary surface with 1 back buffer
                            DDSURFACEDESC ddsd;
                            ddsd.dwSize = sizeof( ddsd );
                            ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;// | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM; 
                            ddsd.dwBackBufferCount = 1;

                            HRESULT ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                            if( ddrval == DD_OK )
                            {
                                dumpmsg("  [OK]"); 
                                // Get a pointer to the back buffer
                                DDSCAPS ddscaps;
                                ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                                ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, &lpDDSBack);
                                if( ddrval == DD_OK ) dumpmsg("  [OK]"); else dumpmsg("  [ERROR]");
                            }
                            else dumpmsg("  [ERROR]");

                            // restore palette
                            lpDDSPrimary->SetPalette(lpDDPal);
                            //lpDDSBack->SetPalette(lpDDPal);
                            //iBlendsLeftInPal = 3;
                        #endif

                        // hide cursor
                        SetCursor(LoadCursor(NULL,IDC_ARROW));
                        SetCursor(NULL);

                        g_PowerSaveReq = false;
                        g_PowerSaveAck = false;
                    }
                }
            #endif
            else
            {
                dumpmsg("[misc. param to WM_SYSCOMMAND]");
                // DANGER ALERT!!!!!!!!!!!!
                // DANGER ALERT!!!!!!!!!!!!
                // DANGER ALERT!!!!!!!!!!!!
                //#if SAVER
                //    return 0;       // should this be here?!
                //#endif
            }
            break;

        case WM_POWERBROADCAST:
            dumpmsg("power broadcast message!");
            switch((DWORD)wParam)
            {
                case PBT_APMBATTERYLOW:             dumpmsg(" Battery power is low. "); break;
                case PBT_APMOEMEVENT:               dumpmsg(" OEM-defined event occurred. "); break;
                case PBT_APMPOWERSTATUSCHANGE:      dumpmsg(" Power status has changed. "); break;
                case PBT_APMQUERYSUSPEND:           dumpmsg(" Request for permission to suspend. "); break;
                case PBT_APMQUERYSUSPENDFAILED:     dumpmsg(" Suspension request denied. "); break;
                case PBT_APMRESUMEAUTOMATIC:        dumpmsg(" Operation resuming automatically after event. "); break;
                case PBT_APMRESUMECRITICAL:         dumpmsg(" Operation resuming after critical suspension. "); break;
                case PBT_APMRESUMESUSPEND:          dumpmsg(" Operation resuming after suspension. "); break;
                case PBT_APMSUSPEND:                dumpmsg(" System is suspending operation."); break; 
                default:                            dumpmsg(" unknown identifier."); break;
            }            
            break;
            */

/*
#if PLUGIN
        case WM_KEYDOWN:
        {
            // translate this into a WM_CHAR message
            MSG msg;
            msg.hwnd = hMainWnd;
            msg.lParam = lParam;
            msg.wParam = wParam;
            msg.message = message;
            
            
            //PostMessage(hMainWnd, message, wParam, lParam);

            TranslateMessage(&msg);
            DispatchMessage(&msg);
           
            return 0;
            break;
        }
#endif
*/

#if SAVER
        case WM_KEYDOWN:
            if (wParam==0x11)
            {
                // CTRL has been pressed
                // -> throw the password dialog box at them
                dumpmsg("bringing up password dialog on CTRL");
                bool CanClose = (intframe < 50) ? true : VerifyPassword(hWnd);
                dumpmsg("done with password dialog for CTRL");
                if (CanClose)
                {
                    dumpmsg("Ok to close");
                    g_QuitASAP = true;
                    //DestroyWindow(hWnd);
                }
                else
                {
                    dumpmsg("NOT ok to close");
                    return FALSE;
                }
            }
            break;
#endif


#if PLUGIN
        case WM_CHAR:
            switch(wParam)
            {
                case '>':
                case '.':
                    modeprefs[mode]++;
                    if (modeprefs[mode] > 5)
                        modeprefs[mode] = 5;
                    else
                    {
                        modeprefs_total = 0;
                        for (int i=1; i<=NUM_MODES; i++) modeprefs_total += modeprefs[i];
                    }
                    SHOW_MODEPREFS_MSG = 30;
                    WriteModePrefs();
                    break;
                case '<':
                case ',':
                    modeprefs[mode]--;
                    if (modeprefs[mode] < 0)
                        modeprefs[mode] = 0;
                    else
                    {
                        modeprefs_total = 0;
                        for (int i=1; i<=NUM_MODES; i++) modeprefs_total += modeprefs[i];
                    }
                    SHOW_MODEPREFS_MSG = 30;
                    WriteModePrefs();
                    break;

                case '[':
                    //load preset
                    ePresetState = LOAD_1;
                    break;

                case ']':
                    //save preset
                    ePresetState = SAVE_1;
                    break;

                /*
                case '!': 
                case '@':
                case '#':
                case '$':
                case '%':
                case '^':
                case '&':
                case '*':
                case '(':
                case ')':
                    if (ePresetState != NONE && wParam >= '0' && wParam <= '9')
                    {
                        // loading/saving a preset

                        switch( ePresetState )
                        {
                        case LOAD_1:
                            ePresetState = LOAD_2;
                            iPresetNum = wParam - '0';
                            break;
                        case LOAD_2:
                            iPresetNum = iPresetNum*10 + wParam - '0';
                            LoadPreset(iPresetNum);
                            ePresetState = NONE;
                            break;
                        case SAVE_1:
                            ePresetState = SAVE_2;
                            iPresetNum = wParam - '0';
                            break;
                        case SAVE_2:
                            iPresetNum = iPresetNum*10 + wParam - '0';
                            SavePreset(iPresetNum);
                            ePresetState = NONE;
                            break;
                        }
                    }
                    else // picking a new map
                    {
                        y_map_pos = -1;             // restart map calculations
                        //new_mode = wParam - '0';    // set new mode
                        //if (wParam == '0') new_mode = 10;    // special case
                        new_mode = char_to_mode[wParam & 0xFF];

                        g_rush_map = true;          // rush it
                        FX_Apply_Mode();

                        // reset fps counter:
                        time_array_pos = 0;
                        time_array_ready = false;
                    }
                    break;
                */

            }
            break;
#endif

#if PLUGIN
        case WM_KEYDOWN:
#endif
#if SAVER
        case WM_CHAR:
#endif
            switch( wParam )
            {
                case VK_TAB:
                    dumpmsg("TAB pressed!");
                    return 0;
                    break;

#if SAVER
                case 'o':
                case 'O':
                    if (SoundReady)
                    {
                        if (SoundActive)
                        {
                            hr = pDSCB->Stop();
                            SoundActive = FALSE;
                            if (hr != DS_OK) 
                            { 
                                dumpmsg("Failed to Stop sound capture on O press"); 
                                //g_QuitASAP = true; 
                                //exit(721); 
                            }
                        }
                        else
                        {
                            hr = pDSCB->Start(DSCBSTART_LOOPING);
                            SoundActive = TRUE;
                            frames_since_silence = 0;
                            if (hr != DS_OK) 
                            { 
                                dumpmsg("Failed to Start sound capture on O press"); 
                                //g_QuitASAP = true; 
                                //exit(720); 
                            }
                        }
                    }
                    break;
#endif
                case 'q':
                case 'Q':
                    effect[CHASERS] *= -1;
                    break;
                case 'W':
                case 'w':
                    //waveform--;
                    //if (waveform<0) waveform = NUM_WAVES;
                    //break;
                    waveform = (waveform + 1) % (NUM_WAVES+1);
                    break;
                case 'u':
                case 'U':
                    effect[BAR] *= -1;
                    break;
                //case 'a':
                case 'A':
                    effect[NUCLIDE] *= -1;
                    break;
                case 'e':
                case 'E':
                    effect[SHADE] *= -1;
                    break;
                //case 's':
                //case 'S':
                case 'Y':
                    effect[SOLAR] *= -1;
                    break;
                case 'g':
                case 'G':
                    effect[GRID] *= -1;
                    break;
                case 'D':
                case 'd':
                    effect[DOTS] *= -1;
                    break;

                    

                /*
                case 'm':
                case 'M':
                    MMX_ON = 1-MMX_ON;
                    start_clock = clock();
                    clock_debt = 0;
                    intframe = 0;
                    break;
                */

#if SAVER
                case 'z':   // prev
                case 'Z':
                    if (!g_bDisableCdControls)
                    {
                        CheckForNewCD();
                        SHOW_TRACK_MSG = 30;
                        //mciSendString("status cdaudio number of tracks",szMCIAnswer,128,NULL);
                        //sscanf(szMCIAnswer, "%d", &iNumTracks);
                    
                        iTrack--;
                        if (iTrack < 1) iTrack = iNumTracks;

                        mciSendString("stop cdaudio",NULL,NULL,NULL);
                        sprintf(lpstrCommand, "seek cdaudio to %d", iTrack);
                        mciSendString(lpstrCommand, NULL, NULL, NULL);
                        if (eDiscState == PLAYING) 
                            mciSendString("play cdaudio",NULL,NULL,NULL);
                    }
                    else if (eDiscState == BUSY)
                    {
                        SHOW_TRACK_MSG = 30;
                    }
                    break;
                
                case 'x':   // play
                case 'X':
                    if (!g_bDisableCdControls)
                    {
                        CheckForNewCD();

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
                            sprintf(lpstrCommand, "seek cdaudio to %d", iTrack);
                            mciSendString(lpstrCommand, NULL, NULL, NULL);
                            mciSendString("play cdaudio",NULL,NULL,NULL);
                        }
                    }
                    else if (eDiscState == BUSY)
                    {
                        SHOW_TRACK_MSG = 30;
                    }
                    break;
                
                case 'c':   // pause
                case 'C':
                    if (!g_bDisableCdControls)
                    {
                        if (eDiscState == PAUSED)
                        {
                            SHOW_TRACK_MSG = 30;
                            eDiscState = PLAYING;
                            mciSendString("play cdaudio",NULL,NULL,NULL);
                        }
                        else if (eDiscState == PLAYING)
                        {
                            SHOW_TRACK_MSG = 30;
                            eDiscState = PAUSED;
                            mciSendString("pause cdaudio wait",NULL,NULL,NULL);
                        }
                    }
                    else if (eDiscState == BUSY)
                    {
                        SHOW_TRACK_MSG = 30;
                    }
                    break;
                
                case 'v':   // stop
                case 'V':
                    if (!g_bDisableCdControls)
                    {
                        CheckForNewCD();
                        SHOW_TRACK_MSG = 30;
                        eDiscState = STOPPED;
                        mciSendString("stop cdaudio wait",NULL,NULL,NULL);
                        //mciSendString("close cdaudio",NULL,NULL,NULL);
                    }
                    else if (eDiscState == BUSY)
                    {
                        SHOW_TRACK_MSG = 30;
                    }
                    break;
                
                case 'b':   // next
                case 'B':
                    if (!g_bDisableCdControls)
                    {
                        CheckForNewCD();
                        SHOW_TRACK_MSG = 30;
                        //mciSendString("status cdaudio number of tracks",szMCIAnswer,128,NULL);
                        //sscanf(szMCIAnswer, "%d", &iNumTracks);

                        iTrack++;
                        if (iTrack > iNumTracks) iTrack = 1;

                        mciSendString("stop cdaudio",NULL,NULL,NULL);
                        sprintf(lpstrCommand, "seek cdaudio to %d", iTrack);
                        mciSendString(lpstrCommand, NULL, NULL, NULL);
                        if (eDiscState == PLAYING) 
                            mciSendString("play cdaudio",NULL,NULL,NULL);
                    }
                    else if (eDiscState == BUSY)
                    {
                        SHOW_TRACK_MSG = 30;
                    }
                    
                    break;
#endif





                    //mciSendString('set cdaudio door open',NULL,NULL,NULL);
                    //mciSendString('set cdaudio door closed',NULL,NULL,NULL);

                    //wsprintf(lpstrCommand, "play cdaudio from 1 to 1");
                    //mciSendString(lpstrCommand, NULL, 0, NULL); 
                    //char lpszReturnString[1024];
                    //mciSendString("play cdaudio from 6 to 7", lpszReturnString, 
                    //    1024, NULL);


                case 'I':
                case 'i':
                    g_bSlideShift = !g_bSlideShift;
                    if (!g_bSlideShift) { slider1 = 0; }
                    break;

                case 'H':
                case 'h':
                case '?':
                    SHOW_HELP_MSG = !SHOW_HELP_MSG;
                    if (!SHOW_HELP_MSG) clearframes = max(clearframes, 2);
                    break;

                    /*
                case 'r':
                case 'R':
                    clearframes = max(clearframes, 2);
                    break;
                    */

                case 'l':
                case 'L':
                    sprintf(szLM, " - screen is LOCKED - ");
                    sprintf(szULM, " - screen is unlocked - ");
                    if (bLocked)
                    {
                        SHOW_LOCKED_MSG = 0;
                        SHOW_UNLOCKED_MSG = 30;
                    }
                    else
                    {
                        SHOW_LOCKED_MSG = 30;
                        SHOW_UNLOCKED_MSG = 0;
                    }
                    bLocked = !bLocked;
                    break;

#if PLUGIN
                case ' ':
                    //FX_Loadimage("c:\\fx.tga");
                    
                    g_hit = (g_song_tooltip_frames + 1)*fps_at_last_mode_switch/30.0f;
                    g_title_R = 128 + rand() % 99;
                    g_title_G = 128 + rand() % 99;
                    g_title_B = 128 + rand() % 99;
                    
                    break;
#endif


#if SAVER
                //case ' ':
                    //FX_Screenshot(intframe);
                    //break;

                    //g_Capturing = !g_Capturing;
                    //break;
#endif
                ///AMDTEST
                ///#if AMDTEST
                ///case 'y':
                ///case 'Y':
                    ///procmode = (procmode+1) % 4;
                    ///intframe = 0;
                    ///core_clock_time = 0;
                    ///break;
                ///#endif
                ///AMDTEST

#if PLUGIN
                case 's':
                case 'S':
                    if (SendMessage(this_mod_hwndParent,WM_USER,0,250))
                        sprintf(szMisc, " shuffle is now OFF ");    // shuffle was on
                    else
                        sprintf(szMisc, " shuffle is now ON ");    // shuffle was off
                    SHOW_MISC_MSG = 30;

                    // toggle shuffle
                    PostMessage(this_mod_hwndParent,WM_COMMAND,40023,0);
                    return 0;
                case 'r':
                case 'R':
                    if (SendMessage(this_mod_hwndParent,WM_USER,0,251))
                        sprintf(szMisc, " repeat is now OFF ");    // shuffle was on
                    else
                        sprintf(szMisc, " repeat is now ON ");    // shuffle was off
                    SHOW_MISC_MSG = 30;

                    // toggle repeat
                    PostMessage(this_mod_hwndParent,WM_COMMAND,40022,0);
                    return 0;
#endif

                case 'n':
                case 'N':
                    // rush the map calculations:
                    g_rush_map = true;
                    FX_Apply_Mode();

                    // reset fps counter:
                    time_array_pos = 0;
                    time_array_ready = false;

                    //FX_Pick_Random_Mode();
                    //FX_Apply_Mode();
                    //FX_Random_Palette();
                    break;

                case 'p':
                case 'P':
                    FX_Random_Palette();
                    //PutPalette();
                    break;

#if SAVER
                case '>':
                case '.':
                    modeprefs[mode]++;
                    if (modeprefs[mode] > 5)
                        modeprefs[mode] = 5;
                    else
                    {
                        modeprefs_total = 0;
                        for (int i=1; i<=NUM_MODES; i++) modeprefs_total += modeprefs[i];
                    }
                    SHOW_MODEPREFS_MSG = 30;
                    WriteModePrefs();
                    break;
                case '<':
                case ',':
                    modeprefs[mode]--;
                    if (modeprefs[mode] < 0)
                        modeprefs[mode] = 0;
                    else
                    {
                        modeprefs_total = 0;
                        for (int i=1; i<=NUM_MODES; i++) modeprefs_total += modeprefs[i];
                    }
                    SHOW_MODEPREFS_MSG = 30;
                    WriteModePrefs();
                    break;
#endif

                case '[':
                    //load preset
                    ePresetState = LOAD_1;
                    break;

                case ']':
                    //save preset
                    ePresetState = SAVE_1;
                    break;

                case 'f':
                case 'F':
                    SHOW_FPS = !SHOW_FPS;
                    //SHOW_TITLE = -1;
                    break;

                case 't':
                case 'T':
                    sprintf(szLM, " - palette is LOCKED - ");
                    sprintf(szULM, " - palette is unlocked - ");
                    if (bPalLocked)
                    {
                        SHOW_LOCKED_MSG = 0;
                        SHOW_UNLOCKED_MSG = 30;
                    }
                    else
                    {
                        SHOW_LOCKED_MSG = 30;
                        SHOW_UNLOCKED_MSG = 0;
                    }
                    bPalLocked = !bPalLocked;
                    break;

                
                case 'k':
                case 'K':
                    if (volpos < 20)
                    {
                        volpos++;
                        volscale *= 1.25;
                    }
                    break;

                case 'J':
                case 'j':
                    if (volpos > 0)
                    {
                        volpos--;
                        volscale *= 0.8;
                    }
                    break;
                

                /*
                case 'j':
                case 'J':
                    damping *= 1.1f;
                    new_damping *= 1.1f;
                    SHOW_DEBUG = 20;
                    debug_param = new_damping;
                    break;
                case 'k':
                case 'K':
                    damping /= 1.1f;
                    new_damping /= 1.1f;
                    SHOW_DEBUG = 20;
                    debug_param = new_damping;
                    break;
                */


                /*
                case 'j':
                case 'J':
                    debug_param -= 10;
                    break;
                case 'k':
                case 'K':
                    debug_param += 10;
                    break;
                */

                case 'm':
                case 'M':
                    //SHOW_DEBUG *= -1;
                    eCustomMsgState = LOAD_1;
                    break;

                case '1': 
                case '2': 
                case '3': 
                case '4': 
                case '5': 
                case '6': 
                case '7': 
                case '8': 
                case '9': 
                case '0':
                    if (eCustomMsgState != NONE)// && wParam >= '0' && wParam <= '9')
                    {
                        switch( eCustomMsgState )
                        {
                        case LOAD_1:
                            eCustomMsgState = LOAD_2;
                            iCustomMsgNum = wParam - '0';
                            break;
                        case LOAD_2:
                            iCustomMsgNum = iCustomMsgNum*10 + wParam - '0';
                            eCustomMsgState = NONE;
                            g_LastMessageWasCustom = true;
                            if (LoadCustomMsg(iCustomMsgNum));  // sets g_song_title[256]
                            {
                                g_hit = (g_song_tooltip_frames + 1)*fps_at_last_mode_switch/30.0f;
                            }
                            break;
                        }
                    }
                    else if (ePresetState != NONE)// && wParam >= '0' && wParam <= '9')
                    {
                        // loading/saving a preset

                        switch( ePresetState )
                        {
                        case LOAD_1:
                            ePresetState = LOAD_2;
                            iPresetNum = wParam - '0';
                            break;
                        case LOAD_2:
                            iPresetNum = iPresetNum*10 + wParam - '0';
                            LoadPreset(iPresetNum);
                            ePresetState = NONE;
                            break;
                        case SAVE_1:
                            ePresetState = SAVE_2;
                            iPresetNum = wParam - '0';
                            break;
                        case SAVE_2:
                            iPresetNum = iPresetNum*10 + wParam - '0';
                            SavePreset(iPresetNum);
                            ePresetState = NONE;
                            break;
                        }
                    }
                    else // picking a new map
                    {
                        if (eModeSelState == NONE_SEL)
                        {
                            eModeSelState = MODE_SEL_1;
                            iModeSelection = (wParam & 0xFF) - '0';
                        }
                        else
                        {
                            eModeSelState = NONE_SEL;
                            iModeSelection *= 10;
                            iModeSelection += (wParam & 0xFF) - '0';

                            if (iModeSelection >= 1 &&
                                iModeSelection <= NUM_MODES)
                            {
                                y_map_pos = -1;             // restart map calculations
                                new_mode = iModeSelection;//char_to_mode[wParam & 0xFF];

                                g_rush_map = true;          // rush it
                                FX_Apply_Mode();

                                // reset fps counter:
                                time_array_pos = 0;
                                time_array_ready = false;
                            }
                        }
                    }
                    break;

                case VK_ESCAPE:
                    if (eModeSelState != NONE_SEL)
                    {
                        eModeSelState = NONE_SEL;
                    }
                    else if (ePresetState != NONE)
                    {
                        ePresetState = NONE;
                    }
                    else if (eCustomMsgState != NONE)
                    {
                        eCustomMsgState = NONE;
                    }
                    else
                    {
                        /* pre-password code
                        dumpmsg("closing on WM_KEYDOWN/VK_ESCAPE");
                        bUserQuit = TRUE;
                        //PostMessage(hWnd, WM_CLOSE, 0, 0);
                        g_QuitASAP = true;
                        #if PLUGIN
                            PostMessage(hWnd, WM_CLOSE, 0, 0);
                        #endif
                        break;
                        */
                        dumpmsg("WM_KEYDOWN/VK_ESCAPE");
                        bUserQuit = TRUE;   // only used for plugin
                        #if PLUGIN
                            PostMessage(hWnd, WM_CLOSE, 0, 0);
                        #endif
                        #if SAVER
                            TryToExit(hWnd);
                        #endif
                        //g_QuitASAP = true;
                        //#if PLUGIN
                        //    PostMessage(hWnd, WM_CLOSE, 0, 0);
                        //#endif
                    }
                    break;

#if PLUGIN
                default:
                    dumpmsg("  <WM_KEYDOWN>");
                    PostMessage(this_mod_hwndParent,message,wParam,lParam);
                    return 0;
                    break;
#endif
            }
            break;
#if PLUGIN
        case WM_KEYUP:
            dumpmsg("  <WM_KEYUP>");
            PostMessage(this_mod_hwndParent,message,wParam,lParam);
            return 0;
            break;
#endif

        
    }

    return DefWindowProc(hWnd, message, wParam, lParam);

} /* WindowProc */





/*
#if PLUGIN
long FAR PASCAL WindowProc2( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    dumpmsg("  <windowproc2 called>");

    switch (message)
    {
        case WM_KEYDOWN: // pass keyboard messages to main winamp window (for processing)
        case WM_KEYUP:
            // get this_mod from our window's user data
            //winampVisModule *this_mod = (winampVisModule *) GetWindowLong(hwnd,GWL_USERDATA);
            //PostMessage(this_mod->hwndParent,message,wParam,lParam);
            dumpmsg("  <WM_KEYDOWN or WM_KEYUP was sent>");
            PostMessage(this_mod_hwndParent,message,wParam,lParam);
            return 0;
            break;
        case WM_DESTROY:
            dumpmsg("  <WM_DESTROY was sent to WindowProc2>");
            g_QuitASAP = true;
            PostQuitMessage(0);
            return 0;
            break;

    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif
*/


/*
BOOL CALLBACK DialogProc(HWND  hwndDlg, UINT  uMsg, WPARAM  wParam, LPARAM  lParam)
{
    intframe = 10000;
    while (1);
/*
    char buf[120];
    wsprintf(buf, "speaking from DialogProc.");
    MessageBox( hwndDlg, buf, "NOTICE", MB_OK );

  switch(uMsg)
  {
    //case WM_INITDIALOG:
    //    return TRUE;

    case WM_COMMAND:
         if(wParam==IDOK)
         {
           EndDialog(hwndDlg, IDOK);
           return TRUE;
         }
  }

  return FALSE;

}
*/

//BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, DWORD wParam, LONG lParam) 
   //HWND  hDlg; 
   //UINT  message; 
   //DWORD wParam; 
   //LONG  lParam; 
/**
   { 
        //char buf[80];
        //wsprintf(buf, "Thank you for trying FX.");
        //MessageBox( hDlg, buf, "ERROR", MB_OK );
       
       HWND hSpeed; /* handle to speed scroll bar */ 
/**       HWND hOK;    /* handle to OK push button */ 
/**    
       switch(message) 
       { 
           case WM_INITDIALOG: 
                  
               /* Retrieve the application name from the .rc file. */ 
    
               //LoadString(hMainInstance, idsAppName, szAppName, 40); 
    
               /* Retrieve the .ini (or registry) file name. */ 
    
               //LoadString(hMainInstance, idsIniFile, szIniFile, 
               //    MAXFILELEN); 
    
               /* Retrieve any redraw speed data from the registry. */ 
/**    
               lSpeed = GetPrivateProfileInt(szAppName, szRedrawSpeed, 
                   DEFVEL, szIniFile); 
    
               /* 
                * If the initialization file does not contain an entry 
                * for this screen saver, use the default value. 
                */ 
/**    
               if(lSpeed > MAXVEL || lSpeed < MINVEL) 
                   lSpeed = DEFVEL; 
    
               /* Initialize the redraw speed scroll bar control. */ 
/**    
               hSpeed = GetDlgItem(hDlg, ID_SPEED); 
               //hSpeed = GetDlgItem(hDlg, 5); 
               SetScrollRange(hSpeed, SB_CTL, MINVEL, MAXVEL, FALSE); 
               SetScrollPos(hSpeed, SB_CTL, lSpeed, TRUE); 
    
               /* Retrieve a handle to the OK push button control. */ 
/**    
               hOK = GetDlgItem(hDlg, ID_OK); 
    
               return TRUE; 
    
           case WM_HSCROLL: 
    
               /* 
                * Process scroll bar input, adjusting the lSpeed 
                * value as appropriate. 
                */ 
/**    
               switch (LOWORD(wParam)) 
                   { 
                       case SB_PAGEUP: 
                           --lSpeed; 
                       break; 
    
                       case SB_LINEUP: 
                           --lSpeed; 
                       break; 
    
                       case SB_PAGEDOWN: 
                           ++lSpeed; 
                       break; 
    
                       case SB_LINEDOWN: 
                           ++lSpeed; 
                       break; 
    
                       case SB_THUMBPOSITION: 
                           lSpeed = HIWORD(wParam); 
                       break; 
    
                       case SB_BOTTOM: 
                           lSpeed = MINVEL; 
                       break; 
    
                       case SB_TOP: 
                           lSpeed = MAXVEL; 
                       break; 
    
                       case SB_THUMBTRACK: 
                       case SB_ENDSCROLL: 
                           return TRUE; 
                       break; 
                   } 
                   if ((int) lSpeed <= MINVEL) 
                       lSpeed = MINVEL; 
                   if ((int) lSpeed >= MAXVEL) 
                       lSpeed = MAXVEL; 
    
                   SetScrollPos((HWND) lParam, SB_CTL, lSpeed, TRUE); 
               break; 
    
           case WM_COMMAND: 
               switch(LOWORD(wParam)) 
               { 
                   case ID_OK: 
    
                      /* 
                       * Write the current redraw speed variable to 
                       * the .ini file. 
                       */ 
/**    
                       wsprintf(szTemp, "%ld", lSpeed); 
                       WritePrivateProfileString(szAppName, szRedrawSpeed, 
                           szTemp, szIniFile); 
    
                   case ID_CANCEL: 
                       EndDialog(hDlg, LOWORD(wParam) == ID_OK); 
                   return TRUE; 
               } 
       } 
       return FALSE; 
   } 
*/

BOOL WINAPI RegisterDialogClasses(HANDLE hInst) 
{ 
       return TRUE; 
} 




bool Try_To_Recover()
{
    // returns true if we successfully recovered.

    HRESULT ddrval;
    
    // if we ALT+TABbed out of the app, just hang out until we can restore our surfaces...
    ddrval = lpDDSPrimary->Restore();
    if (ddrval == DD_OK)
        ddrval = lpDDSBack->Restore();

    if (ddrval != DD_OK)
    {
      Sleep(200);
      return false;
    }

    PutPalette();

    return true;
      



  /*
    dumpmsg("Try_To_Recover() called...");

    if( ddrval == DD_OK )
    {
        dumpmsg("  oops!");
        return; //oops!
    }



#if SAVER

    g_DDRAW_ERRORS++;
    if (g_DDRAW_ERRORS > 1000)
    {
        dumpmsg("DDRAW FRAGGED... upping frame # by 1000 so passwords take effect.");
        g_DDRAW_FRAGGED_req = true;
        intframe += 1000;
    }
    return;

    /*
    while (ddrval != DD_OK) 
    {
        Sleep(100);
        ddrval = lpDDS->Restore();
    }
    return;


    //if (bComingFromThread)      //3.40
    //{
        //dumpmsg("Killing thread via _endthread...");
        //_endthread();

        //--------------------------------------------------------------
        // THE FOLLOWING FIVE LINES ARE VITAL TO SAFE HANDLING OF
        // THE ALT+TAB and CTRL+ESC KEY COMBINATIONS.  DO NOT MODIFY!
        //--------------------------------------------------------------
        dumpmsg(" Calling finiObjects immediately...");
        finiObjects();
        dumpmsg(" finiObjects done - exiting");
        g_QuitASAP = true;
        exit(0);
        //--------------------------------------------------------------
    //}
    */
  /*
#endif

#if PLUGIN
    dumpmsg(" Calling finiObjects immediately...");
    finiObjects();
    dumpmsg(" finiObjects done - exiting");
    g_QuitASAP = true;
    //exit(0);
    PostMessage(hMainWnd, WM_CLOSE, 0, 0);
#endif

    dumpmsg("end of Try_To_Recover()");
    g_QuitASAP = true;
    //exit(97);
  */

}

//void RestoreCtrlAlt( void )
void __cdecl RestoreCtrlAlt(void)
{
    //if (fOnWin95)
    //{
        // Local reboot and hotkey control (on Win95) 
        BOOL dummy;
        SystemParametersInfo( SPI_SCREENSAVERRUNNING, FALSE, &dummy, 0 );
    //}
}


void GetWaveData()
{
    float fDiv = (1.0/(64.0 * (640.0/(float)FXW) ));
    float billy = volscale * fDiv;
    #ifdef SAVER
        //added in 4.25:
        billy *= 4.0f;  // because some microphones just can't be cranked up any higher
    #endif
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
        pDSCB->GetCurrentPosition(&a, &b);    // record pos. (changes always), playback pos. (sometimes sticks)

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

        //for trigger:
        BUFSIZE             =max(FXW*3, MINBUFSIZE);

        if (b==oldSoundBufPos)
        {
            if (oldSoundBufNum<0) oldSoundBufNum++;
            offset = ((BUFSIZE))*oldSoundBufNum;
        }
        else
        {
            oldSoundBufNum = -3;        // was -2 in first v3.02 release
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

        // level trigger:
        int v, old_v;
        int FXW_DIV_2 = FXW/2;    // centers the trigger point (rather than @ left)
        for (i=8; i<FXW/2; i+=2)
        {
            old_v = g_SoundBuffer[i + FXW_DIV_2 - 8];
            v     = g_SoundBuffer[i + FXW_DIV_2];
            if ( 
                (abs(    v - last_frame_v    ) <= 256)     // height-match
                &&
                (last_frame_slope*(v-old_v) >= 0)        // slope-match
               )
            {
                last_frame_v = v;
                last_frame_slope = v - old_v;
                break;
            }
        }

        //i = FXW-2;

        if (i < FXW/2)      // trigger.
        {
            //char buf[64];
            //sprintf(buf, "trigger: frame=%d, pos=%d", intframe, i);
            //dumpmsg(buf);
            int iShift = i;
            for (i=iShift; i<BUFSIZE; i++)
            {
                g_SoundBuffer[i-iShift] = g_SoundBuffer[i];
            }
        }
        else    // no trigger
        {
            old_v = g_SoundBuffer[FXW_DIV_2 + 0];
            v     = g_SoundBuffer[FXW_DIV_2 + 8];
            last_frame_v = v;
            last_frame_slope = v - old_v;
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
                    (last_frame_slope*(v-old_v) >= 0 )
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
            /*
            for ( ; x<BUFSIZE && y<512; x+=2, y++)
            {
                g_SoundBuffer[x] = ((int)(256-g_this_mod->spectrumData[0][y]) ) << 10;
                g_SoundBuffer[x+1] = ((int)(256-g_this_mod->spectrumData[1][y]) ) << 10;
            }
            */
            __int16 nL, nR;
            int decamt = 2200;//2000;
            for (x=0, y=0; x<BUFSIZE && y<512; x+=2, y++)
            {
                nL = (((int)(256-g_this_mod->spectrumData[0][y])) << 10);
                nR = (((int)(256-g_this_mod->spectrumData[1][y])) << 10);
                nL *= 0.4 + 5.6*(y/512.0);
                nR *= 0.4 + 5.6*(y/512.0);
                g_SoundBuffer[x]   = min(nL, g_SoundBuffer[x]+decamt);
                g_SoundBuffer[x+1] = min(nR, g_SoundBuffer[x+1]+decamt);
            }

            /*
            // set blob_str[]
            int x_max = min(BUFSIZE, 1024);
            int x_lo, x_hi;
            for (int i=0; i<NUM_BLOBS; i++)
            {
                blob_str[i] = 0;
                x_lo = x_max*i/(float)(NUM_BLOBS+1);
                x_hi = x_max*(i+1)/(float)(NUM_BLOBS+1);
                if (x_lo & 1) x_lo--;

                for (x = x_lo; x < x_hi; x+=2)
                    blob_str[i] -= g_SoundBuffer[x];
                blob_str[i] /= (float)(x_hi - x_lo);
                
                // show bars:
                //for (x = x_lo; x < x_hi; x+=2)
                //    g_SoundBuffer[x] = -blob_str[i];
            }

            // move charges
            for (i=0; i<NUM_BLOBS; i++)
            {
                blob_x[i] += 0.2*cosf(intframe*0.0133 + i/(float)NUM_BLOBS*6.28);
                blob_y[i] += 0.2*sinf(intframe*0.0151 + i/(float)NUM_BLOBS*6.28);
            }
            float u, v, d, repulsion;
            for (i=0; i<NUM_BLOBS; i++)
                for (int j=0; j<NUM_BLOBS; j++)
                    if (i != j)
                    {
                        u = blob_x[i] - blob_x[j];
                        v = blob_y[i] - blob_y[j];
                        d = sqrtf(u*u+v*v) + 0.1;
                        repulsion = blob_str[i]*blob_str[j]/d * 0.00000005f;
                        blob_x[i] += u*repulsion;
                        blob_y[i] += v*repulsion;
                    }
            for (i=0; i<NUM_BLOBS; i++)
            {
                blob_x[i] *= 0.98;// - 0.001*blob_str[i]*0.001;
                blob_y[i] *= 0.98;// - 0.001*blob_str[i]*0.001;
            }

            
            char buf[99];
            sprintf(buf, "blob_x[2] = %f, blob_str[2] = %f", blob_x[2], blob_str[2]);
            dumpmsg(buf);
            */

            // write matrix - sloppy
            /*
            float dx, dy;
            int j, k;
            for (i=0; i<40; i++)
            for (j=0; j<30; j++)
            {
                matrix[i][j] = 0;
                for (k=0; k<NUM_BLOBS; k++)    
                {
                    dx = i-20 - blob_x[k];
                    dy = j-15 - blob_y[k];
                    d = (dx*dx+dy*dy) + 0.1;
                    matrix[i][j] += blob_str[k] / d;   // actually r^2
                }
            }
            */


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
            center[0] /= (float)FXW*0.125;
            center[1] /= (float)FXW*0.125;

            // now translate wave so center is at zero.
            for (i=0; i<BUFSIZE; i+=2) g_fSoundBuffer[i] -= center[0];
            for (i=1; i<BUFSIZE; i+=2) g_fSoundBuffer[i] -= center[1];
        }


        // ----------------
        // ----------------
        // fourier analysis
        // ----------------
        // ----------------

        //----disable map damping----
        if (iDispBits > 8)
        //----disable map damping----
        {
            int n, i;
            float theta;
            float a, b;    // a + bi
            float old_power;
            float net_power_change = 1.0;
            
            for (n=1; n<FOURIER_DETAIL; n++)
            {
                a = 0;
                b = 0;
                float w = 6.28 * (20*powf(2, n/(float)FOURIER_DETAIL*10)/44100.0);
                for (i=0; i<256; i++)
                {
                    theta = w*i;
                    a += g_fSoundBuffer[i*2] * cosf(theta);
                    b += g_fSoundBuffer[i*2] * sinf(theta);
                }
                //g_power[n] = max(sqrtf(a*a + b*b), g_power[n]*0.95);
                old_power = g_power[n];
                g_power[n] = sqrtf(a*a + b*b);
                g_power_smoothed[n] = g_power_smoothed[n]*0.94 + 0.06*g_power[n];
                net_power_change += fabsf(old_power - g_power[n]);
            }

            // divide the raw sum of 'frequency differences'
            // by the average volume as far back as we know of...
            // this gives us an average freq. change RELATIVE TO
            // the volume of all the other songs that have played.
            net_power_change /= (float)(iVolumeSum / (intframe + 1));
            // at this point, >100 should be full-speed, while toward 0 should be slower.
            // 'suggested_damping' should be from roughly 0.3 ... 1.0
            // -> suggested_damping == 0: no movement
            // -> suggested_damping == 1: full movement
            // -> suggested_damping >  1: full movement (extra ignored)
            net_power_change *= 0.01;
            if (intframe < 50)
                suggested_damping = 1.0;
            else
                suggested_damping = suggested_damping*0.98 + net_power_change*0.02;
            
            // ----disable map damping----
            suggested_damping = 1.0f;
            // ----disable map damping----

            debug_param = suggested_damping;
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
            i -= 4; // was measuring both channels by -= 3 !!!
        }

        /*
        // cancel out vocals for volume calx
        i = BUFSIZE - 4;
        int temp;
        while (i>0)
        {
            temp = abs(g_SoundBuffer[i] - g_SoundBuffer[i+1]);
            low = min(low, temp);
            high = max(high, temp);
            i -= 4;
        }
        */

        vol = (high - low) / 256.0;

        //char buf[64];
        //sprintf(buf, "%f", vol);
        //dumpmsg(buf);

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

        float rate;

        current_vol  = vol;
        //avg_vol_narrow = avg_vol_narrow*0.3 + vol*0.7;
        //avg_vol        = avg_vol*0.85 + vol*0.15;
        //avg_vol_wide   = avg_vol_wide*0.96 + vol*0.04;
        //avg_peaks      = avg_peaks*0.9 + peaks*0.1;
        rate = AdjustRateToFPS(0.30f, 30.0f, fps_at_last_mode_switch); avg_vol_narrow = avg_vol_narrow*rate + vol*(1-rate);
        rate = AdjustRateToFPS(0.85f, 30.0f, fps_at_last_mode_switch); avg_vol        = avg_vol       *rate + vol*(1-rate);
        rate = AdjustRateToFPS(0.96f, 30.0f, fps_at_last_mode_switch); avg_vol_wide   = avg_vol_wide  *rate + vol*(1-rate);
        rate = AdjustRateToFPS(0.90f, 30.0f, fps_at_last_mode_switch); avg_peaks      = avg_peaks     *rate + peaks*(1-rate);
        
        iVolumeSum += (int)avg_vol;



        g_hit = max(g_hit-1, -1);

        #if PLUGIN
            if (!g_bDisableSongTitlePopups && g_hit == -1 && (rand()%7000) < g_random_songtitle_freq*g_random_songtitle_freq*30.0f/fps_at_last_mode_switch) 
            {
                g_hit = g_song_tooltip_frames * fps_at_last_mode_switch/30.0f;
                g_title_R = 128 + rand() % 99;
                g_title_G = 128 + rand() % 99;
                g_title_B = 128 + rand() % 99;
            }
        #endif

        //if (vol > avg_vol*3.5 && vol > avg_vol_wide*3.0) g_hit = 10;
        //if (g_hit == -1 && vol > avg_vol_wide*3.6) g_hit = 5;

        /*
        if (effect[SPECTRAL]==1)//if (mode==17 && !SoundEmpty)
        {
                /////
                static int last_z[NUM_FREQS];
                int i, k;
                int z[NUM_FREQS];

                // problem here!
                if (intframe < 10)
                    for (i=0; i<NUM_FREQS; i++) last_z[i] = 0;

                for (i=0; i<NUM_FREQS; i++) z[i] = 0;
                
                for (k=0; k<NUM_FREQS; k++)
                    for (i=30; i<FREQ_SAMPLES-30; i+=2) z[k] += abs(g_Lsmoothed[k][i]);    

                for (i=0; i<NUM_FREQS; i++) 
                {
                    z[i] = z[i]*0.6 + last_z[i]*0.4;
                    z[i] *= (256.0 - 60.0)/(float)(FREQ_SAMPLES - 60);
                    z[i] *= 0.0005 + 0.00070*(k/(float)(NUM_FREQS-1)) + 0.00045*sqrtf(k/(float)(NUM_FREQS-1));
                    if (z[i] < 0) z[i] = 0;
                    if (z[i] > 255) z[i] = 255;

                    if (min(z[i],255) > last_z[i]-4)
                        g_phase_inc[i] += (last_z[i] - min(z[i],255))*2.5;  // 20 looks cool too

                    last_z[i] = min(z[i],255);
                }


                int nodes = NUM_FREQS;///// + rand()%5;
                int n;
                int phase = 0.8;/////rand()%1000;
                int rad;
                int x,y,cx,cy,str;
                int val;
                int r;// = rand()%8 + 3;  // MAX is **10**

                if (FXW==320)
                    //r = 2 + 20*(vol/avg_vol - 1.25);
                    r = 2 + 40*(vol/avg_vol_narrow - 1.1);
                else
                    //r = 3 + 32*(vol/avg_vol - 1.25);
                    r = 3 + 40*(vol/avg_vol_narrow - 1.1);
                if (r<1) r=1;
                if (FXW==320 && r>7) r=7;
                if (FXW!=320 && r>10) r=10;

                if (FXW==320)
                    rad = 22;///// + rand()%6; //min(35, 14 + 0.7*vol);//(20 + rand()%30);
                else
                    rad = 34;///// + rand()%8; //min(50, 20 + vol);//(20 + rand()%30);

                float cr=1.0, cg=1.0, cb=1.0;
                if (iDispBits>8)
                {
                    int intfram = intframe + chaser_offset;
                    float f = 7*sinf(intfram*0.007+29) + 5*cosf(intfram*0.0057+27);
                    cr = 0.58 + 0.21*sinf(intfram*gF[0] +20-f) + 0.21*cosf(intfram*gF[3] +17+f);
                    cg = 0.58 + 0.21*sinf(intfram*gF[1] +42+f) + 0.21*cosf(intfram*gF[4] +26-f);
                    cb = 0.58 + 0.21*sinf(intfram*gF[2] +57-f) + 0.21*cosf(intfram*gF[5] +35+f);
                }

                long T_offset;
                int pixelsize = (iDispBits==8) ? 1 : 4;

                for (n=0; n<nodes; n++)
                {
                    /////
                    float s = FXW/640.0;
                    float temp_frame = g_phase_inc[n];
                    cx = gXC + s*(64*cosf(temp_frame*0.00451+n*99+3) + 51*cosf(temp_frame*0.00572+n*13+7));  
                    cy = gYC + s*(51*cosf(temp_frame*0.00502+n*78+8) + 45*cosf(temp_frame*0.00653+n*17+5)); 
                    //float crad = ((cx-gXC)*(cx-gXC) + (cy-gYC)*(cy-gYC));
                    //cx = 144.0/(crad+0.1);//max(0, 12 - (crad + 1));
                    //cy =  81.0/(crad+0.1);//max(0,  9 - (crad + 1));
                    /////

                    /////cx = gXC + rad*cosf(n/(float)nodes * 6.28 + phase);
                    /////cy = gYC + rad*sinf(n/(float)nodes * 6.28 + phase);

                    int sqrt_pos = 0;

                    /////
                    //if (n==0) { r = z[n]*0.06 - 2; cr = 0.0; cg = 0.5; cb = 1.0; }
                    //if (n==1) { r = z[n]*0.06 - 2; cr = 1.0; cg = 0.5; cb = 0.0; }  // awesome fiery yellow-red
                    //if (n==2) { r = z[n]*0.06 - 2; cr = 0.2; cg = 1.0; cb = 0.4; }
                    //if (n==3) { r = z[n]*0.06 - 2; cr = 1.0; cg = 0.0; cb = 0.5; }
                    //if (n==4) { r = z[n]*0.06 - 2; cr = 0.5; cg = 1.0; cb = 0.3; }
                    //if (n==5) { r = z[n]*0.06 - 2; cr = 0.2; cg = 0.5; cb = 1.0; }
                    //if (n==6) { r = z[n]*0.06 - 2; cr = 0.5; cg = 0.5; cb = 0.5; }
                    cr = g_DotColor[n][0];
                    cg = g_DotColor[n][1];
                    cb = g_DotColor[n][2];
                    r = z[n]*0.06 - 2;
                    if (r>10) r=10;
                    /////
                   
                    for (y=-10; y<=10; y++)
                    {
                        T_offset = ((cy+y)*FXW + (cx-10)) * pixelsize;
                        for (x=-10; x<=10; x++)
                        {
                            val = (r - sqrt_tab[x+10][y+10])*25;
                            if (val>0)
                            {
                                if (iDispBits==8)
                                {
                                    str = VS1[T_offset] + val;
                                    VS1[T_offset] = min(255, str);
                                }
                                else
                                {
                                    str = VS1[T_offset  ] + val*cb;
                                    VS1[T_offset  ] = min(255, str);
                                    str = VS1[T_offset+1] + val*cg;
                                    VS1[T_offset+1] = min(255, str);
                                    str = VS1[T_offset+2] + val*cr;
                                    VS1[T_offset+2] = min(255, str);
                                }
                            }
                            T_offset += pixelsize;
                        }
                    }
                }
        }
        */

        if (effect[NUCLIDE]>0 && !SoundEmpty)
        {
            //if (vol > avg_vol*1.25)
            if (vol > avg_vol_narrow*1.1)
            {

                int nodes = 3 + rand()%5;
                int n;
                int phase = rand()%1000;
                int rad;
                int x,y,cx,cy,str;
                int val;
                int r;// = rand()%8 + 3;  // MAX is **10**

                if (FXW==320)
                    //r = 2 + 20*(vol/avg_vol - 1.25);
                    r = 2 + 40*(vol/avg_vol_narrow - 1.1);
                else
                    //r = 3 + 32*(vol/avg_vol - 1.25);
                    r = 3 + 40*(vol/avg_vol_narrow - 1.1);
                if (r<1) r=1;
                if (FXW==320 && r>7) r=7;
                if (FXW!=320 && r>10) r=10;

                if (FXW==320)
                    rad = 22 + rand()%6; //min(35, 14 + 0.7*vol);//(20 + rand()%30);
        else {
                    rad = (34 + rand()%8);
          if (FXW > 1024)
            rad = rad * FXW/1024.0f; //min(50, 20 + vol);//(20 + rand()%30);
        }

                float cr=1.0, cg=1.0, cb=1.0;
                if (iDispBits>8)
                {
                    int intfram = intframe + chaser_offset;
                    float f = 7*sinf(intfram*0.007+29) + 5*cosf(intfram*0.0057+27);
                    cr = 0.58 + 0.21*sinf(intfram*gF[0] +20-f) + 0.21*cosf(intfram*gF[3] +17+f);
                    cg = 0.58 + 0.21*sinf(intfram*gF[1] +42+f) + 0.21*cosf(intfram*gF[4] +26-f);
                    cb = 0.58 + 0.21*sinf(intfram*gF[2] +57-f) + 0.21*cosf(intfram*gF[5] +35+f);
                }

                long T_offset;
                int pixelsize = (iDispBits==8) ? 1 : 4;

                for (n=0; n<nodes; n++)
                {
                    cx = gXC + rad*cosf(n/(float)nodes * 6.28 + phase);
                    cy = gYC + rad*sinf(n/(float)nodes * 6.28 + phase);

                    int sqrt_pos = 0;
                   
                    for (y=-10; y<=10; y++)
                    {
                        T_offset = ((cy+y)*FXW + (cx-10)) * pixelsize;
                        for (x=-10; x<=10; x++)
                        {
                            //val = (r - sqrtf(x*x+y*y))*25;
                            val = (r - sqrt_tab[x+10][y+10])*25;
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
                                    str = VS1[T_offset  ] + val*cb;
                                    VS1[T_offset  ] = min(255, str);
                                    str = VS1[T_offset+1] + val*cg;
                                    VS1[T_offset+1] = min(255, str);
                                    str = VS1[T_offset+2] + val*cr;
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
    
    //char buf[64];
    //sprintf(buf, "%f", current_vol);
    //dumpmsg(buf);

    //base = (current_vol*4.5  + avg_vol * 0.45) - 10;
    #ifdef PLUGIN
        base = (current_vol*4  + avg_vol * 0.4) - 10;
    #endif
    #ifdef SAVER
        //base = (current_vol)/(avg_vol + 0.1f) * 160 * max(0, min(1.0f, (current_vol-0.1f)/0.1f));
        // revised in 4.25:
        base = (current_vol*6 - avg_vol*3.5f)*10 - 40;
    #endif


    //-------------------------------------------

    //------------------------------------
    //ATTEMPT #2: two means, two std. devs
    //------------------------------------

    // find the avg. vol of the past N frames...
    // now draw a line there, and find the average of volumes above it,
    //   and the average of volumes below it.
    // measure avg_dev_lo and avg_dev_hi... if both low, we're in beat mode.

    // cheat: store a smoothed-out volume tape, not a jaggy one
    past_vol[past_vol_pos] = avg_vol_narrow;

    // 1. get avg vol
    float avg_vol_uniform = 0;
    for (i=0; i<PAST_VOL_N; i++) avg_vol_uniform += past_vol[i];
    avg_vol_uniform /= (float)PAST_VOL_N;

/*  // 2. get average hi-vol and average lo-vol
    float avg_vol_lo = 0;
    float avg_vol_hi = 0;
    int   n_lo = 0;
    int   n_hi = 0;
    for (i=0; i<PAST_VOL_N; i++) 
    {
        if (past_vol[i] > avg_vol_uniform)
        {
            avg_vol_hi += past_vol[i];
            n_hi++;
        }
        else
        {
            avg_vol_lo += past_vol[i];
            n_lo++;
        }
    }
    avg_vol_hi /= (float)n_hi;
    avg_vol_lo /= (float)n_lo;

    // 3. get average deviation from each of the volumes.
    float avg_dev_lo = 0;
    float avg_dev_hi = 0;
    for (i=0; i<PAST_VOL_N; i++) 
        if (past_vol[i] > avg_vol_uniform)
            avg_dev_hi += fabs(past_vol[i] - avg_vol_hi);
        else
            avg_dev_lo += fabs(past_vol[i] - avg_vol_lo);
    avg_dev_hi /= (float)n_hi;
    avg_dev_lo /= (float)n_lo;
*/

    // BEATMODE:
    // to know whether or not we should be in beat mode,
    // look at the n past 'avg_vol_narrow' values.  This
    // creates a smoothed wave that is flattish under normal
    // circumstances, but very evenly bumpy (w/high amplitude)
    // when a good beat is on.


    //bBeatMode = false;
    float beat_strength = 0;
    for (i=1; i<PAST_VOL_N; i++) 
        beat_strength += max(0, fabs(past_vol[i] - past_vol[i-1]) - avg_vol_uniform*0.15);
        //beat_strength += max(0, max(0, past_vol[i] - past_vol[i-1]) - avg_vol_uniform*0.15);
    beat_strength /= avg_vol_uniform;
    beat_strength *= 10;
    if (avg_vol_uniform < 10) beat_strength = 0;

    if (beat_strength > 90 + 19) bBeatMode = true;
    if (beat_strength < 90 - 19) bBeatMode = false;


/*
    // Code to visualize the past n 'avg_vol_narrow' values:
    memset((void *)g_fSoundBuffer, 0, BUFSIZE*sizeof(float));
    for (i=0; i<PAST_VOL_N; i++)
    {
        int j = i - past_vol_pos;
        if (j<0) j += PAST_VOL_N;
        
        g_fSoundBuffer[j*2] = past_vol[i]/4;
    }
*/


    // NOW we know bBeatMode.  Use old code to get beat-mode brightness.


    
    //float avg_vol_uniform = 0;
    //for (i=0; i<PAST_VOL_N; i++) avg_vol_uniform += past_vol[i];
    //avg_vol_uniform /= (float)PAST_VOL_N;
    float variance = 0;
    float temp;
    for (i=0; i<PAST_VOL_N; i++) 
    {
        temp = (past_vol[i] - avg_vol_uniform);
        variance += temp*temp;
    }
    variance /= (float)(PAST_VOL_N - 1);
    float std_dev = sqrtf(variance);

    /*
    float avg_vol_change = 0;
    for (i=1; i<PAST_VOL_N; i++) avg_vol_change += abs(past_vol[i] - past_vol[i-1]);
    avg_vol_change /= (float)(PAST_VOL_N - 1);
    float avg_vol_change_var = 0;
    for (i=1; i<PAST_VOL_N; i++) 
    {
        temp = abs(past_vol[i] - past_vol[i-1]) - avg_vol_change;
        avg_vol_change_var += temp*temp;
    }
    avg_vol_change_var /= (float)(PAST_VOL_N - 2);
    float avg_vol_change_std_dev = sqrtf(avg_vol_change_var);
    */

    float max_vol = 0;
    for (i=0; i<PAST_VOL_N/3; i++) max_vol = max(max_vol, past_vol[i]);
    if (avg_vol_narrow > max_vol*fBigBeatThreshold)//avg_vol_narrow - avg_vol_wide > avg_vol_change_std_dev*2)
        bBigBeat = true;
    else
        bBigBeat = false;

    //damped_std_dev = damped_std_dev*0.95 + std_dev*0.05;
    //if (damped_std_dev > 12.7f) bBeatMode = true;
    //if (damped_std_dev < 11.3f) bBeatMode = false;

    #ifdef PLUGIN
        float brite_scale = (current_vol - avg_vol_uniform) / (std_dev*0.5);
        //float brite_scale = (current_vol - avg_vol_uniform*0.8f) / (avg_vol_uniform*0.95f - avg_vol_uniform*0.8f);
    #endif
    #ifdef SAVER
        // revised in 4.25:
        float brite_scale = 1.0f;
    #endif

    // at current_vol:         val = 0
    // at 0.5 std. dev. above: val = 1
    if (brite_scale < 0.0f) brite_scale = 0.0f;
    if (brite_scale > 1.0f) brite_scale = 1.0f;

    if (bBeatMode && 
        g_bUseBeatDetection &&
        #if PLUGIN
        visMode != spectrum &&
        #endif
        waveform != 6
        ) 
    {
        // use beat detection mode
        // a "beat" hits whenever current_vol is > a user-definable # of standard 
        //   deviations away from the uniform average vol. over the last so many 
        //   frames.  Usually 0.5 standard deviations.

        base *= brite_scale;
    }

    // shifting
    int old_slider1 = slider1;
    //if (bBeatMode && brite_scale >= 0.99f && g_bSlideShift && g_FramesSinceShift > g_SlideShiftMinFrames)
    // new technique:
    if (bBeatMode && g_bSlideShift)
    {
        //static float last_peak = 0;

        if (current_vol > g_ShiftMaxVol)
        {
            g_ShiftMaxVol = current_vol * 1.05;
            //last_peak = g_ShiftMaxVol;

            if (g_FramesSinceShift > 2)
            {
                g_FramesSinceShift = 0;

                //slider1 = (rand()%FXW - FXW/2)/115;//80;
                slider1 = ((rand()%(FXW/2)) + 50)/145;//80;

                // force it to go the other way on X this time:
                if (old_slider1 > 0) slider1 *= -1;

                // vertical shift: -1, 0, or 1 allowed
                slider1 += (rand()%3-1)*FXW;
            }
        }
        else
        {
            float rate = 0.975f;
            rate = AdjustRateToFPS(0.975f, 30.0f, fps);
            float limit_vol = avg_vol_uniform*1.43;
            g_ShiftMaxVol = g_ShiftMaxVol*rate + limit_vol*(1-rate);
            g_FramesSinceShift++;

            //if (g_ShiftMaxVol > limit_vol)
            //{
                // within 50 frames, it will go back to low expectations.
                //g_ShiftMaxVol -= (last_peak - limit_vol)*0.02;
            //}
        }
    }



    sprintf(szDEBUG, "%5.3f ", debug_param);
    if (bBeatMode) strcat(szDEBUG, ", beat mode");
    if (bBigBeat) strcat(szDEBUG, ", BIGBEAT");
        

    ///if (iDispBits==8)
    ///    base += wave_brightness;

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
        if (g_bSyncColorToSound)
        {
            // 1. base color on time
            float ir2, ig2, ib2;   // was -int-
            int intfram = intframe + chaser_offset;
            float f = 7*sinf(intfram*0.006+59) + 5*cosf(intfram*0.0077+17);
            ir2 = base*1.07 * (1 + 0.3*sinf(intfram*gF[0] +10-f)) * (1 + 0.20*cosf(intfram*gF[1] +37+f));
            ig2 = base*1.07 * (1 + 0.3*sinf(intfram*gF[2] +32+f)) * (1 + 0.20*cosf(intfram*gF[3] +16-f));
            ib2 = base*1.07 * (1 + 0.3*sinf(intfram*gF[4] +87-f)) * (1 + 0.20*cosf(intfram*gF[5] +25+f));
            
            ///ir += wave_brightness;
            ///ig += wave_brightness;
            ///ib += wave_brightness;

            // 2. base color on frequency
            int a;
            float ir=0, ig=0, ib=0;
            //4.20:
            //for (a=0; a<FOURIER_DETAIL*0.34; a++) ir += g_power_smoothed[a];
            //for (a=FOURIER_DETAIL*0.36; a<FOURIER_DETAIL*0.63; a++) ig += g_power_smoothed[a];
            //for (a=FOURIER_DETAIL*0.6; a<FOURIER_DETAIL; a++) ib += g_power_smoothed[a];
            for (a=0; a<FOURIER_DETAIL*0.31; a++) ir += g_power_smoothed[a];
            for (a=FOURIER_DETAIL*0.30; a<FOURIER_DETAIL*0.59; a++) ig += g_power_smoothed[a];
            for (a=FOURIER_DETAIL*0.56; a<FOURIER_DETAIL; a++) ib += g_power_smoothed[a];
            ir *= 0.93;
            ig *= 1.18;
            ib *= 2.40;
             ir -= ib*0.4;
            f = base/sqrtf(ir*ir + ig*ig + ib*ib);
            ir *= f;
            ig *= f;
            ib *= f;

            // 3. combine
            ir = (ir*0.97 + ir2*0.03)*1.35;
            ig = (ig*0.97 + ig2*0.03)*1.35;
            ib = (ib*0.97 + ib2*0.03)*1.35;

            if (ir<0) ir=0;
            if (ir>255) ir=255;
            if (ig<0) ig=0;
            if (ig>255) ig=255;
            if (ib<0) ib=0;
            if (ib>255) ib=255;
            r = ir;
            g = ig;
            b = ib;
        }
        else
        {
            float ir, ig, ib;   // was -int-
            float intfram = (float)(intframe + chaser_offset) * 30.0f/fps_at_last_mode_switch;
            float f = 7*sinf(intfram*0.006+59) + 5*cosf(intfram*0.0077+17);
            float c1 = 0.55f;  // was 0.3f in v4.25 and before
            float c2 = 0.50f;  // was 0.2f in v4.25 and before
            ir = base*1.07 * (1 + c1*sinf(intfram*gF[0] +10-f)) * (1 + c2*cosf(intfram*gF[1] +37+f));
            ig = base*1.07 * (1 + c1*sinf(intfram*gF[2] +32+f)) * (1 + c2*cosf(intfram*gF[3] +16-f));
            ib = base*1.07 * (1 + c1*sinf(intfram*gF[4] +87-f)) * (1 + c2*cosf(intfram*gF[5] +25+f));
                
            ///ir += wave_brightness;
            ///ig += wave_brightness;
            ///ib += wave_brightness;

            // COFFEE FILTER:
            //ig = powf(ir/255.0f, 1.5f)*255.0f;
            //ib = powf(ir/255.0f, 1.9f)*255.0f;
            /*
            float ig2, ib2, ir2;
            float fExp, fLerp;
            #define FRAND ((rand()%4716)/4715.0f);
            #define GEN_RAND(x,y,z) {   \
                fExp = 0.5f + 0.5f*cosf(intframe*(x+0.432f)+2);   \
                fExp = powf(fExp, 0.5f); \
                if (z==1) { \
                    fLerp = -9.0f + 11.0f*cosf(intframe*x+y+2);   \
                    fLerp = max(0,min(1,fLerp));   \
                } \
            }
            GEN_RAND(0.27929f,1,1); fLerp=0; ig2 = powf(ir/255.0f, 1+fExp)*255.0f*fLerp + ig*(1-fLerp);
            GEN_RAND(0.24552f,3,0); fLerp=0; ib2 = powf(ir/255.0f, 1+fExp)*255.0f*fLerp + ib*(1-fLerp);
            GEN_RAND(0.23016f,7,1); fLerp=1; ir2 = powf(ig/255.0f, 1+fExp)*255.0f*fLerp + ir*(1-fLerp);
            GEN_RAND(0.25357f,6,0); fLerp=1; ib2 = powf(ig/255.0f, 1+fExp)*255.0f*fLerp + ib*(1-fLerp);
            GEN_RAND(0.24987f,2,1); fLerp=0; ir2 = powf(ib/255.0f, 1+fExp)*255.0f*fLerp + ir*(1-fLerp);
            GEN_RAND(0.22934f,0,0); fLerp=0; ig2 = powf(ib/255.0f, 1+fExp)*255.0f*fLerp + ig*(1-fLerp);
            ir = ir2;
            ig = ig2;
            ib = ib2;
            */

            /*
            float desat = -14.0f + 6.0f*cosf(intframe/fps_at_last_mode_switch*0.63723f+2) 
                                 + 8.0f*cosf(intframe/fps_at_last_mode_switch*0.58399f+1)
                                    + 7.0f*cosf(intframe/fps_at_last_mode_switch*0.43818f+4);
            desat = min(1,max(0,desat));
            float lum = ir*0.32f + ig*0.43f + ib*0.25f;
            ir = lum*desat + ir*(1-desat);
            ig = lum*desat + ig*(1-desat);
            ib = lum*desat + ib*(1-desat);
            */

            if (ir<0) ir=0;
            if (ir>255) ir=255;
            if (ig<0) ig=0;
            if (ig>255) ig=255;
            if (ib<0) ib=0;
            if (ib>255) ib=255;
            r = ir;
            g = ig;
            b = ib;
        }    
    }

        //UNFIN
        // NEW_COLORS_430
        // ALTERNATE WAVE COLORING METHOD:
        #if 0
        {
            #define FRAND ((rand()%4716)/4715.0f)
            #define COIN_TOSS (FRAND < 0.5f)
            static float sc[3] = { FRAND, FRAND, FRAND };
            static float dc[3] = { FRAND, FRAND, FRAND };
            static int start_frame = 0;
            static int lerp_frames = 60;
            if (intframe >= start_frame + lerp_frames) 
            {
                sc[0] = dc[0];
                sc[1] = dc[1];
                sc[2] = dc[2];
                dc[0] = powf(FRAND, 1.0f)*1.2f;
                dc[1] = powf(FRAND, 1.0f)*1.2f;
                dc[2] = powf(FRAND, 1.0f)*1.2f;
                float desat = powf(FRAND, 3.0f);
                float lum = 0.3f*dc[0] + 0.5f*dc[1] + 0.2f*dc[2];
                dc[0] = dc[0]*(1.0f-desat) + lum*desat;
                dc[1] = dc[1]*(1.0f-desat) + lum*desat;
                dc[2] = dc[2]*(1.0f-desat) + lum*desat;
                
                /*
                if (FRAND < 0.25f)
                  dc[0] = powf(  COIN_TOSS ? dc[1] : dc[2], 0.5f + 0.5f*FRAND );
                else if (FRAND < 0.333f)
                  dc[1] = powf(  COIN_TOSS ? dc[0] : dc[2], 0.5f + 0.5f*FRAND );
                else if (FRAND < 0.5f)
                  dc[2] = powf(  COIN_TOSS ? dc[1] : dc[0], 0.5f + 0.5f*FRAND );
                  */
                start_frame = intframe;
                float lerp_duration_seconds = 0.5f * powf(1.6f, FRAND*2-1) * 1.1f;
                lerp_frames = lerp_duration_seconds * fps_at_last_mode_switch;
            }
            float t = (intframe - start_frame) / (float)lerp_frames;
            int ir = (sc[0]*(1.0f-t) + dc[0]*t)*255;
            int ig = (sc[1]*(1.0f-t) + dc[1]*t)*255;
            int ib = (sc[2]*(1.0f-t) + dc[2]*t)*255;
            


            if (ir<  0) ir=0;
            if (ir>255) ir=255;
            if (ig<  0) ig=0;
            if (ig>255) ig=255;
            if (ib<  0) ib=0;
            if (ib>255) ib=255;
            r = ir;
            g = ig;
            b = ib;
        }
        #endif



    //EXPERIMENT
    //EXPERIMENT
    //EXPERIMENT
    /*
    if ((intframe % 50)==0)
    {
        char msg[64];
        sprintf(msg, "sound: wave#/ready/active/empty = %d/%d/%d/%d", waveform, SoundReady, SoundActive, SoundEmpty);
        dumpmsg(msg);
    }
    */
    //EXPERIMENT
    //EXPERIMENT
    //EXPERIMENT

    if (SoundReady && SoundActive && !SoundEmpty)
    {
        //EXPERIMENT
        //EXPERIMENT
        //EXPERIMENT
        /*
        if ((intframe % 50)==0)
        {
            char msg[64];
            sprintf(msg, "wave: r=%d g=%d b=%d", r, g, b);
            dumpmsg(msg);
        }
        r = 100;
        g = 120;
        b = 140;
        */
        //EXPERIMENT
        //EXPERIMENT
        //EXPERIMENT

        // for high resolutions, add more sound samples (via interpolation)
        int passes = 0;
        if (waveform==1 || waveform==2)
        {
            if (FXW >= 1920)
                passes = 2;
            else if (FXW > 1024)
                passes = 1;
        }
        else 
        {
            if (FXW >= 1440)
                passes = 1;
        }
        float amp_scale_per_pass = 1.14f;
        for (int pass=0; pass<passes; pass++)
        {
            float fSoundBufTemp[16384];
            memcpy(fSoundBufTemp, g_fSoundBuffer, sizeof(float) * min(16384,FXW*4));
            float sL = fSoundBufTemp[0];
            float sR = fSoundBufTemp[1];
            for (int i=0; i<FXW*2; i+=2)
            {
                float tL = fSoundBufTemp[i+2] * amp_scale_per_pass;
                float tR = fSoundBufTemp[i+3] * amp_scale_per_pass;
                float* p = &g_fSoundBuffer[i*2];
                p[0] = sL;
                p[1] = sR;
                p[2] = 0.5f*(sL + tL);
                p[3] = 0.5f*(sR + tR);
                sL = tL;
                sR = tR;
            }
        }


        if (waveform==1)
        {
            int y_center = gYC;//FXH/2;
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
            zL = g_fSoundBuffer[start & 0xFFFFFFFE] + y_center;
            
            for (i=start; i<end; i++)  
            {
                prev_zL = zL;
                zL = g_fSoundBuffer[(i & 0xFFFFFFFE)] + y_center;
                zL = prev_zL*0.9 + zL*0.1;
                yL = (int)zL;

                if (yL >= FX_YCUT_HIDE && yL < FXH-FX_YCUT_HIDE)
                {
                    D_offset = FXW*yL + i;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
                    }                
                }
            }
        }
        else if (waveform==2)
        {
            float fDiv = 0.7;
            float h1 = gYC - FXH*0.12;//FXH*0.4;
            float h2 = gYC + FXH*0.12;//FXH*0.6;
            zL = g_fSoundBuffer[0] * fDiv + h1;
            zR = g_fSoundBuffer[1] * fDiv + h2;
            for (i=0; i<FXW; i++)  
            {
                prev_zL = zL;
                prev_zR = zR;
                zL = g_fSoundBuffer[(i & 0xFFFFFFFE)] * fDiv + h1;
                zR = g_fSoundBuffer[(i & 0xFFFFFFFE)+1] * fDiv + h2;
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
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
                    }
                }
                if (yR>FX_YCUT_HIDE && yR<FXH-FX_YCUT_HIDE) 
                {
                    D_offset = FXW*yR + i;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
                    }
                }
            }
        }
        else if (waveform==3)
        {
            zL = g_fSoundBuffer[FX_YCUT_HIDE^(FX_YCUT_HIDE & 1)] + gXC;//FXW/2;
            for (i=FX_YCUT_HIDE; i<FXH-FX_YCUT_HIDE; i++)  
            {
                prev_zL = zL;
                zL = g_fSoundBuffer[(i & 0xFFFFFFFE)] + gXC;//FXW/2;
                zL = prev_zL*0.9 + zL*0.1;
                xL = (int)zL;
                if (xL>=0 && xL<FXW)
                {
                    D_offset = FXW*i + xL;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
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
                zL = g_fSoundBuffer[(i & 0xFFFFFFFE)] * fDiv;
                zR = g_fSoundBuffer[(i & 0xFFFFFFFE)+1] * fDiv;
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
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
                    }
                }
                if (xR>=0 && xR<FXW)
                {
                    D_offset = FXW*i + xR;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
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
            float base_rad = (FXW==320) ? 40.0 : FXW/640.0f*60.0;
            float fDiv = 0.7;

            for (i=0; i<WAVE_5_BLEND_RANGE; i++)  
            {
                amt = i * range_inv;
                g_fSoundBuffer[(i & 0xFFFFFFFE)] = g_fSoundBuffer[(i & 0xFFFFFFFE)]*(amt) + (1-amt)*(g_fSoundBuffer[(i+314)^((i+314)&1)]);
            }
                        
            rad = base_rad + g_fSoundBuffer[0] * fDiv;
            for (i=0; i<314; i++)  
            {
                rad = rad*0.5 + 0.5*(base_rad + g_fSoundBuffer[(i & 0xFFFFFFFE)] * fDiv);
                if (rad>=5)
                {
                    px = (float)gXC + rad*cosf(i*0.02);
                    py = (float)gYC + rad*sinf(i*0.02);
                    if (px>=0 && px<FXW && py>=FX_YCUT && py<FXH-FX_YCUT)
                    {
                        D_offset = FXW*py + px;
                        if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                        else 
                        { 
                            D_offset *= 4; 
                            if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                            if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                            if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
                        }
                    }
                }
            }
        }
        else if (waveform==6)
        {
            int px, py;
            float fDiv = 1.2;
                        
            float px2, py2;//, ang;
            float ang = sinf(intframe*0.01);
            float cosang = cosf(ang);
            float sinang = sinf(ang);

            px2 = g_fSoundBuffer[0];
            py2 = g_fSoundBuffer[1];
            for (i=0; i<314; i++)  
            {
                px2 = px2*0.5 + 0.5*g_fSoundBuffer[i*2]*fDiv;
                py2 = py2*0.5 + 0.5*g_fSoundBuffer[i*2+1]*fDiv;
                px = px2*cosang + py2*sinang;
                py = px2*-sinang + py2*cosang;
                px += gXC;
                py += gYC;
                if (px>=0 && px<FXW && py>=FX_YCUT && py<FXH-FX_YCUT)
                {
                    D_offset = FXW*py + px;
                    if (iDispBits==8) { if (VS1[D_offset]<r) VS1[D_offset] = r; }
                    else 
                    { 
                        D_offset *= 4; 
                        if (VS1[D_offset  ]<b) VS1[D_offset  ] = b; 
                        if (VS1[D_offset+1]<g) VS1[D_offset+1] = g; 
                        if (VS1[D_offset+2]<r) VS1[D_offset+2] = r; 
                    }
                }
            }
        }
        else if (waveform==7)
        {
            /*
            float d2, dx, dy, val;
            for (int k=0; k<NUM_BLOBS; k++)    
            {
                unsigned char c1 = 50;        //b
                unsigned char c2 = 150*((k+1)/(float)NUM_BLOBS);      //g
                unsigned char c3 = 150;//150-c2;    //r

                float x_halfspan = 10;
                float y_halfspan = x_halfspan;
                int x2, y2;
                for (int x = blob_x[k]-x_halfspan; x < blob_x[k]+x_halfspan; x++)
                for (int y = blob_y[k]-y_halfspan; y < blob_y[k]+y_halfspan; y++)
                {
                    dx = x - blob_x[k];
                    dy = y - blob_y[k];
                    d2 = (dx*dx + dy*dy) / (blob_str[k]*0.001);
                    d2 *= 0.03;

                    if (d2 < 0.5)     // if (d < 0.707)
                    {
                        val = 128;//4*(0.25 + d2*d2 - d2) * 255;
                        x2 = x + gXC;
                        y2 = y + gYC;
                    
                        if (iDispBits==8) 
                        {
                            if (x2>=0 && x2<FXW && y2>=FX_YCUT && y2<FXH-FX_YCUT)
                            {
                                D_offset = y2*FXW + x2;
                                if (VS1[D_offset] < val) VS1[D_offset] = val; 
                            }
                        }
                        else
                        {
                            if (x2>=0 && x2<FXW && y2>=FX_YCUT && y2<FXH-FX_YCUT)
                            {
                                D_offset = (y2*FXW + x2) << 2;
                                if (VS1[D_offset]   < c1) VS1[D_offset  ] = c1;
                                if (VS1[D_offset+1] < c2) VS1[D_offset+1] = c2;
                                if (VS1[D_offset+2] < c3) VS1[D_offset+2] = c3;
                            }
                        }

                    }
                }
            }
            */

            // the rotating line...

            float ang = avg_vol_narrow*0.03;
            float dx = sinf(ang);
            float dy = cosf(ang);
            int x, y;
            float t;

            dx = cosf(intframe*0.03);
            dy = sinf(intframe*0.03);

            if (fabs(dx) > 0.001)    
            {
                float m = dy/dx;
                if (fabs(dx) > fabs(dy))
                {
                    float b = gYC - m*gXC;
                    for (x=0; x<FXW; x++)
                    {
                        y = (int)(m*x + b);
                        if (y>FX_YCUT && y < FXH-FX_YCUT)
                        {
                            D_offset = FXW*y + x;
                            t = min(1.0, fabs(g_fSoundBuffer[x^(x&1)]/64.0f));
                            if (iDispBits==8) { if (VS1[D_offset]<r*t) VS1[D_offset] = r*t; }
                            else 
                            { 
                                D_offset *= 4; 
                                if (VS1[D_offset  ]<r*t) VS1[D_offset  ] = b*t; 
                                if (VS1[D_offset+1]<g*t) VS1[D_offset+1] = g*t; 
                                if (VS1[D_offset+2]<b*t) VS1[D_offset+2] = r*t; 
                            }
                        }
                    }
                }
                else
                {
                    m = dx/dy;
                    float b = gXC - m*gYC;
                    for (y=FX_YCUT; y<FXH-FX_YCUT; y++)
                    {
                        x = (int)(m*y + b);
                        if (x>=0 && x<FXW)
                        {
                            D_offset = FXW*y + x;
                            t = min(1.0, fabs(g_fSoundBuffer[y^(y&1)]/64.0f));
                            if (iDispBits==8) { if (VS1[D_offset]<r*t) VS1[D_offset] = r*t; }
                            else 
                            { 
                                D_offset *= 4; 
                                if (VS1[D_offset  ]<r*t) VS1[D_offset  ] = b*t; 
                                if (VS1[D_offset+1]<g*t) VS1[D_offset+1] = g*t; 
                                if (VS1[D_offset+2]<b*t) VS1[D_offset+2] = r*t; 
                            }
                        }
                    }
                }
            }
        }
    }
}


void dumpmsg(char *s)
{
    if (g_bDebugMode)
    {
        if (!g_bDumpFileCleared)
        {
            g_bDumpFileCleared = true;
            FILE *infile = fopen("c:\\g_debug.txt", "w");
            if (infile)
            {
                fprintf(infile, "[Geiss debug file]\n");
                fclose(infile);
            }
        }
    
        FILE *infile2;
        infile2 = fopen("c:\\g_debug.txt", "a");
        if (infile2)
        {
            fprintf(infile2, "%s\n", s);
            fclose(infile2);
        }
    }
}


#if SAVER
void CheckForNewCD()
{
    if (!g_bDisableCdControls)
    {
        char szMCIAnswer[128];

        //mciSendString("info cdaudio info identity", szNewCD, 128, NULL);
        //dumpmsg(szCurrentCD);
        //dumpmsg(szNewCD);
        //if (strcmp(szNewCD, szCurrentCD) != 0)
        {
            strcpy(szCurrentCD, szNewCD);

            mciSendString("status cdaudio mode wait", szMCIAnswer, 128, NULL);
            if (strcmp(szMCIAnswer, "playing")==0) 
                eDiscState = PLAYING;
            else if (strcmp(szMCIAnswer, "paused")==0) 
                eDiscState = PAUSED;
            else if (strcmp(szMCIAnswer, "stopped")==0) 
                eDiscState = STOPPED;
            else 
            {
                // STOPPED or PAUSED crash it.if Windows CD player is running.
                // PLAYING just disables cd controls if Windoes CD player is running.
                eDiscState = BUSY;
                g_bDisableCdControls = true;    // just for this run!
            }
            
            if (eDiscState != BUSY)
            {
                mciSendString("open cdaudio wait",NULL,NULL,NULL);
                mciSendString("set cdaudio time format tmsf wait",NULL,NULL,NULL);
                mciSendString("status cdaudio current track wait",szMCIAnswer,128,NULL);
                sscanf(szMCIAnswer, "%d", &iTrack);
                mciSendString("status cdaudio number of tracks",szMCIAnswer,128,NULL);
                sscanf(szMCIAnswer, "%d", &iNumTracks);
            }
        }
    }
}
#endif

