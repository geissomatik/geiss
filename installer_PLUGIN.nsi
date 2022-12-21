; This file is part of: Geiss screensaver and Winamp music visualization plug-in
;                           ___ ___ ___ ___ ___
;                          / __| __|_ _/ __/ __|
;                         | (_ | _| | |\__ \__ \
;                          \___|___|___|___/___/
; 3-Clause BSD License
;
; Copyright (c) 1998-2022 Ryan Geiss (@geissomatik)
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; 1. Redistributions of source code must retain the above copyright notice, this
;    list of conditions and the following disclaimer.
;
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
;
; 3. Neither the name of the copyright holder nor the names of its
;    contributors may be used to endorse or promote products derived from
;    this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; ----------------------------------------------------------------
; NOTE: this .nsi script was last built with 'makensis.exe' 
; version 1.98.  If you go to install it (unchanged) and get errors,
; try tracking down that older version, or try to figure out what
; has changed since then.  You can get NSIS at:
;
;               http://www.nullsoft.com/free/nsis/
; 
; This installer will produce a single EXE file that, when run, 
; will decompress and install your plugin and all of its data files 
; to Winamp 2 or Winamp 3.
;
; If Winamp 3 is installed, the plugin will be installed to Winamp 3 
; as a classic visualization plugin, to the directory Winamp3\Plugins.
;
; If Winamp 3 is not present but Winamp 2 is, or if the user manually
; browses to the Winamp 2 directory, it will install to Winamp 2 
; instead, to the directory Winamp\Plugins (or whatever is specified
; as the vis plugins path, in Winamp\winamp.ini).
;
; MODIFYING THIS SCRIPT TO CREATE AN INSTALLER FOR YOUR OWN PLUGIN:
; ----------------------------------------------------------------
;   1) Modify the values of PLUGIN_NAME, PLUGIN_CAPTION, etc. below
;        to reflect your plugin's identifying information.
;   2) Don't forget to update the version # in PLUGIN_CAPTION!
;   3) To specify which files to include in the install,
;        find the command 'SetOutPath' below and modify the subsequent 
;        list of files to include all the files you want bundled with
;        your installer.  See the comments there for more information.
;   4) IMPORTANT: To specify which files to UNINSTALL, 
;        search for the label "UninstStep2" below, and modify the
;        list of files there.
;   5) Run the command 'makensis.exe installer.nsi' to build
;        the executable.  (note: ignore the warning message about
;        'InstallRegKey' being used multiple times; this is necessary
;        to determine whether Winamp 2 or 3 is a better candidate
;        for the install.)
; ----------------------------------------------------------------

!define PLUGIN_NAME    "Geiss"                 ; Name of the component.  Can have spaces in it.
!define PLUGIN_CAPTION "Geiss 4.29 Setup"     ; Caption for the installer dialog
!define PLUGIN_DLL     "vis_geis.dll"         ; The filename of the actual plugin
!define PLUGIN_OUTFILE "bin_plugin\geiss4winamp_429.exe"          ; Name of the installer to create

; ----------------------------------------------------------------
Name ${PLUGIN_NAME}
Caption "${PLUGIN_CAPTION}"
OutFile ${PLUGIN_OUTFILE}
; ----------------------------------------------------------------

; to determine the install directory, we start with Program Files\Winamp.
; then, if winamp 3 is found, override this and use that directory.
; finally, if winamp 2 is found, override again and use that directory.
InstallDir $PROGRAMFILES\Winamp
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" "UninstallString"
DirText \
    "The installer has determined that this is the path to Winamp.$\rIf it is not correct, please change it.  You will not be able to proceed$\runtil a valid path is found." \
    "Select the directory in which Winamp is installed:"

Function .onInit
  IfFileExists $INSTDIR\winamp.exe End      ; else if winamp.exe found (winamp2), we're good
  ;IfFileExists $INSTDIR\winamp3.exe End     ; if winamp3.exe found (winamp3), we're good
  IfFileExists $INSTDIR\studio.exe End      ; or if studio.exe found (older version of winamp3), we're good
  ; the above will work fine if winamp 2 or 3 is installed,
  ; but will break if winamp 2 or 3 was uninstalled *but the directory remains.*
  IfFileExists $PROGRAMFILES\Winamp\winamp.exe SelectNaturalWinamp2         ; last but not least, try the default winamp 2 directory
  ;IfFileExists $PROGRAMFILES\Winamp3\winamp3.exe SelectNaturalWinamp3       ; otherwise, try the default winamp 3 directory
  ;IfFileExists $PROGRAMFILES\Winamp3\studio.exe SelectNaturalWinamp3        ; otherwise, try the default winamp 3 directory
  ; if all of these failed, no good path to Winamp (2 or 3) could be found,
  ; and the 'Next' button will be disabled until the user can specify
  ; the correct folder.
  Goto End
  
  ;SelectNaturalWinamp3:
  ;  strcpy $INSTDIR $PROGRAMFILES\Winamp3
  ;  goto End
    
  SelectNaturalWinamp2:
    strcpy $INSTDIR $PROGRAMFILES\Winamp
    goto End  
  
  End:
FunctionEnd


Function .onVerifyInstDir
  IfFileExists $INSTDIR\Winamp.exe DirOk
  ;IfFileExists $INSTDIR\Winamp3.exe DirOk
  IfFileExists $INSTDIR\Studio.exe DirOk
  Abort   ; leaves the directory as the selected one, but disables the 'Next' button...
  
  DirOk:
FunctionEnd


  
  

Function QueryWinampVisPath 
  ; input: $INSTDIR, the currently-selected install dir (path to winamp)
  ; output: $1, the path to the winamp vis plugins subdirectory
  ;           -for winamp 3x, this is just $INSTDIR\plugins
  ;           -for winamp 2x, it comes from the winamp.ini file
  ;             (or just $INSTDIR\plugins if there is an error reading it.)
  IfFileExists $INSTDIR\Winamp.exe CaseWinamp2
  ;IfFileExists $INSTDIR\Winamp3.exe CaseWinamp3
  ;IfFileExists $INSTDIR\Studio.exe CaseWinamp3  ; legacy check
  goto CaseImpossible
          
  CaseWinamp2:
    StrCpy $1 $INSTDIR\Plugins
    ReadINIStr $8 $INSTDIR\winamp.ini Winamp VisDir
    StrCmp $8 "" End
    IfFileExists $8 0 End
      StrCpy $1 $8 ; update dir
    goto end
  
  ;CaseWinamp3: 
  CaseImpossible:
    StrCpy $1 $INSTDIR\Plugins
    goto end
    
  End: 
FunctionEnd


; The stuff to install
Section ""

  ; this function sets $1 to point to the 'winamp\plugins' folder:
  ; (see function definition above)
  Call QueryWinampVisPath

  ;----------------------------------------------------------------
  ; LIST FILES TO INCLUDE WITH THE INSTALLER
  ;----------------------------------------------------------------
  ; For each file we want to install onto the destination system,
  ; we first set the output directory (relative to $1, which is
  ; the PLUGINS directory) and then list files.  The paths for
  ; the files will be local paths on your hard disk, but fear not -
  ; the files will be placed in the current output directory 
  ; (as last set by SetOutPath) on the destination system.
  
  ; Set the current output path (the folder to which files will be 
  ; decompressed) to '$1', which is the path to the winamp plugins 
  ; folder.
  
  SetOutPath $1
  
  ; Then specify some files to install to that path:
  ; The main DLL for the plugin, the documentation,
  ; and vms_desktop.dll (required for desktop mode to work).
  
  ;File "C:\program files\winamp\plugins\${PLUGIN_DLL}"      ; the main DLL for our plugin
  File "bin_plugin\${PLUGIN_DLL}"
  File "bin_plugin\geiss_about.txt"
  File "bin_plugin\geiss_faq.txt"
  File "bin_plugin\geiss_whatsnew.txt"

  ; To install more data files to a custom subfolder,
  ; just set the new output path (relative to $1)
  ; and then list the files to place in that folder.
  
  ;SetOutPath $1\mysubdir                                   ; will create a relative subdirectory
  ;File "C:\billy\*.jpg"                                    ; put some data files in it
  ;File "C:\billy\*.dat"                                    ; put some data files in it
  
  ;----------------------------------------------------------------
  

  ; now time to create the Uninstaller:

  IfFileExists $INSTDIR\Winamp.exe UninstWinamp2
  ;IfFileExists $INSTDIR\Winamp3.exe UninstWinamp3
  ;IfFileExists $INSTDIR\Studio.exe UninstWinamp3  ; legacy check
  goto UninstDone

  ;UninstWinamp3:
  ;  WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp3 "Install_Dir" "$INSTDIR"
  ;  WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp3 "Install_Plugins_Dir" $1
  ;  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp3" "DisplayName" "${PLUGIN_NAME} for Winamp 3 (remove only)"
  ;  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp3" "UninstallString" '"$INSTDIR\uninst-${PLUGIN_DLL}.exe"'
  ;  WriteUninstaller "$INSTDIR\uninst-${PLUGIN_DLL}.exe"
  ;  CreateDirectory "$SMPROGRAMS\Winamp3"
  ;  CreateShortCut "$SMPROGRAMS\Winamp3\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" "" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" 0
  ;  goto UninstDone
  
  UninstWinamp2:
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM SOFTWARE\${PLUGIN_DLL}Winamp "Install_Plugins_Dir" $1
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp" "DisplayName" "${PLUGIN_NAME} for Winamp 2x (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp" "UninstallString" '"$INSTDIR\uninst-${PLUGIN_DLL}.exe"'
    WriteUninstaller "$INSTDIR\uninst-${PLUGIN_DLL}.exe"
    CreateDirectory "$SMPROGRAMS\Winamp"
    CreateShortCut "$SMPROGRAMS\Winamp\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" "" "$INSTDIR\uninst-${PLUGIN_DLL}.exe" 0
    goto UninstDone
    
  UninstDone:

SectionEnd

;----------------------------------------------------------------------

UninstallText "This will uninstall the ${PLUGIN_NAME} plugin.  Hit next to continue."
ShowUninstDetails Show

Section "Uninstall"

  ; This section is the code that will be run when the user goes
  ; to Uninstall the plugin.

  IfFileExists $INSTDIR\Winamp.exe UninstStep1Winamp2
  ;IfFileExists $INSTDIR\Winamp3.exe UninstStep1Winamp3
  ;IfFileExists $INSTDIR\Studio.exe UninstStep1Winamp3  ; legacy check
  goto UninstScriptDone

  ;UninstStep1Winamp3:
  ;  ReadRegStr $1 HKLM SOFTWARE\${PLUGIN_DLL}Winamp3 "Install_Plugins_Dir"
  ;  goto UninstStep2
  UninstStep1Winamp2:
    ReadRegStr $1 HKLM SOFTWARE\${PLUGIN_DLL}Winamp "Install_Plugins_Dir"
    goto UninstStep2

  UninstStep2:
  
    ;----------------------------------------------------------------
    ; LIST OF FILES TO DELETE WHEN USER RUNS THE UNINSTALL:
    ;----------------------------------------------------------------

    ; IMPORTANT NOTE: You don't want to remove 'vms_desktop.dll' here
    ;   because it is a shared file that other VMS-based plugins
    ;   will also use!  So, PLEASE DO NOT REMOVE THIS FILE IN YOUR 
    ;   UNINSTALLER, since it might break other plugins.  (The file
    ;   is only 48 kb anyway.)
    
    ; First, delete the main plugin DLL and the documentation:
    
    Delete "$1\${PLUGIN_DLL}"                  ; the plugin
    Delete "$1\geiss_about.txt"
    Delete "$1\geiss_faq.txt"
    Delete "$1\geiss_whatsnew.txt"
    
    
    ; Then clean up the files we put in a subdirectory,
    ; and remove the subdirectory (if empty):
    
    ;Delete "$1\mysubdir\*.jpg"                 ; some misc. data files in a subdirectory
    ;Delete "$1\mysubdir\*.dat"                 ; some misc. data files in a subdirectory
    ;RMDir  "$1\mysubdir                        ; remove the subdirectory (if empty)
    
    ;----------------------------------------------------------------

  IfFileExists $INSTDIR\Winamp.exe UninstStep3Winamp2
  ;IfFileExists $INSTDIR\Winamp3.exe UninstStep3Winamp3
  ;IfFileExists $INSTDIR\Studio.exe UninstStep3Winamp3  ; legacy check
  goto UninstScriptDone
    
  ;UninstStep3Winamp3:
  ;  DeleteRegKey HKLM SOFTWARE\${PLUGIN_DLL}Winamp3
  ;  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp3"
  ;  Delete $INSTDIR\uninst-${PLUGIN_DLL}.exe
  ;  Delete "$SMPROGRAMS\Winamp3\Uninstall ${PLUGIN_NAME}.lnk"
  ;  RMDir "$SMPROGRAMS\Winamp3"
  ;  goto UninstScriptDone
  UninstStep3Winamp2:
    DeleteRegKey HKLM SOFTWARE\${PLUGIN_DLL}Winamp
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_DLL}Winamp"
    Delete $INSTDIR\uninst-${PLUGIN_DLL}.exe
    Delete "$SMPROGRAMS\Winamp\Uninstall ${PLUGIN_NAME}.lnk"
    RMDir "$SMPROGRAMS\Winamp"
    goto UninstScriptDone

  UninstScriptDone:
SectionEnd

;----------------------------------------------------------------------

Function .onInstSuccess
  ;IfFileExists $INSTDIR\Winamp.exe CaseWinamp2
  ;IfFileExists $INSTDIR\Winamp3.exe CaseWinamp3
  ;IfFileExists $INSTDIR\Studio.exe CaseWinamp3  ; legacy check
  ;goto CaseImpossible
  
  CaseWinamp2:
  CloseWinamp2:
    ; make sure winamp is closed before we try to edit winamp.ini
    FindWindow $R0 "winamp v1.x"
    StrCmp $R0 0 "" WarnNotSetAsDefault     ;RequestCloseWinamp2
    goto Winamp2Closed
  ;RequestCloseWinamp2:
  ;  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Winamp is currently running; please close it, and then hit OK.$\r$\r(Or click 'cancel' if you prefer to manually set your new plugin$\ras the active visualizer (CTRL+K), from within Winamp.)" \
  ;      IDCANCEL WarnNotSetAsDefault
  ;  goto CloseWinamp2
  Winamp2Closed:    
    ;WriteINIStr "$INSTDIR\Winamp.ini" "Winamp" "visplugin_name" ${PLUGIN_DLL}
    ;WriteINIStr "$INSTDIR\Winamp.ini" "Winamp" "visplugin_num" "0"
    MessageBox MB_YESNO|MB_ICONQUESTION \
            "The plugin was installed successfully.$\r$\rTO FINISH SETUP:$\r   From Winamp, hit CTRL+K to bring up the music visualizer screen.$\r   From there, you can select, configure, and run your new plugin.$\r$\rWould you like to run Winamp now?" \
            IDNO end
        Exec '"$INSTDIR\Winamp.exe"'
    Goto end
  
  
  WarnNotSetAsDefault:
    MessageBox MB_OK|MB_ICONEXCLAMATION "TO FINISH SETUP:$\r   From Winamp, hit CTRL+K to bring up the music visualizer screen.$\r   From there, you can select, configure, and run your new plugin."
    goto end
  
  ;CaseImpossible:  
  ;  MessageBox MB_OK|MB_ICONEXCLAMATION "ERROR: unable to find winamp.exe in the install directory..."
  ;  Goto end
    
  End:
FunctionEnd

; eof

