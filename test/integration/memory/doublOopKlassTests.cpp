#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_doubleKlass.cpp.incl"


extern "C" oop* eden_top;
extern "C" oop* eden_end;

class DoubleKlassTests : public ::testing::Test {
protected:
  klassOop theClass;
  oop* oldEdenTop;

virtual void SetUp() {
  theClass = klassOop(Universe::find_global("Float"));
  oldEdenTop = eden_top;
}

virtual void TearDown(){
  eden_top = oldEdenTop;
  MarkSweep::collect();
}
};

TEST_F(DoubleKlassTests, floatShouldBeDoubleC) {
  ASSERT_TRUE(theClass->klass_part()->oop_is_double());
}

TEST_F(DoubleKlassTests, allocateShouldFailWhenAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
  ASSERT_EQ((int)NULL, (int)(theClass->klass_part()->allocateObject(false)));
}

TEST_F(DoubleKlassTests, allocateShouldAllocateTenuredWhenRequired) {
  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObject(false, true)));
}

TEST_F(DoubleKlassTests, allocateShouldNotFailWhenNotAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
  ASSERT_TRUE(Universe::new_gen.contains(theClass->klass_part()->allocateObject(true)));
}
