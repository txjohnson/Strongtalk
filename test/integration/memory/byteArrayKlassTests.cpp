#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_byteArrayKlass.cpp.incl"


extern "C" oop* eden_top;
extern "C" oop* eden_end;

class ByteArrayKlassTests : public ::testing::Test {
protected:
  klassOop theClass;
  oop* oldEdenTop;

virtual void SetUp() {
  theClass = klassOop(Universe::find_global("ByteArray"));
  oldEdenTop = eden_top;
}

virtual void TearDown(){
  eden_top = oldEdenTop;
  MarkSweep::collect();
}
};

TEST_F(ByteArrayKlassTests, shouldBeDoubleByteArray) {
  eden_top = eden_end;
  ASSERT_TRUE(theClass->klass_part()->oop_is_byteArray());
}
TEST_F(ByteArrayKlassTests, allocateShouldFailWhenAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_EQ((int)NULL, (int)(theClass->klass_part()->allocateObjectSize(100, false)));
}

TEST_F(ByteArrayKlassTests, allocateShouldAllocateTenuredWhenRequired) {
  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObjectSize(100, false, true)));
}

TEST_F(ByteArrayKlassTests, allocateShouldNotFailWhenNotAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
  ASSERT_TRUE(Universe::new_gen.contains(theClass->klass_part()->allocateObjectSize(100, true)));
}
