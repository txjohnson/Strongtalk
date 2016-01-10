#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_behavior_prims.cpp.incl"


extern "C" int expansion_count;

class OldSpaceTests : public ::testing::Test {
protected:
virtual void TearDown(){
  MarkSweep::collect();
}
};

TEST_F(OldSpaceTests, expandShouldIncreaseExpansionCount) {
  int expansions = expansion_count;
  oldSpace* theSpace = Universe::old_gen.top_mark()._space;
  theSpace->expand(ObjectHeapExpandSize * K);
  ASSERT_EQ(expansions + 1, expansion_count);
}

TEST_F(OldSpaceTests, oldSpaceMarkShouldRestoreTop) {
  oldSpace* theSpace = Universe::old_gen.top_mark()._space;
  int freeSpace = theSpace->free();
  oop* oldTop = theSpace->top();
  {
    OldSpaceMark mark(theSpace);
    ASSERT_TRUE(theSpace->allocate(100, false) != NULL);
    ASSERT_EQ(freeSpace - (100 * oopSize), theSpace->free());
  }
  ASSERT_EQ(freeSpace, theSpace->free());
  ASSERT_EQ((char*)oldTop, (char*)theSpace->top());
}

TEST_F(OldSpaceTests, expandAndAllocateShouldLeaveFreeSpaceUnchanged) {
  HandleMark mark;
  Handle byteArrayClass(Universe::find_global("ByteArray"));

  oldSpace* theSpace = Universe::old_gen.top_mark()._space;
  int freeSpace = theSpace->free();
  {
    OldSpaceMark mark(theSpace);
    oop* result = theSpace->expand_and_allocate(ObjectHeapExpandSize * K);
    ASSERT_EQ(freeSpace, theSpace->free());
  }
}
