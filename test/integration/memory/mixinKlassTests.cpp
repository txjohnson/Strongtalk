#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_mixinKlass.cpp.incl"


extern "C" oop* eden_top;
extern "C" oop* eden_end;

class MixinKlassTests : public ::testing::Test {
protected:
  klassOop theClass;
  oop* oldEdenTop;

virtual void SetUp() {
  theClass = klassOop(Universe::find_global("Object"))->klass_part()->mixin()->klass();
  oldEdenTop = eden_top;
}

virtual void TearDown(){
  eden_top = oldEdenTop;
  MarkSweep::collect();
}
};

TEST_F(MixinKlassTests, allocateShouldFailWhenAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_EQ((int)NULL, (int)(theClass->klass_part()->allocateObject(false)));
}

TEST_F(MixinKlassTests, allocateShouldAllocateTenuredWhenRequired) {
  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObject(false, true)));
}

TEST_F(MixinKlassTests, allocateShouldNotFailWhenNotAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
  ASSERT_TRUE(Universe::new_gen.contains(theClass->klass_part()->allocateObject(true)));
}
