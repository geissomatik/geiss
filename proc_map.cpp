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

#ifndef GEISS_PROCESS_MAP 
#define GEISS_PROCESS_MAP 1

#include "proc_map.h"
#include <ddraw.h>

//-----for slider--------
//#include <math.h>
//extern int volpos;
//extern long intframe;
extern int slider1;
//extern int slider2;
extern bool bMMX;
//-----------------------



/*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  VERY, VERY IMPORTANT:
  You must build this with Visual Studio 6.0.
  Visual Studio 2003 doesn't seem to let you read code segments ! ! !
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
*/

//------------------------------------------------------------------
static void *realAddress(void *fn)
{
#ifdef _DEBUG
	// Debug Mode
  char *ptr = (char *)fn;
  return ptr + (*(int *)(ptr+1))+5;
#else
  // Release Mode
	return fn;
#endif
}

//------------------------------------------------------------------
int ALIGNCODE(int n, unsigned char *dest_code_buffer)
{
    int ret = 0;
    while ((DWORD)dest_code_buffer % n)
    {
        *dest_code_buffer++ = 0x90; // NOP
        ret++;        
    }
    return ret;
}

//------------------------------------------------------------------
int ADDBLOCK(unsigned char* fn, unsigned char *dest_code_buffer) 
{
  fn = (unsigned char *)realAddress(fn);
  int ret = 0;

  for (ret=0; ret<4; ret++)
      dest_code_buffer[ret] = fn[ret];

  do
  {
      dest_code_buffer[ret] = fn[ret];
      ret++;
      if (fn[ret-5]==0x68 &&       // watch for "push 0xDEADBEEF" end sequence
          fn[ret-4]==0xEF &&
          fn[ret-3]==0xBE &&
          fn[ret-2]==0xAD &&
          fn[ret-1]==0xDE
         )
         break;
  }
  while (1);
  return ret - 5;
}

//------------------------------------------------------------------
int FixJumpTarget(unsigned char* code, int start_byte, int end_byte, int new_target_addr)
{
  // finds the first occurrence of 'jnz 0' and replaces it with a 
  // [properly relative] jump to new_target_addr.
  // returns the byte offset of the beginning of the next instruction AFTER
  // the adjusted "jnz 0" instruction, or 0 on failure to find one.
  for (int i=start_byte; i<end_byte-5; i++)
  {
      if (code[i] == 0x0F && code[i+1] == 0x85)
      {
          int* p = (int*)(&code[i+2]);
          if (*p == 0)
          {
              *p = new_target_addr - i - 6;
              return i+6;
          }
      }
  }
  return 0;
}

//------------------------------------------------------------------
int ReplaceDWORD(unsigned char* code, int start_byte, int end_byte, 
				 DWORD old_value, DWORD new_value)
{
  for (int i=start_byte; i<end_byte-4; i++)
  {
	  if (*(DWORD*)(&code[i]) == old_value)
	  {
		  *(DWORD*)(&code[i]) = new_value;
		  return i;
	  }
  }
  return 0;
}

//------------------------------------------------------------------
__declspec ( naked ) void _return_baby(void) { __asm { 

    ret

    // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS:
    push 0xDEADBEEF  // 0x68DEADBEEF
    emms
  }                      
}



void* g_proc_map_p1;
void* g_proc_map_p2;

//------------------------------------------------------------------
__declspec ( naked ) void _proc_map_8bit_part01(void) { __asm {

	mov   ecx, FX_YCUT_NUM_LINES    // our "loop counter", goes down to 0

	push  esi
	push  edi
	mov   ESI, DATA_FX
	mov   EDI, g_proc_map_p1
 	mov   EBX, g_proc_map_p2		// will need this momentarily...
	add   ESI, FX_YCUT_xFXW_x8
 		//add   EDI, FX_YCUT_xFXW
		//add   EDI, initial_map_offset
	      add   EDI, slider1			// we start further into the transformation map...
		//sub   EDI, slider2
	    //add   EBX, slider			// we start further into the VS...
	add   EBX, FX_YCUT_xFXW
	push  ebp

	mov  EBP, ebx

	// ESI: data array; ++8
	// EDI: source screen; will be mutilitated and fixed repeatedly
	// EBP: dest. screen; ++1

	//xor   eax, eax
	//sub   EDI, 32767
	mov   eax, [ESI+4]
dec   ebp

      shr   ecx, 1
//sub   ecx, 256

ALIGN 8

      CyrixLoopA:

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part02(void) { __asm {
														//---------------------------------------------------------------

		//xor   eax, eax
		mov   eax, [ESI+4]
		xor   dh, dh
		add   EDI, eax			// now EDI points to VS1:topleft.  upper 16 bits of eax always stay at zero.

		// top left and top right pixels
		mov		  ebx, [ESI]

		mov       al, [EDI]
		mul       bl
            inc       ebp
		add       dx, ax

		mov       al, [EDI+1]
		mul       bh
			shr       ebx, 16
		add       dx, ax

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part03(void) { __asm {
														//---------------------------------------------------------------

		// btm left and btm right pixels
		mov       al, [EDI+23456]
		mul       bl
			add	   esi, 8
		add       dx, ax

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part04(void) { __asm {
														//---------------------------------------------------------------

		mov       al, [EDI+23457]
		mul       bh
			//sub       EDI, 32767
		add       dx, ax		
//dec dh
		// now DH stores the averaged color value.

    		          //xor   eax, eax
				mov   eax, [ESI+4]
		mov   [ebp], dh			// plot
			          xor   dh, dh        
			          add   EDI, eax			// now EDI points to VS1:topleft.  upper 16 bits of eax always stay at zero.

			          // top left and top right pixels
			          mov		  ebx, [ESI]

			          mov       ax, [EDI]
			          mul       bl
			            dec   ecx 
						inc       ebp
			          add       dx, ax

			          mov       al, [EDI+1]
			          mul       bh
			            shr      ebx, 16
			          add       dx, ax

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part05(void) { __asm {
														//---------------------------------------------------------------

			          // btm left and btm right pixels
			          mov       al, [EDI+23456]
			          mul       bl
			            add	   esi, 8
			          add       dx, ax

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part06(void) { __asm {
														//---------------------------------------------------------------

			          mov       al, [EDI+23457]
			          mul       bh
			            //sub       EDI, 32767
			          add       dx, ax		
//dec dh
			          // now DH stores the averaged color value.

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part07(void) { __asm {
														//---------------------------------------------------------------

					test     cl, 63
			          mov   [ebp], dh			// plot
					jnz      0 //CyrixLoopA                 // every 256th pixel do some prefetches

					mov      ebx, 8*128                // need to prefetch 256*6 bytes

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part08(void) { __asm {
														//---------------------------------------------------------------

			CYRIX_CACHE_ESI_A:
					mov      eax, [ebx+esi]
					mov      eax, [ebx+esi+32]
					mov      eax, [ebx+esi+64]
					mov      eax, [ebx+esi+96]
					sub      ebx, 128
					jnz      0 //CYRIX_CACHE_ESI_A
					xor      eax, eax

														//---------------------------------------------------------------
														push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
														emms
														} } __declspec ( naked ) void _proc_map_8bit_part09(void) { __asm {
														//---------------------------------------------------------------

						sub   ecx, 0
						jnz   0// CyrixLoopA

	  pop  ebp
	  pop  edi
	  pop  esi

    // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS:
    push 0xDEADBEEF  // 0x68DEADBEEF
	emms
  }                      
}





//------------------------------------------------------------------
__declspec ( naked ) void _proc_map_32bit_part01(void) { __asm {

			mov   ecx, FX_YCUT_NUM_LINES    // our "loop counter", goes down to 0

			push  esi
			push  edi
			mov   ESI, DATA_FX          // data_fx[]
			 add  ESI, FX_YCUT_xFXW_x8
			mov   EDI, g_proc_map_p1//p1               // lookat pixel pointer
 			 //add  EDI, FX_YCUT_xFXW    
			 //add  EDI, FX_YCUT_xFXW
			 //add  EDI, FX_YCUT_xFXW
			 //add  EDI, FX_YCUT_xFXW
			 //add  EDI, initial_map_offset
			 //add  EDI, initial_map_offset
			 //add  EDI, initial_map_offset
			 //add  EDI, initial_map_offset
			 add  EDI, slider1
			 add  EDI, slider1
			 add  EDI, slider1
			 add  EDI, slider1
			 //sub  EDI, slider2
			 //sub  EDI, slider2
			 //sub  EDI, slider2
			 //sub  EDI, slider2

 			mov   EBX, g_proc_map_p2//p2		        // dest pixel pointer (will be EBP)
			 add  EBX, FX_YCUT_xFXW
			 add  EBX, FX_YCUT_xFXW
			 add  EBX, FX_YCUT_xFXW
			 add  EBX, FX_YCUT_xFXW
			//add  EDI, slider			// we start further into the transformation map...
			//add  EBX, slider			// we start further into the VS...
			push  ebp

			mov  EBP, ebx

			// ESI: data array; ++8
			// EDI: source screen; will be mutilitated and fixed repeatedly
			// EBP: dest. screen; ++1

			//sub   EDI, 32767*4

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part02(void) { __asm {
																//---------------------------------------------------------------

			NewPixelLoop1:
					
				xor   eax, eax
				mov   eax, [ESI+4]	// amount to move lookat pixel + 32767 (we pre-subtracted)
				//shl   eax, 2        // move that # times 4  
				add   EDI, eax      // apply; now EDI points to VS1:topleft.  upper 16 bits of eax always stay at zero.

				mov   ebx, [ESI]    // the 4 weights

				// ------------ RED PIXEL ---------------------------------------

				// top left and top right pixels
				mov       al, [EDI]
			   xor       dh, dh            // INIT: leave some random dither
				mul       bl
				add       dx, ax

				mov       al, [EDI+4]
				mul       bh
				 ror      ebx, 16
				add       dx, ax

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part03(void) { __asm {
																//---------------------------------------------------------------

				// btm left and btm right pixels
				mov       al, [EDI+17387]//[EDI+320*4]
				mul       bl
				add       dx, ax

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part04(void) { __asm {
																//---------------------------------------------------------------

				mov       al, [EDI+17388]//[EDI+320*4+4]
				mul       bh
				add       dx, ax		// u-pipe

				// now DH stores the averaged color value.
				mov   [ebp], dh			// plot

				// ----------- GREEN PIXEL --------------------------------------
			   inc EDI      // INIT: lookat pixel
			   rol EBX, 16  // INIT: start w/weights in original place
			   //inc EBP    // INIT: dest pixel
			   //xor dh, dh // INIT: leave some random dither

				// top left and top right pixels
				mov       al, [EDI]
			   xor       dh, dh         // INIT:leave some random dither
				mul       bl
			   inc       EBP			// INIT: dest pixel
				add       dx, ax

				mov       al, [EDI+4]
				mul       bh
				 ror      ebx, 16
				add       dx, ax

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part05(void) { __asm {
																//---------------------------------------------------------------

				// btm left and btm right pixels
				mov       al, [EDI+17387]//[EDI+320*4]
				mul       bl
				add       dx, ax

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part06(void) { __asm {
																//---------------------------------------------------------------

				mov       al, [EDI+17388]//[EDI+320*4+4]
				mul       bh
				add       dx, ax

				// now DH stores the averaged color value.
				mov   [ebp], dh			// plot
        
				// ----------- BLUE PIXEL --------------------------------------
			   inc EDI      // INIT: lookat pixel
			   rol EBX, 16  // INIT: start w/weights in original place
			   //inc EBP    // INIT: dest pixel
			   //xor dh, dh // INIT: leave some random dither
       
				// top left and top right pixels
				mov       al, [EDI]
			   xor       dh, dh         // INIT:leave some random dither
				mul       bl
			   inc       EBP    // INIT: dest pixel
				add       dx, ax

				mov       al, [EDI+4]
				mul       bh
				 ror      ebx, 16
				add       dx, ax

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part07(void) { __asm {
																//---------------------------------------------------------------

				// btm left and btm right pixels
				mov       al, [EDI+17387]//[EDI+320*4]
				mul       bl
			   add	     esi, 8          // CLEANUP: move along through DATA_FX[]
				add       dx, ax

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part08(void) { __asm {
																//---------------------------------------------------------------

				mov       al, [EDI+17388]//[EDI+320*4+4]
				mul       bh
			   //sub       EDI, 32767*4+2  // CLEANUP: reset lookat pixel to what it should be & pre-subtract
			   sub       EDI, 2  // CLEANUP: reset lookat pixel to what it should be & pre-subtract
				add       dx, ax		// u-pipe

				// now DH stores the averaged color value.
				mov   [ebp], dh			// plot

				// ----------- CLEANUP -----------------------------------------
				add       EBP, 2          // CLEANUP: dest pixel should be @ new position
				//add     esi, 6          // CLEANUP: move along through DATA_FX[]
				//sub     EDI, 32767*4+2  // CLEANUP: reset lookat pixel to what it should be & pre-subtract

																//---------------------------------------------------------------
																push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
																emms
																} } __declspec ( naked ) void _proc_map_32bit_part09(void) { __asm {
																//---------------------------------------------------------------

				dec   ecx
				jnz   0 //NewPixelLoop1

			pop  ebp
			pop  edi
			pop  esi

			push 0xDEADBEEF // MANDATORY FOOTER FOR ALL OUR NAKED FUNCTIONS: 0x68DEADBEEF      
			emms
		} 
}
//---------------------------------------------------------------












// DON'T CALL THIS FUNCTION DIRECTLY
void Process_Map_Asm(void *p1, void *p2)//, LPDIRECTDRAWSURFACE lpDDSurf)
// DON'T CALL THIS FUNCTION DIRECTLY
{
    //int slider = (!bScrollMap) ? 0 : FXW/2 + FXW/3*sin(intframe*0.059) + FXW/7*sin(intframe*0.044);
	unsigned __int16 NUM_LINES = (unsigned __int16)((FXH-FX_YCUT*2));
	unsigned __int16 NUM_PIXELS = (unsigned __int16)(FXW);
	unsigned __int32 SCANLINE_PADDING = 0;
	unsigned __int32 FX_YCUT_xPITCH = 0;
	void *video = NULL;

    g_proc_map_p1 = p1;
    g_proc_map_p2 = p2;

    // ------------- begin building custom code seg -----------------
	if (iDispBits == 8)
	{
		// copy the code into a data segment, fix up the jump targets
		// and hard-code our custom variables into the instructions (...our
		// solution for not having enough registers), then execute the code.

		unsigned char code[8192];
		int codepos=0;
    
		int start_codepos[16];

		//----------------
		// Paste all the function chunks together into one piece of code.
		//----------------
		codepos += ALIGNCODE(16, &code[codepos]);
		start_codepos[1] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part01, &code[codepos]); 
		start_codepos[2] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part02, &code[codepos]); 
		start_codepos[3] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part03, &code[codepos]); 
		start_codepos[4] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part04, &code[codepos]); 
		start_codepos[5] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part05, &code[codepos]); 
		start_codepos[6] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part06, &code[codepos]); 
		start_codepos[7] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part07, &code[codepos]); 
		start_codepos[8] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part08, &code[codepos]); 
		start_codepos[9] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_8bit_part09, &code[codepos]); 
		start_codepos[10] = codepos; codepos += ADDBLOCK((unsigned char *)_return_baby, &code[codepos]); 
		start_codepos[11] = codepos; 
		
		int ret;

		//----------------
		// Fix up the "jnz 0" instructions
		//----------------
		ret = FixJumpTarget(code, start_codepos[7], start_codepos[ 8], start_codepos[2]);
		ret = FixJumpTarget(code, start_codepos[8], start_codepos[ 9], start_codepos[8]);
		ret = FixJumpTarget(code, start_codepos[9], start_codepos[10], start_codepos[2]);

		//----------------
		// Write some custom variables
		//----------------
		ret = ReplaceDWORD(code, start_codepos[3], start_codepos[4], 23456, FXW  );
		ret = ReplaceDWORD(code, start_codepos[4], start_codepos[5], 23457, FXW+1);
		ret = ReplaceDWORD(code, start_codepos[5], start_codepos[6], 23456, FXW  );
		ret = ReplaceDWORD(code, start_codepos[6], start_codepos[7], 23457, FXW+1);

		//----------------
		// EXECUTE
		//----------------
		void* start_ptr = (void*)code;
		__asm 
		{
			pusha           // release builds will break without this!
			call start_ptr
			popa            // release builds will break without this!
			emms
		};
	}
	else //32-bit case
	{
		if (!bMMX)
		{
			// copy the code into a data segment, fix up the jump targets
			// and hard-code our custom variables into the instructions (...our
			// solution for not having enough registers), then execute the code.

			unsigned char code[8192];
			int codepos=0;
    
			int start_codepos[16];

			//----------------
			// Paste all the function chunks together into one piece of code.
			//----------------
			codepos += ALIGNCODE(16, &code[codepos]);
			start_codepos[1] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part01, &code[codepos]); 
			start_codepos[2] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part02, &code[codepos]); 
			start_codepos[3] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part03, &code[codepos]); 
			start_codepos[4] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part04, &code[codepos]); 
			start_codepos[5] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part05, &code[codepos]); 
			start_codepos[6] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part06, &code[codepos]); 
			start_codepos[7] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part07, &code[codepos]); 
			start_codepos[8] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part08, &code[codepos]); 
			start_codepos[9] = codepos; codepos += ADDBLOCK((unsigned char *)_proc_map_32bit_part09, &code[codepos]); 
			start_codepos[10] = codepos; codepos += ADDBLOCK((unsigned char *)_return_baby, &code[codepos]); 
			start_codepos[11] = codepos; 
			
			int ret;

			//----------------
			// Fix up the "jnz 0" instructions
			//----------------
			ret = FixJumpTarget(code, start_codepos[9], start_codepos[10], start_codepos[2]);

			//----------------
			// Write some custom variables
			//----------------
			ret = ReplaceDWORD(code, start_codepos[3], start_codepos[4], 17387, FXW*4  );
			ret = ReplaceDWORD(code, start_codepos[4], start_codepos[5], 17388, FXW*4+4);
			ret = ReplaceDWORD(code, start_codepos[5], start_codepos[6], 17387, FXW*4  );
			ret = ReplaceDWORD(code, start_codepos[6], start_codepos[7], 17388, FXW*4+4);
			ret = ReplaceDWORD(code, start_codepos[7], start_codepos[8], 17387, FXW*4  );
			ret = ReplaceDWORD(code, start_codepos[8], start_codepos[9], 17388, FXW*4+4);

			//----------------
			// EXECUTE
			//----------------
			void* start_ptr = (void*)code;
			__asm 
			{
				pusha           // release builds will break without this!
				call start_ptr
				popa            // release builds will break without this!
				emms
			};
		}
		else // MMX version
		{
			__asm // MMX version.  Works 100%, except you must write the weights in the order +2+3+0+1 (vs. +0+1+2+3).
				  // Doesn't hold up very well when other windows are on the screen!
				  // Sort of sucks... it's only about 60% as fast as the integer version.
			{
				push  esi
				push  edi
				mov   ESI, DATA_FX          // data_fx[]
				 add  ESI, FX_YCUT_xFXW_x8
				mov   EDI, p1               // lookat pixel pointer
 				 //add  EDI, FX_YCUT_xFXW    
				 //add  EDI, FX_YCUT_xFXW
				 //add  EDI, FX_YCUT_xFXW
				 //add  EDI, FX_YCUT_xFXW
				 //add  EDI, initial_map_offset
				 //add  EDI, initial_map_offset
				 //add  EDI, initial_map_offset
				 //add  EDI, initial_map_offset
				 add  EDI, slider1
				 add  EDI, slider1
				 add  EDI, slider1
				 add  EDI, slider1
				 //sub  EDI, slider2
				 //sub  EDI, slider2
				 //sub  EDI, slider2
				 //sub  EDI, slider2

 				mov   EBX, p2		        // dest pixel pointer (will be EBP)
				 add  EBX, FX_YCUT_xFXW
				 add  EBX, FX_YCUT_xFXW
				 add  EBX, FX_YCUT_xFXW
				 add  EBX, FX_YCUT_xFXW
				//add  EDI, slider			// we start further into the transformation map...
				//add  EBX, slider			// we start further into the VS...

				// ESI: data array; ++8
				// EDI: source screen; will be mutilitated and fixed repeatedly
				// EBX: dest. screen; ++1
				// EBP: video mem pointer

				//sub   EDI, 32767*4
				mov   cx, NUM_LINES    // our "loop counter", goes down to 0

				mov   edx, FXW
				add   edx, edx
				add   edx, edx	// edx holds FXW*4

//vid				mov   eax, video
//vid				 add  eax, FX_YCUT_xPITCH
				push  ebp
//vid				mov   ebp, eax
				pxor      mm7, mm7

				ALIGN 8
				ScanlineLoop2:

//vid				    mov  eax, ebp    // back up our video mem pointer
				 	pop  ebp         // get base pointer back (to get to NUM_PIXELS)
					 shl  ecx, 16
					 mov  cx, NUM_PIXELS
					 // also, add our scanline padding =)
//vid				     add  eax, SCANLINE_PADDING
				    push ebp         // save base pointer onto stack
//vid				    mov  ebp, eax	 // restore video mem pointer

				ALIGN 8
				PixelLoop2:

			     test     bx, 0x03ff
			     jnz      PROCESS_PIXEL_MMX32           // every 256th pixel do some prefetches

						push ebx
						 mov      ebx, 8*256                 // need to prefetch 256*6 bytes
					    LOAD_ESI_ARRAY_MMX32:
						 mov      eax, [ebx+esi]
						 mov      eax, [ebx+esi+32]
						 sub      ebx, 64
						 jnz      LOAD_ESI_ARRAY_MMX32
						pop ebx

				 PROCESS_PIXEL_MMX32:

				/*
				  // BACKUP OF UNINTERLEAVED 32-BIT MMX LOOP
				  // BACKUP OF UNINTERLEAVED 32-BIT MMX LOOP
				  // BACKUP OF UNINTERLEAVED 32-BIT MMX LOOP

				  movq      mm0, dword ptr [esi]      ;    0    0    0    0    c3   c2   c1   c0
				   pxor      mm7, mm7
				  punpcklbw mm0, mm0                  ;    c3   c3   c2   c2   c1   c1   c0   c0
				  movq      mm2, mm0                  ;    c3   c3   c2   c2   c1   c1   c0   c0
				  punpcklbw mm0, mm0                  ;    c1   c1   c1   c1   c0   c0   c0   c0
				  punpckhbw mm2, mm2                  ;    c3   c3   c3   c3   c2   c2   c2   c2
				  movq      mm1, mm0                  ;    c1   c1   c1   c1   c0   c0   c0   c0
				  movq      mm3, mm2                  ;    c3   c3   c3   c3   c2   c2   c2   c2
				  punpcklbw mm0, mm7                  ;    0    c0   0    c0   0    c0   0    c0
				  punpckhbw mm1, mm7                  ;    0    c1   0    c1   0    c1   0    c1
				  punpcklbw mm2, mm7                  ;    0    c2   0    c2   0    c2   0    c2
				  punpckhbw mm3, mm7                  ;    0    c3   0    c3   0    c3   0    c3

				  // fake (equal) weights:
				  //mov        eax, 0x40404040
				  //movd       mm0, eax                 ;    0    0    0    0    64   64   64   64
				  //punpcklbw  mm0, mm0                 ;    64   64   64   64   64   64   64   64
				  //psrlw      mm0, 8                   ;    0    64   0    64   0    64   0    64
				  //movq       mm1, mm0
				  //movq       mm2, mm0
				  //movq       mm3, mm0

				  //	now get the colors, multiply, and add:
				  movq      mm4, qword ptr [edi]      ;    a1   b1   g1   r1   a0   b0   g0   r0
				  movq      mm5, mm4                  ;    a1   b1   g1   r1   a0   b0   g0   r0
				  punpcklbw mm4, mm7                  ;    0    a0   0    b0   0    g0   0    r0
				  punpckhbw mm5, mm7                  ;    0    a1   0    b1   0    g1   0    r1
				  pmullw    mm0, mm4                  ;    a0*c0     b0*c0     g0*c0     r0*c0
				  pmullw    mm1, mm5                  ;    a1*c1     b1*c1     g1*c1     r1*c1

				  movq      mm4, qword ptr [edi+edx]  ;    a3   b3   g3   r3   a2   b2   g2   r2
				  movq      mm5, mm4                  ;    a3   b3   g3   r3   a2   b2   g2   r2
				  punpcklbw mm4, mm7                  ;    0    a2   0    b2   0    g2   0    r2
				  punpckhbw mm5, mm7                  ;    0    a3   0    b3   0    g3   0    r3
				  pmullw    mm2, mm4                  ;    a2*c2     b2*c2     g2*c2   r2*c2
				  pmullw    mm3, mm5                  ;    a3*c3     b3*c3     g3*c3   r3*c3
				  paddw          mm0, mm1
				  paddw          mm0, mm2
				  paddw          mm0, mm3             ;    a'        b'        g'        r'
  
				//now get the colors
				//xor   eax, eax
				//mov   ax, [ESI+4]
				//shl   eax, 2        // move that # times 4  
				//add   EDI, eax

				//PUNPCKHBW MM4, [EDI]
				//PUNPCKHBW MM5, [EDI+4]
				//PUNPCKHBW MM6, [EDI+640*4]
				//PUNPCKHBW MM7, [EDI+641*4]
				//PSRLW mm4, 8
				//PSRLW mm5, 8
				//PSRLW mm6, 8
				//PSRLW mm7, 8

				//PMULLW    MM0, MM4
				//PMULLW    MM1, MM5
				//PMULLW    MM2, MM6
				//PMULLW    MM3, MM7
				//PADDW     MM0, MM1
				//PADDW     MM2, MM3
				//PADDW     MM0, MM2

				psrlw     mm0, 8               ;    0    a'   0    b'   0    g'   0    r'
					sub   EDI, 32767*4
				packuswb  mm0, mm0             ;    a'   b'   g'   r'   a'   b'   g'   r'
					add   ESI, 6
				movd      dword ptr [ebx], mm0 ; store to the destination array
					add   EBP, 4

					dec   ecx
					jnz   PixelLoop2
				*/

						 /*
				xor   eax, eax
				  movq      mm0, dword ptr [esi]      ;    0    0    0    0    c3   c2   c1   c0
				mov   eax, [ESI+4]
				  punpcklbw mm0, mm0                  ;    c3   c3   c2   c2   c1   c1   c0   c0
				  //pxor      mm7, mm7
				//shl   eax, 2        // move that # times 4  
				  movq      mm2, mm0                  ;    c3   c3   c2   c2   c1   c1   c0   c0
				add   EDI, eax
				  punpcklbw mm0, mm0                  ;    c1   c1   c1   c1   c0   c0   c0   c0
          movq      mm4, qword ptr [edi]      ;    a1   b1   g1   r1   a0   b0   g0   r0
				  punpckhbw mm2, mm2                  ;    c3   c3   c3   c3   c2   c2   c2   c2
				  movq      mm1, mm0                  ;    c1   c1   c1   c1   c0   c0   c0   c0
          movq      mm5, mm4                  ;    a1   b1   g1   r1   a0   b0   g0   r0
				  movq      mm3, mm2                  ;    c3   c3   c3   c3   c2   c2   c2   c2
          punpcklbw mm4, mm7                  ;    0    a0   0    b0   0    g0   0    r0
				  punpcklbw mm0, mm7                  ;    0    c0   0    c0   0    c0   0    c0
          punpckhbw mm5, mm7                  ;    0    a1   0    b1   0    g1   0    r1
				  punpckhbw mm1, mm7                  ;    0    c1   0    c1   0    c1   0    c1
          pmullw    mm0, mm4                  ;    a0*c0     b0*c0     g0*c0     r0*c0
				  punpcklbw mm2, mm7                  ;    0    c2   0    c2   0    c2   0    c2
          pmullw    mm1, mm5                  ;    a1*c1     b1*c1     g1*c1     r1*c1
				  punpckhbw mm3, mm7                  ;    0    c3   0    c3   0    c3   0    c3
				  */

				//xor   eax, eax
				  movq      mm0, dword ptr [esi]      ;    0    0    0    0    c3   c2   c1   c0
				mov   eax, [ESI+4]
				  punpcklbw mm0, mm0                  ;    c3   c3   c2   c2   c1   c1   c0   c0
				  //pxor      mm7, mm7
				//mov   eax, [ESI+4]
                //shl   eax, 2        // move that # times 4  
				add   EDI, eax
				  movq      mm2, mm0                  ;    c3   c3   c2   c2   c1   c1   c0   c0
				  punpcklbw mm0, mm0                  ;    c1   c1   c1   c1   c0   c0   c0   c0
          movq      mm4, qword ptr [edi]      ;    a1   b1   g1   r1   a0   b0   g0   r0
				  punpckhbw mm2, mm2                  ;    c3   c3   c3   c3   c2   c2   c2   c2
				  movq      mm1, mm0                  ;    c1   c1   c1   c1   c0   c0   c0   c0
          movq      mm5, mm4                  ;    a1   b1   g1   r1   a0   b0   g0   r0
				  movq      mm3, mm2                  ;    c3   c3   c3   c3   c2   c2   c2   c2
          punpcklbw mm4, mm7                  ;    0    a0   0    b0   0    g0   0    r0
				  punpcklbw mm0, mm7                  ;    0    c0   0    c0   0    c0   0    c0
          punpckhbw mm5, mm7                  ;    0    a1   0    b1   0    g1   0    r1
				  punpckhbw mm1, mm7                  ;    0    c1   0    c1   0    c1   0    c1
          pmullw    mm0, mm4                  ;    a0*c0     b0*c0     g0*c0     r0*c0
				  punpcklbw mm2, mm7                  ;    0    c2   0    c2   0    c2   0    c2
          pmullw    mm1, mm5                  ;    a1*c1     b1*c1     g1*c1     r1*c1
				  punpckhbw mm3, mm7                  ;    0    c3   0    c3   0    c3   0    c3

		  //	now get the colors, multiply, and add:

          movq      mm4, qword ptr [edi+edx]  ;    a3   b3   g3   r3   a2   b2   g2   r2
           paddw     mm0, mm1
          movq      mm5, mm4                  ;    a3   b3   g3   r3   a2   b2   g2   r2
          punpcklbw mm4, mm7                  ;    0    a2   0    b2   0    g2   0    r2
          punpckhbw mm5, mm7                  ;    0    a3   0    b3   0    g3   0    r3
           paddw     mm0, mm6 //DITHER 1
          pmullw    mm2, mm4                  ;    a2*c2     b2*c2     g2*c2   r2*c2
          pmullw    mm3, mm5                  ;    a3*c3     b3*c3     g3*c3   r3*c3
	       paddw     mm0, mm2
           paddw     mm0, mm3             ;    a'        b'        g'        r'
	       
		   movq      mm6, mm0 //DITHER 2


				// ---DITHER---
				//paddusw     mm0, mm6    ; DITHER: add old dither values
				//movq        mm6, mm0	; DITHER: copy sum
				//psllw       mm6, 8      ; DITHER: save low bits for next dither values
				//psrlw       mm6, 8      ; DITHER: save low bits for next dither values
				// ---DITHER---

				psrlw     mm0, 8               ;    0    a'   0    b'   0    g'   0    r'
			    psllw     mm6, 8  //DITHER 3
					//sub   EDI, 32767*4
				packuswb  mm0, mm0             ;    a'   b'   g'   r'   a'   b'   g'   r'
					add   ESI, 8
				movd      dword ptr [ebx], mm0 ; store to the destination array
					add   EBX, 4

					
				// store pseudo-random dither info for next time in EBP


//vid				PADDUSB   mm0, mm0
//vid				movd      dword ptr [ebp], mm0 ; store to VIDEO
//vid				add   EBP, 4

					dec   cx
				   psrlw     mm6, 8  //DITHER 4
					jnz   PixelLoop2

					shr   ecx, 16
					dec   cx
					jnz   ScanlineLoop2

				EMMS

				pop  ebp
				pop  edi
				pop  esi

			} // end MMX version
//vid            ddrval = lpDDSurf->Unlock(NULL);
//vid		  }
		}
	}
}




void Process_Map(void *p1, void *p2)//, LPDIRECTDRAWSURFACE lpDDSurf)
{
    float temp_clock2 = (float)clock();

    Process_Map_Asm(p1, p2);//, lpDDSurf);

    /*
    if (!bBypassAssembly)
	{
   	    Process_Map_Asm(p1, p2);
    }
    else
    {
        if (iDispBits == 8)
        {
            int x, y, count, dest_pixel;
            int delta_lookat, lookat, val;
            unsigned char *SRC = (unsigned char *)p1;
            unsigned char *DEST = (unsigned char *)p2;
            unsigned char *data = (unsigned char *)&DATA_FX[FX_YCUT_xFXW_x8];
            int *ddata = (int *)&DATA_FX[FX_YCUT_xFXW_x8];

            count = FX_YCUT_NUM_LINES;  // # of pixels to process
            lookat = 0;//FX_YCUT_xFXW;  // starting lookat pixel
            dest_pixel = FX_YCUT_xFXW;
            do
            {
                //------------------------------------------------------
                lookat += ddata[1];
                val = SRC[lookat]*data[0] + SRC[lookat+1]*data[1] + SRC[lookat+FXW]*data[2] + SRC[lookat+FXW+1]*data[3];
                DEST[dest_pixel++] = val >> 8;
                //------------------------------------------------------
        
                data  = &data[8];
                ddata = &ddata[2];
                count--;
            }
            while (count > 0);
        }
        else  // 32-bit code
        {
            int x, y, count, dest_pixel;
            int delta_lookat, lookat, val1, val2, val3;
            unsigned char *SRC = (unsigned char *)p1;
            unsigned char *DEST = (unsigned char *)p2;
            unsigned char *data = (unsigned char *)&DATA_FX[FX_YCUT_xFXW_x8];
            int *ddata = (int *)&DATA_FX[FX_YCUT_xFXW_x8];

            count      = FX_YCUT_NUM_LINES;  // # of tri-pixels to process
            lookat     = 0;//FX_YCUT*FXW*4;  // starting lookat pixel
            dest_pixel = FX_YCUT*FXW*4;
            
            do
            {
                //------------------------------------------------------
                lookat += ddata[1];//(ddata[1] << 2);

                val1 = (SRC[lookat]*data[0] + SRC[lookat+4]*data[1] + SRC[lookat+FXW*4]*data[2] + SRC[lookat+FXW*4+4]*data[3]) >> 8;
                lookat++;
                val2 = (SRC[lookat]*data[0] + SRC[lookat+4]*data[1] + SRC[lookat+FXW*4]*data[2] + SRC[lookat+FXW*4+4]*data[3]) >> 8;
                lookat++;
                val3 = (SRC[lookat]*data[0] + SRC[lookat+4]*data[1] + SRC[lookat+FXW*4]*data[2] + SRC[lookat+FXW*4+4]*data[3]) >> 8;
                lookat -= 2;

                DEST[dest_pixel++] = val1;
                DEST[dest_pixel++] = val2;
                DEST[dest_pixel++] = val3;
                dest_pixel++;
                //------------------------------------------------------
        
                data  = &data[8];
                ddata = &ddata[2];
                count--;
            }
            while (count > 0);
        }
    }
    /**/

    core_clock_time += clock() - temp_clock2;

}



#endif






