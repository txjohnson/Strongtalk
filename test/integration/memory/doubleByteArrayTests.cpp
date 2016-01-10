#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_dByteArrayKlass.cpp.incl"


extern "C" oop* eden_top;
extern "C" oop* eden_end;

class DByteArrayKlassTests : public ::testing::Test {
protected:
  klassOop theClass;
  oop* oldEdenTop;

virtual void SetUp() {
  theClass = klassOop(Universe::find_global("String"));
  oldEdenTop = eden_top;
}

virtual void TearDown(){
  eden_top = oldEdenTop;
  MarkSweep::collect();
}
};

TEST_F(DByteArrayKlassTests, shouldBeDoubleByteArray) {
  eden_top = eden_end;
  ASSERT_TRUE(theClass->klass_part()->oop_is_doubleByteArray());
}
TEST_F(DByteArrayKlassTests, allocateShouldFailWhenAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_EQ((int)NULL, (int)(theClass->klass_part()->allocateObjectSize(100, false)));
}

TEST_F(DByteArrayKlassTests, allocateShouldAllocateTenuredWhenRequired) {
  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObjectSize(100, false, true)));
}

TEST_F(DByteArrayKlassTests, allocateShouldNotFailWhenNotAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
  ASSERT_TRUE(Universe::new_gen.contains(theClass->klass_part()->allocateObjectSize(100, true)));
}
