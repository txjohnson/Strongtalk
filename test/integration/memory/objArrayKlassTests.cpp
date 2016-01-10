#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_objArrayKlass.cpp.incl"


extern "C" oop* eden_top;
extern "C" oop* eden_end;

class ObjArrayKlassTests : public ::testing::Test {
protected:
  klassOop theClass;
  oop* oldEdenTop;

virtual void SetUp() {
  theClass = klassOop(Universe::find_global("Array"));
  oldEdenTop = eden_top;
}

virtual void TearDown() {
  eden_top = oldEdenTop;
  MarkSweep::collect();
}
};

TEST_F(ObjArrayKlassTests, shouldBeObjArray) {
  eden_top = eden_end;
  ASSERT_TRUE(theClass->klass_part()->oop_is_objArray());
}
TEST_F(ObjArrayKlassTests, allocateShouldFailWhenAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_EQ((int)NULL, (int)(theClass->klass_part()->allocateObjectSize(100, false)));
}

TEST_F(ObjArrayKlassTests, allocateShouldAllocateTenuredWhenRequired) {
  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObjectSize(100, false, true)));
}

TEST_F(ObjArrayKlassTests, allocateShouldNotFailWhenNotAllowedAndNoSpace) {
  eden_top = eden_end;
  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
  ASSERT_TRUE(Universe::new_gen.contains(theClass->klass_part()->allocateObjectSize(100, true)));
}

TEST_F(ObjArrayKlassTests, allocateShouldExpandOldSpaceDuringTenuredAllocWhenAllowed) {
  OldSpaceMark mark = Universe::old_gen.memo();
  oldSpace* space = mark.theSpace;
  int free = Universe::old_gen.free()/oopSize;
  oop* temp = Universe::allocate_tenured(free - 1, false);
  ASSERT_TRUE(temp != NULL);
  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObjectSize(100, true, true)));
}

TEST_F(ObjArrayKlassTests, allocateShouldFailDuringTenuredAllocWhenOldSpaceExpansionNotAllowed) {
  OldSpaceMark mark = Universe::old_gen.memo();
  oldSpace* space = mark.theSpace;
  int free = Universe::old_gen.free()/oopSize;
  oop* temp = Universe::allocate_tenured(free - 1, false);
  ASSERT_TRUE(temp != NULL);
  ASSERT_TRUE(NULL == theClass->klass_part()->allocateObjectSize(100, false, true));
}
