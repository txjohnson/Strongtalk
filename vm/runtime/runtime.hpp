#pragma once
/* Copyright 1994, 1995 LongView Technologies L.L.C. $Revision: 1.6 $ */
/* Copyright (c) 2006, Sun Microsystems, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
	  disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Sun Microsystems nor the names of its contributors may be used to endorse or promote products derived 
	  from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE


*/


/*
  Implementation of a function accessable from compiled code should
  follow the below structure (see verifyNLR for an example)
  
  // Microsoft Visual C++, Compiler option: Generate frame pointer
  #pragma optimize("y", on)

  extern "C" volatile void function_called_from_vm(...) {
    VM_StackMarker mark;
    .. code ...
  }

  // Microsoft Visual C++, Compiler option:  Default settings
  #pragma optimize("",  on)
*/

/*
  Pascal Calling Convention:
   A routine that uses the Pascal calling convention must preserve EBX, EBP, ESI, EDI.
   (extracted from Microsoft Development Library, MASM 6.1 Programmer's Guide)

  C Calling Convention:
   A routine that uses the C calling convention must preserve EBP, ESI, EDI.
*/

extern "C" char* byte_map_base;
extern "C" char* MaxSP;
inline void Set_Byte_Map_Base(char* base) {byte_map_base = base;}

inline void  setSPMax(char* m)        { MaxSP = m; }
inline char* currentSPMax()           { return MaxSP; }
