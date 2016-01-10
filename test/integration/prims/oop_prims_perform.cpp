#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_oop_prims.cpp.incl"
#include "rSet.inline.hpp"


class OopPrimitivesPerformTest : public ::testing::Test {
protected:
  oop fixture;

virtual void SetUP() {
  klassOop objectClass = klassOop(Universe::find_global("DoesNotUnderstandFixture"));

  fixture = objectClass->klass_part()->allocateObject();
}

virtual void TearDown(){
  fixture = NULL;
  MarkSweep::collect();
}
};

TEST_F(OopPrimitivesPerformTest, noArgPerformWithUnknownShouldInvokeDoesNotUnderstand)
{
	symbolOop selector = oopFactory::new_symbol("unknown", 7);
	oop result = oopPrimitives::perform(selector, fixture);
  klassOop expectedKlass = klassOop(Universe::find_global("Message"));
  ASSERT_TRUE(result->is_mem()) << "result should be object";
  ASSERT_EQ(expectedKlass, result->klass()) << "wrong class returned";
  ASSERT_EQ(fixture, memOop(result)->raw_at(2)) << "message should contain receiver";
  ASSERT_EQ(selector, memOop(result)->raw_at(3)) << "message should contain selector";
}

TEST_F(OopPrimitivesPerformTest, oneArgPerformWithUnknownShouldInvokeDoesNotUnderstand)
{
  symbolOop selector = oopFactory::new_symbol("unknown:", 8);
  symbolOop arg1 = oopFactory::new_symbol("arg1", 4);
  oop result = oopPrimitives::performWith(arg1, selector, fixture);
  klassOop expectedKlass = klassOop(Universe::find_global("Message"));
  ASSERT_TRUE(result->is_mem()) << "result should be object";
  ASSERT_EQ(expectedKlass, result->klass()) << "wrong class returned";
  ASSERT_EQ(fixture, memOop(result)->raw_at(2)) << "message should contain receiver";
  ASSERT_EQ(selector, memOop(result)->raw_at(3)) << "message should contain selector";
  oop args = memOop(result)->raw_at(4);
  ASSERT_TRUE(args->is_objArray()) << "args should be object array";
  ASSERT_EQ(arg1, objArrayOop(args)->obj_at(1)) << "message should contain argument 1";
}

TEST_F(OopPrimitivesPerformTest, twoArgPerformWithUnknownShouldInvokeDoesNotUnderstand)
{
  symbolOop selector = oopFactory::new_symbol("unknown:with:", 13);
  symbolOop arg1 = oopFactory::new_symbol("arg1", 4);
  symbolOop arg2 = oopFactory::new_symbol("arg2", 4);

  oop result = oopPrimitives::performWithWith(arg2, arg1, selector, fixture);

  klassOop expectedKlass = klassOop(Universe::find_global("Message"));

  ASSERT_TRUE(result->is_mem()) << "result should be object";
  ASSERT_EQ(expectedKlass, result->klass()) << "wrong class returned";
  ASSERT_EQ(fixture, memOop(result)->raw_at(2)) << "message should contain receiver";
  ASSERT_EQ(selector, memOop(result)->raw_at(3)) << "message should contain selector";
  oop args = memOop(result)->raw_at(4);
  ASSERT_TRUE(args->is_objArray()) << "args should be object array";
  ASSERT_EQ(2, objArrayOop(args)->length()) << "wrong number of arguments";
  ASSERT_EQ(arg1, objArrayOop(args)->obj_at(1)) << "message should contain argument 1";
  ASSERT_EQ(arg2, objArrayOop(args)->obj_at(2)) << "message should contain argument 2";
}

TEST_F(OopPrimitivesPerformTest, threeArgPerformWithUnknownShouldInvokeDoesNotUnderstand)
{
  symbolOop selector = oopFactory::new_symbol("unknown:with:with:", 18);
  symbolOop arg1 = oopFactory::new_symbol("arg1", 4);
  symbolOop arg2 = oopFactory::new_symbol("arg2", 4);
  symbolOop arg3 = oopFactory::new_symbol("arg3", 4);

  oop result = oopPrimitives::performWithWithWith(arg3, arg2, arg1, selector, fixture);

  klassOop expectedKlass = klassOop(Universe::find_global("Message"));

  ASSERT_TRUE(result->is_mem()) << "result should be object";
  ASSERT_EQ(expectedKlass, result->klass()) << "wrong class returned";
  ASSERT_EQ(fixture, memOop(result)->raw_at(2)) << "message should contain receiver";
  ASSERT_EQ(selector, memOop(result)->raw_at(3)) << "message should contain selector";
  oop args = memOop(result)->raw_at(4);
  ASSERT_TRUE(args->is_objArray()) << "args should be object array";
  ASSERT_EQ(3, objArrayOop(args)->length()) << "wrong number of arguments";
  ASSERT_EQ(arg1, objArrayOop(args)->obj_at(1)) << "message should contain argument 1";
  ASSERT_EQ(arg2, objArrayOop(args)->obj_at(2)) << "message should contain argument 2";
  ASSERT_EQ(arg3, objArrayOop(args)->obj_at(3)) << "message should contain argument 3";
}

TEST_F(OopPrimitivesPerformTest, varArgPerformWithUnknownShouldInvokeDoesNotUnderstand)
{
  symbolOop selector = oopFactory::new_symbol("unknown:with:with:with:", 23);
  symbolOop arg1 = oopFactory::new_symbol("arg1", 4);
  symbolOop arg2 = oopFactory::new_symbol("arg2", 4);
  symbolOop arg3 = oopFactory::new_symbol("arg3", 4);
  symbolOop arg4 = oopFactory::new_symbol("arg4", 4);

  objArrayOop inputArgs = oopFactory::new_objArray(4);
  inputArgs->obj_at_put(1, arg1);
  inputArgs->obj_at_put(2, arg2);
  inputArgs->obj_at_put(3, arg3);
  inputArgs->obj_at_put(4, arg4);
  oop result = oopPrimitives::performArguments(inputArgs, selector, fixture);

  klassOop expectedKlass = klassOop(Universe::find_global("Message"));

  ASSERT_TRUE(result->is_mem()) << "result should be object";
  ASSERT_EQ(expectedKlass, result->klass()) << "wrong class returned";
  ASSERT_EQ(fixture, memOop(result)->raw_at(2)) << "message should contain receiver";
  ASSERT_EQ(selector, memOop(result)->raw_at(3)) << "message should contain selector";
  oop args = memOop(result)->raw_at(4);
  ASSERT_TRUE(args->is_objArray()) << "args should be object array";
  ASSERT_EQ(4, objArrayOop(args)->length()) << "wrong number of arguments";
  ASSERT_EQ(arg1, objArrayOop(args)->obj_at(1)) << "message should contain argument 1";
  ASSERT_EQ(arg2, objArrayOop(args)->obj_at(2)) << "message should contain argument 2";
  ASSERT_EQ(arg3, objArrayOop(args)->obj_at(3)) << "message should contain argument 3";
  ASSERT_EQ(arg4, objArrayOop(args)->obj_at(4)) << "message should contain argument 4";
}
