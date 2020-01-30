#pragma once
/* Copyright 1994, 1995 LongView Technologies L.L.C. $Revision: 1.8 $ */
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

class mixinKlass: public memOopKlass {
 public:
  // testers
  bool oop_is_mixin()          const { return true;  }

  // allocation properties
  bool can_inline_allocation() const { return false; }

  // allocation operations
  oop allocateObject(bool permit_scavenge = true, bool tenured = false);
  oop oop_shallow_copy(oop obj, bool tenured);
  
  // creates invocation
  klassOop create_subclass(mixinOop mixin, Format format);

  static klassOop create_class(klassOop super_class, mixinOop mixin);

  // Format
  Format format() { return mixin_klass; }

  // memory operations
  int  oop_scavenge_contents(oop obj);
  int  oop_scavenge_tenured_contents(oop obj);
  void oop_follow_contents(oop obj);

  // iterators
  void oop_layout_iterate(oop obj, ObjectLayoutClosure* blk);
  void oop_oop_iterate(oop obj, OopClosure* blk);

  // sizing
  int oop_header_size() const { return mixinOopDesc::header_size(); }

  // printing support
  char* name() const { return "mixin"; }

  // class creation
  friend void set_mixinKlass_vtbl(Klass* k);
};
void set_mixinKlass_vtbl(Klass* k);