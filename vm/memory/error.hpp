#pragma once
/* Copyright 1994 - 1996 LongView Technologies L.L.C. $Revision: 1.8 $ */
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

// http://clang-analyzer.llvm.org/annotations.html#custom_assertions
#if defined(__clang_analyzer__) && ! defined(CLANG_ANALYZER_NORETURN)
  //#error "defining analyzer attribute"
  #define CLANG_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
#else
  #define CLANG_ANALYZER_NORETURN
#endif

#ifndef DEBUG_EXCEPTION
#define DEBUG_EXCEPTION error_breakpoint();
#endif

#ifdef PRODUCT
#define FILE_INFO "<no file info>"
#define LINE_INFO 0
#else
#define FILE_INFO __FILE__
#define LINE_INFO __LINE__
#endif



extern "C" {
  void breakpoint();		// called at every warning
  void error_breakpoint();	// called at every error or fatal
}

#ifdef ASSERT
#define assert(b,msg)                                            \
  if (!(b)) {                                                    \
    report_assertion_failure(XSTR(b),FILE_INFO, LINE_INFO, msg); \
    breakpoint();                                             \
  }
#else
#define assert(b,msg)
#endif


#define fatal(m)                             { report_fatal(FILE_INFO, LINE_INFO, m                                    ); DEBUG_EXCEPTION; }
#define fatal1(m,x1)                         { report_fatal(FILE_INFO, LINE_INFO, m, x1                                ); DEBUG_EXCEPTION; }
#define fatal2(m,x1,x2)                      { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2                            ); DEBUG_EXCEPTION; }
#define fatal3(m,x1,x2,x3)                   { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3                        ); DEBUG_EXCEPTION; }
#define fatal4(m,x1,x2,x3,x4)                { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3, x4                    ); DEBUG_EXCEPTION; }
#define fatal5(m,x1,x2,x3,x4,x5)             { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3, x4, x5                ); DEBUG_EXCEPTION; }
#define fatal6(m,x1,x2,x3,x4,x5,x6)          { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3, x4, x5, x6            ); DEBUG_EXCEPTION; }
#define fatal7(m,x1,x2,x3,x4,x5,x6,x7)       { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3, x4, x5, x6, x7        ); DEBUG_EXCEPTION; }
#define fatal8(m,x1,x2,x3,x4,x5,x6,x7,x8)    { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3, x4, x5, x6, x7, x8    ); DEBUG_EXCEPTION; }
#define fatal9(m,x1,x2,x3,x4,x5,x6,x7,x8,x9) { report_fatal(FILE_INFO, LINE_INFO, m, x1, x2, x3, x4, x5, x6, x7, x8, x9); DEBUG_EXCEPTION; }


#define guarantee(b,msg)         { if (!(b)) fatal(msg); }
#define ShouldNotCallThis()      { report_should_not_call        (FILE_INFO, LINE_INFO); DEBUG_EXCEPTION; }
#define ShouldNotReachHere()     { report_should_not_reach_here  (FILE_INFO, LINE_INFO); DEBUG_EXCEPTION; }
#define SubclassResponsibility() { report_subclass_responsibility(FILE_INFO, LINE_INFO); DEBUG_EXCEPTION; }
#define Unimplemented()          { report_unimplemented          (FILE_INFO, LINE_INFO); DEBUG_EXCEPTION; }


void report_assertion_failure(const char* code_str, const char* file_name, int line_no, const char* message) CLANG_ANALYZER_NORETURN;
void report_fatal(const char* file_name, int line_no, const char* format, ...) CLANG_ANALYZER_NORETURN;
void report_should_not_call(const char* file_name, int line_no) CLANG_ANALYZER_NORETURN;
void report_should_not_reach_here(const char* file_name, int line_no) CLANG_ANALYZER_NORETURN;
void report_subclass_responsibility(const char* file_name, int line_no) CLANG_ANALYZER_NORETURN;
void report_unimplemented(const char* file_name, int line_no) CLANG_ANALYZER_NORETURN;
void report_vm_state();
