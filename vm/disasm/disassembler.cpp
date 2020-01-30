/* Copyright 1994 - 1996 LongView Technologies L.L.C. $Revision: 1.35 $ */
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

#include "incls/_precompiled.incl"
#ifdef DELTA_COMPILER
#include "incls/_disassembler.cpp.incl"

//#include <inttypes.h>

#define DISASM_LIBRARY  "libnasm"
#define DISASM_FUNCTION "disasm"

#define MAX_HEXBUF_SIZE 256
#define MAX_OUTBUF_SIZE 256

typedef int32_t (*disasm_f) (uint8_t *, char *, int, int, int32_t, int, uint32_t);

static void initialize(void);
static char tohex(unsigned char c);
static char* bintohex(char *data, int bytes);

static void printRelocInfo(relocIterator* iter, outputStream* st);
static void printRelocInfo(nmethod* nm, char* pc, int lendis, outputStream* st);
static void printPcDescInfo(nmethod* nm, char* pc, outputStream* st);

static void disasm(char* begin, char* end, nmethod* nm, outputStream* st);

static disasm_f disassemble;
static bool library_loaded = false;

/* default parameters for the NASM disassembler */
static int32_t   offset    = 0;
static int       autosync  = 0;
static uint32_t  prefer    = 0;    // select instruction set; 0 = Intel (default)  


static void initialize(void) {
  DLL* library_handle;
  char libname[13];
  char* extension = os::dll_extension();
  strcpy(libname, DISASM_LIBRARY);
  strcpy(libname+7, extension);
  libname[7+strlen(extension)] = '\0';
          
  library_handle = os::dll_load(libname);
  if (library_handle == NULL) {
    return;
  }
  disassemble = (disasm_f) os::dll_lookup(DISASM_FUNCTION, library_handle);
  if (disassemble == NULL) {
    return;
  }
  library_loaded = true;
}

static char tohex(unsigned char c) {
  char* digits = "0123456789ABCDEF";
  if (c > 0xf) return '?';
  return digits[c];
}

static char* bintohex(char *data, int bytes) {
  static char buf[MAX_HEXBUF_SIZE];

  char* p = buf;
  while (bytes--) {
    *p++ = tohex((*data & 0xF0) >> 4);
    *p++ = tohex( *data & 0x0F);
    data++;
  }
  *p = '\0';
  return buf;
}

static void printRelocInfo(relocIterator* iter, outputStream* st) {
  primitive_desc* pd;
  char* target;
  int* addr;
  
  st->print("[reloc @ ");
  addr = iter->word_addr();
  switch (iter->type()) {
    case relocInfo::none:
      st->print("none");
      break;
      
    case relocInfo::oop_type:
      st->print("%p, embedded oop, ", addr);
      oop(*addr)->print_value();
      break;

    case relocInfo::ic_type:
      st->print("%p, inline cache", addr);
      break;
      
    case relocInfo::prim_type:
      st->print("%p, primitive call, ", addr);
      target = (char*) (*addr + (intptr_t) addr + oopSize);
      pd = primitives::lookup((fntype) target);
      if (pd != NULL) {
        st->print("(%s)", pd->name());
      } else {
        st->print("runtime routine");
      }
      break;
      
    case relocInfo::runtime_call_type:
      st->print("%p, runtime call", addr);
      break;
      
    case relocInfo::external_word_type:
      st->print("%p, external word", addr);
      break;
      
    case relocInfo::internal_word_type:
      st->print("%p, internal word", addr);
      break;
      
    case relocInfo::uncommon_type:
      st->print("%p, uncommon trap ", addr);
      if (iter->wasUncommonTrapExecuted())
        st->print(" (taken)");
      else
        st->print(" (not taken)");
      break;
      
    case relocInfo::dll_type:
      st->print("%p, dll", addr);
      break;

    default:
      st->print("???");
      break;
  }
  st->print("]");
}

static void printRelocInfo(nmethod* nm, char* pc, int lendis, outputStream* st) {
  relocIterator iter(nm);
  char *addr;
  
  while (iter.next()) {
    addr = (char*) iter.word_addr();
    if (addr > pc && addr < (pc + lendis)) {
      printRelocInfo(&iter, st);
      break;
    }
  }
}

static void printPcDescInfo(nmethod* nm, char* pc, outputStream* st) {
  PcDesc* pcs;

  pcs = nm->containingPcDesc(pc, NULL);
  if (pcs) {
    st->print("bc = %03ld ", pcs->byteCode);
    if (pcs->is_prologue()) {
      st->print("prologue "); 
    } else if (pcs->is_epilogue()) {
      st->print("epilogue ");
    }
  }
}


static void disasm(char* begin, char* end, nmethod* nm, outputStream* st) {
  static char buf[MAX_OUTBUF_SIZE];
  int lendis;      
  
  if (!library_loaded) {
    initialize();
  }
  if (disassemble) {
    for (char* pc = begin; pc < end; pc += lendis) {
      lendis = disassemble((uint8_t*) pc, buf, sizeof(buf), 32, offset, autosync, prefer);
      if (lendis) {
        st->print("%p %-20s    %-40s", pc, bintohex(pc, lendis), buf);
        if (nm) {
          st->print("; ");
          printPcDescInfo(nm, pc, st);
          printRelocInfo(nm, pc, lendis, st);
        }
      }
      st->cr();
    }
  } else {
    st->print_cr("INFO: no disassemble() function available!");
  }  
}

void Disassembler::decode(nmethod* nm, outputStream* st) {
  disasm(nm->insts(), nm->instsEnd(), nm, st);
}

void Disassembler::decode(char* begin, char* end, outputStream* st) {
  disasm(begin, end, NULL, st);
}

#endif
