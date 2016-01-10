#include "gtest/gtest.h"

# include "incls/_precompiled.incl"

class UniverseTests : public ::testing::Test {
protected:
oop* eden_old_top;

virtual void SetUp() {
  eden_old_top = eden_top;
}

virtual void TearDown(){
  //Can't collect because the allocated space is not an object.
  eden_top = eden_old_top;
}
};

TEST_F(UniverseTests, allocateShouldAllocateInNewSpaceWhenSpaceAvailable) {
  ASSERT_TRUE(Universe::new_gen.eden()->free() > 10);
  oop* chunk = Universe::allocate(10, NULL, false);
  ASSERT_TRUE(chunk != NULL);
  ASSERT_TRUE(Universe::new_gen.eden()->contains(chunk));
}

TEST_F(UniverseTests, allocateShouldFailWhenNoSpaceAndScavengeDisallowed) {
  int freeSpace = Universe::new_gen.eden()->free();
  oop* chunk = Universe::allocate(freeSpace/oopSize + 1, NULL, false);
  ASSERT_EQ(NULL, (char*)chunk);
}

TEST_F(UniverseTests, allocateTenuredShouldFailWhenNoSpaceAndExpansionDisallowed) {
  int freeSpace = Universe::old_gen.free();
  oop* chunk = Universe::allocate_tenured(freeSpace/oopSize + 1, false);
  ASSERT_EQ(NULL, (char*)chunk);
}