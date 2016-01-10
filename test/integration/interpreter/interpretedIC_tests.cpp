#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_oop_prims.cpp.incl"
#include "rSet.inline.hpp"


class InterpretedICTest : public ::testing::Test {
protected:
oop fixture;

virtual void SetUp() {
  klassOop objectClass = klassOop(Universe::find_global("DoesNotUnderstandFixture"));
  fixture = objectClass->klass_part()->allocateObject();
}

virtual void TearDown(){
  fixture = NULL;
  MarkSweep::collect();
}
};

TEST_F(InterpretedICTest, noArgSendWithUnknownSelectorShouldInvokeDoesNotUnderstand)
{
  BlockScavenge bs;
  symbolOop selector = oopFactory::new_symbol("dnuTrigger1", 11);
  symbolOop returnedSelector = oopFactory::new_symbol("quack", 5);
  oop result = Delta::call(fixture, selector);

  klassOop expectedKlass = klassOop(Universe::find_global("Message"));

  ASSERT_TRUE(result->is_mem()) << "result should be object";
  ASSERT_EQ(expectedKlass, result->klass()) << "wrong class returned";
  ASSERT_EQ(fixture, memOop(result)->raw_at(2)) << "message should contain receiver";
  ASSERT_EQ(returnedSelector, memOop(result)->raw_at(3)) << "message should contain correct selector";
}
