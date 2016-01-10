#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_objArray_prims.cpp.incl"


extern "C" int expansion_count;
class ObjectArrayPrimsTests : public ::testing::Test {
protected:
  klassOop arrayClass;

virtual void SetUp() {
  arrayClass = klassOop(Universe::find_global("Array"));
}

virtual void TearDown(){
  MarkSweep::collect();
}
};

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldAllocateArrayOfCorrectSize) {
  HandleMark handles;
  Handle arrayClassHandle(arrayClass);

  oop result = objArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), arrayClass);
  ASSERT_TRUE(result->is_objArray());
  ASSERT_EQ(10, objArrayOop(result)->length());
  ASSERT_EQ((char*)arrayClassHandle.as_klass(), (char*)result->klass());
  for (int index = 10; index > 0; index--)
    ASSERT_TRUE(Universe::nilObj() == (objArrayOop(result)->obj_at(index)));
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldAllocateTenuredArrayOfCorrectSize) {
  HandleMark handles;
  Handle arrayClassHandle(arrayClass);

  oop result = objArrayPrimitives::allocateSize2(trueObj, as_smiOop(10), arrayClass);
  ASSERT_TRUE(result->is_objArray());
  ASSERT_EQ(10, objArrayOop(result)->length());
  ASSERT_TRUE(result->is_old());
  ASSERT_EQ((char*)arrayClassHandle.as_klass(), (char*)result->klass());
  for (int index = 10; index > 0; index--)
    ASSERT_TRUE(Universe::nilObj() == (objArrayOop(result)->obj_at(index)));
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldFailWithNonObjectArray) {
  oop result = objArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), Universe::find_global("Object"));
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::invalid_klass()), (char*)result);
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldFailWithNonInteger) {
  oop result = objArrayPrimitives::allocateSize2(falseObj, arrayClass, arrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::first_argument_has_wrong_type()), (char*)result);
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldFailWithNegativeSize) {
  oop result = objArrayPrimitives::allocateSize2(falseObj, as_smiOop(-1), arrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::negative_size()), (char*)result);
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldFailWhenTenuredNotBoolean) {
  oop result = objArrayPrimitives::allocateSize2(Universe::nilObj(), as_smiOop(10), arrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::second_argument_has_wrong_type()), (char*)result);
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldFailWhenInsufficientSpace) {
  int size = Universe::new_gen.eden()->free() / oopSize;
  oop result = objArrayPrimitives::allocateSize2(falseObj, as_smiOop(size + 1), arrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ(markSymbol(vmSymbols::failed_allocation()), result) << unmarkSymbol(result)->as_string();
}

TEST_F(ObjectArrayPrimsTests, allocateSize2ShouldFailWhenTooBigForOldGen) {
  int size = Universe::old_gen.free() / oopSize;
  oop result = objArrayPrimitives::allocateSize2(trueObj, as_smiOop(size + 1), arrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ(markSymbol(vmSymbols::failed_allocation()), result) << unmarkSymbol(result)->as_string();
}
