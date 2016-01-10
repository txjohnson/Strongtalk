#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_smi_prims.cpp.incl"


typedef oop (__stdcall * smifntype)(smiOop, smiOop);

extern "C" int expansion_count;
class SmiPrimsTests : public ::testing::Test {
protected:
  smifntype smiQuo;
  symbolOop quoSymbol;

virtual void SetUp() {
  quoSymbol = oopFactory::new_symbol("primitiveQuo:ifFail:");
  primitive_desc* prim = primitives::lookup(quoSymbol);
  smiQuo = smifntype(prim->fn());
}

virtual void TearDown(){
}
};

TEST_F(SmiPrimsTests, quoShouldReturnDivideReceiverByArgument) {
  ASSERT_EQ(5, smiOop(smiQuo(as_smiOop(2), as_smiOop(10)))->value());
}

TEST_F(SmiPrimsTests, quoShouldReturnReceiverHasWrongTypeWhenNotSMI) {
  oop result = smiQuo(as_smiOop(2), smiOop(quoSymbol));
  ASSERT_EQ((int)markSymbol(vmSymbols::receiver_has_wrong_type()), (int)result);
}
