/* Copyright 1994, 1995 LongView Technologies L.L.C. $Revision: 1.23 $ */
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

// super class for all heap objects

class memOopKlass: public Klass {
 protected:
  oop* basicAllocate(int size, klassOop* klass, bool permit_scavenge, bool tenured) {
    return tenured ?
      Universe::allocate_tenured(size, permit_scavenge) :
      Universe::allocate(size, (memOop*) klass, permit_scavenge);
  }

 public:
  // allocation properties
  bool can_inline_allocation()       const { return true; }

  // reflective properties
  bool can_have_instance_variables() const { return true; }
  bool can_be_subclassed()           const { return true; }

  // allocation operations
  oop allocateObject(bool permit_scavenge = true, bool tenured = false);
  oop allocateObjectSize(int size, bool permit_scavenge = true, bool tenured = false);

  // invocation creation
  klassOop create_subclass(mixinOop mixin, Format format);
  klassOop create_subclass(mixinOop mixin, klassOop instSuper, klassOop metaClass, Format format);

  static klassOop create_class(klassOop super_class, mixinOop mixin);

  // Format
  Format format() { return mem_klass; }

  // copy operation
  oop oop_shallow_copy(oop obj, bool tenured);

  // printing support
  const char* name() const { return "memOop"; }

  // class creation
  friend void set_memOopKlass_vtbl(Klass* k);

  // ALL FUNCTIONS BELOW THIS POINT ARE DISPATCHED FROM AN OOP
 public:
  // memory operations
  int  oop_scavenge_contents(oop obj);
  int  oop_scavenge_tenured_contents(oop obj);
  void oop_follow_contents(oop obj);

  // printing operations
  void oop_print_on      (oop obj, outputStream* st);
  void oop_print_value_on(oop obj, outputStream* st);

  // iterators
  void oop_oop_iterate(oop obj, OopClosure* blk);
  void oop_layout_iterate(oop obj, ObjectLayoutClosure* blk);

  // verification
  bool oop_verify(oop obj);

  // sizing
  int oop_header_size() const { return memOopDesc::header_size(); }
};
void set_memOopKlass_vtbl(Klass* k);
