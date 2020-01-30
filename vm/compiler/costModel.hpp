#pragma once
/* Copyright 1994 - 1996 LongView Technologies L.L.C. $Revision: 1.1 $ */
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

# ifdef DELTA_COMPILER

// CostModel holds the 'costs' for all bytecodes. These costs
// are used to estimate the inline cost for a given methodOop.

class CostModel: AllStatic {
 private:
  static int _cost[Bytecodes::number_of_codes];

 public:
  static int  cost_for(Bytecodes::Code code)				{ return _cost[(int)code]; }

  static void set_default_costs();					// sets default costs
  static void set_cost_for_all(int cost);				// sets cost for all instructions
  static void set_cost_for_code(Bytecodes::Code     code, int cost);	// sets cost for individual instruction
  static void set_cost_for_type(Bytecodes::CodeType type, int cost);	// sets cost for all instructions of a certain type
  static void set_cost_for_send(Bytecodes::SendType send, int cost);	// sets cost for all sends of a certain type

  static void print();
};

# endif
