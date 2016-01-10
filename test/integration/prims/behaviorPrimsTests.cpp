#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_behavior_prims.cpp.incl"
#include "edenMark.hpp"


class BehaviorPrimitives : public ::testing::Test {
protected:
  EdenMark edenMark;
  klassOop objectClass;

virtual void SetUp() {
  edenMark.setToEnd();
  objectClass = klassOop(Universe::find_global("Object"));
}

virtual void TearDown(){
}
};

TEST_F(BehaviorPrimitives, allocateForMemOopShouldReportFailureWhenNoSpace) {
  ASSERT_TRUE(Universe::new_gen.eden()->free() < (2 * oopSize)) << "Too much free space";
  ASSERT_EQ(markSymbol(vmSymbols::failed_allocation()), behaviorPrimitives::allocate3(Universe::falseObj(), objectClass)) << "Allocation should fail";
}

TEST_F(BehaviorPrimitives, allocateForMemOopShouldAllocateTenureWhenRequired) {
  ASSERT_TRUE(behaviorPrimitives::allocate3(Universe::trueObj(), objectClass)->is_old());
}

TEST_F(BehaviorPrimitives, allocateForMemOopShouldCheckTenuredIsBoolean) {
  ASSERT_TRUE(markSymbol(vmSymbols::second_argument_has_wrong_type()) == behaviorPrimitives::allocate3(Universe::nilObj(), objectClass));
}

TEST_F(BehaviorPrimitives, allocateForMemOopShouldScavengeAndAllocateWhenAllowed) {
  HandleMark mark;
  Handle objectClassHandle(objectClass);
  ASSERT_TRUE(Universe::new_gen.eden()->free() < (2 * oopSize)) << "Too much free space";
  oop object = behaviorPrimitives::allocate(objectClass);
  ASSERT_TRUE(!object->is_mark()) << "result should not be marked";
  ASSERT_TRUE(object->is_mem()) << "result should be mem";
  ASSERT_EQ(memOop(object)->klass(), objectClassHandle.as_memOop()) << "wrong class";
}