/* Copyright 1994, 1995 LongView Technologies L.L.C. $Revision: 1.24 $ */
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

# include "incls/_precompiled.incl"
# include "incls/_lprintf.cpp.incl"
#include <string.h>
//#include <io.h> // for GCC's _unlink

static FILE* logFile = NULL;
static char fname[80];

// dont't use include files for the thing below because this would include
// the conflicting defs of lprintf et al.
extern "C" bool PrintVMMessages;
extern "C" bool LogVMMessages;
extern "C" bool AlwaysFlushVMMessages;
extern "C" {
  void breakpoint();
  void error_breakpoint();
}

void lprintf_exit() {
  if (logFile) {
    fclose(logFile);
    logFile = NULL;
    remove(fname);
  }
}

static void check_log_file() {
  if (LogVMMessages && !logFile) {
    os::move_file(SYSTEM_NAME ".log", SYSTEM_NAME ".log.old");
    logFile = fopen(SYSTEM_NAME ".log", "w");
  }
}


extern "C" void lprintf(const char* m, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, m);
  vsprintf(buf, m, ap);
  va_end(ap);

  check_log_file();
  if (LogVMMessages) {
    fputs(buf, logFile);
    if (AlwaysFlushVMMessages) fflush(logFile);
  }
  if (PrintVMMessages) {
    fputs(buf, stdout);
    if (AlwaysFlushVMMessages) fflush(stdout);
  }
}

extern "C" void lputc(char c) {
  check_log_file();
  if (LogVMMessages) {
    fputc(c, logFile);
    if (AlwaysFlushVMMessages) fflush(logFile);
  }
  if (PrintVMMessages) {
    fputc(c, stdout);
    if (AlwaysFlushVMMessages) fflush(stdout);
  }
}

extern "C" void lputs(const char* str) {
  check_log_file();
  if (LogVMMessages) {
    fputs(str, logFile);
    if (AlwaysFlushVMMessages) fflush(logFile);
  }
  if (PrintVMMessages) {
    fputs(str, stdout);
    if (AlwaysFlushVMMessages) fflush(stdout);
  }
}

extern "C" void error(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  Notifier::current->error(format, ap);
  va_end(ap);
}

extern "C" void warning(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  Notifier::current->warning(format, ap);
  va_end(ap);
}

extern "C" void compiler_warning(const char* format, ...) {
  if (PrintCompilerWarnings) {
    va_list ap;
    va_start(ap, format);
    Notifier::current->compiler_warning(format, ap);
    va_end(ap);
  }
}

void flush_logFile() { if (logFile) fflush(logFile); }

extern "C" void my_sprintf(char*& buf, const char* format, ...){
  // like sprintf, but updates the buf pointer so that subsequent
  // sprintfs append to the string
  va_list ap;
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);
  buf += strlen(buf);
}

extern "C" void my_sprintf_len(char*& buf, const int len, const char* format, ...){
  char* oldbuf = buf;
  va_list ap;
  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);
  buf += strlen(buf);
  for ( ; buf < oldbuf + len; *buf++ = ' ') ;
  *buf = '\0';
}


class DebugNotifier: public Notifier, public CHeapObj {
public:
  DebugNotifier() {}
  void error(const char* m, va_list ap);
  void warning(const char* m, va_list ap);
  void compiler_warning(const char* m, va_list ap);
};

Notifier* Notifier::current = new DebugNotifier();

void DebugNotifier::error(const char* m, va_list ap) {
  std->print_cr("VM Error:");
  std->vprint_cr(m, ap);
  DEBUG_EXCEPTION;
}

void DebugNotifier::warning(const char* m, va_list ap) {
  std->print_cr("VM Warning:");
  std->vprint_cr(m, ap);
  if (BreakAtWarning) DEBUG_EXCEPTION;
}

void DebugNotifier::compiler_warning(const char* m, va_list ap) {
  std->print_cr("Compiler Warning:");
  std->vprint_cr(m, ap);
  if (BreakAtWarning) DEBUG_EXCEPTION;}