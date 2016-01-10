#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_generation.cpp.incl"

class OldGeneration : public ::testing::Test {
protected:
  void expandAndCheckCapacity(int expansionSize) {
    char msg[100];
    int oldSize = Universe::old_gen.capacity();

    Universe::old_gen.expand(expansionSize);

    int expectedIncrement = ReservedSpace::align_size(expansionSize, ObjectHeapExpandSize * K);
    int actual = Universe::old_gen.capacity();
    int expectedSize = oldSize + expectedIncrement;
    sprintf(msg, "Generation has wrong capacity. Expected: %d, but was: %d", expectedSize, actual);
    ASSERT_EQ(expectedSize, actual) << msg;
  }
};

TEST_F(OldGeneration, expansionShouldExpandOldGenerationCapacity) {
  expandAndCheckCapacity(1000 * K);
}

TEST_F(OldGeneration, expansionShouldExpandOldGenerationCapacityByMinimumAmount) {
  expandAndCheckCapacity(10);
}

TEST_F(OldGeneration, expansionShouldAlignExpansionWithExpansionSize) {
  expandAndCheckCapacity(ObjectHeapExpandSize * K * 3 / 2);
}

TEST_F(OldGeneration, allocateWithoutExpansionWhenEmptyShouldFail) {
  int free = Universe::old_gen.free();
  oop* result = Universe::old_gen.allocate(free + 1, false);
  ASSERT_EQ(NULL, result) << "Result should be NULL";
}

TEST_F(OldGeneration, shrinkShouldReduceOldSpaceCapacity) {
  int freeSpace = Universe::old_gen.free();
  Universe::old_gen.expand(ObjectHeapExpandSize *K);
  Universe::old_gen.shrink(ObjectHeapExpandSize *K);
  ASSERT_EQ(freeSpace, Universe::old_gen.free());
}

TEST_F(OldGeneration, shrinkShouldReturnZeroWhenInsufficientFreeSpace) {
  int freeSpace = Universe::old_gen.free();
  ASSERT_EQ(0, Universe::old_gen.shrink(freeSpace + 1));
  ASSERT_EQ(freeSpace, Universe::old_gen.free());
}