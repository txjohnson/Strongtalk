#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_dByteArray_prims.cpp.incl"


extern "C" int expansion_count;
class DByteArrayPrimsTests : public ::testing::Test {
protected: 
  klassOop dByteArrayClass;

virtual void SetUp() {
  dByteArrayClass = klassOop(Universe::find_global("String"));
}

virtual void TearDown(){
  MarkSweep::collect();
}
};

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldAllocateDByteArrayOfCorrectSize) {
  HandleMark handles;
  Handle dByteArrayClassHandle(dByteArrayClass);
  oop result = doubleByteArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), dByteArrayClass);
  ASSERT_TRUE(result->is_doubleByteArray());
  ASSERT_TRUE(!Universe::old_gen.contains(result));
  ASSERT_TRUE(Universe::new_gen.contains(result));
  ASSERT_EQ(10, doubleByteArrayOop(result)->length());
  ASSERT_EQ((char*)dByteArrayClassHandle.as_klass(), (char*)result->klass());
  for (int index = 10; index > 0; index--)
    ASSERT_EQ(doubleByte(0), doubleByteArrayOop(result)->doubleByte_at(index));
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldAllocateTenuredWhenRequested) {
  HandleMark handles;
  Handle classHandle(dByteArrayClass);
  oop result = doubleByteArrayPrimitives::allocateSize2(trueObj,
                                                  as_smiOop(10),
                                                  dByteArrayClass);
  ASSERT_TRUE(result->is_doubleByteArray());
  ASSERT_TRUE(Universe::old_gen.contains(result));
  ASSERT_EQ(10, doubleByteArrayOop(result)->length());
  ASSERT_EQ((char*)classHandle.as_klass(), (char*)result->klass());
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWithNonDoubleByteArray) {
  oop result = doubleByteArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), Universe::find_global("Object"));
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((int)markSymbol(vmSymbols::invalid_klass()), (int)result);
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWithNonKlass) {
  oop result = doubleByteArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), Universe::trueObj());
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((int)markSymbol(vmSymbols::invalid_klass()), (int)result);
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWithNonInteger) {
  oop result = doubleByteArrayPrimitives::allocateSize2(falseObj, dByteArrayClass, dByteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::first_argument_has_wrong_type()), (char*)result);
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWithNegativeSize) {
  oop result = doubleByteArrayPrimitives::allocateSize2(falseObj, as_smiOop(-1), dByteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::negative_size()), (char*)result);
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWhenTenuredNotBoolean) {
  oop result = doubleByteArrayPrimitives::allocateSize2(Universe::nilObj(), as_smiOop(10), dByteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ((char*)markSymbol(vmSymbols::second_argument_has_wrong_type()), (char*)result);
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWhenInsufficientSpace) {
  int size = Universe::new_gen.eden()->free();
  oop result = doubleByteArrayPrimitives::allocateSize2(falseObj, as_smiOop(size + 1), dByteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ(markSymbol(vmSymbols::failed_allocation()), result) << unmarkSymbol(result)->as_string();
}

TEST_F(DByteArrayPrimsTests, allocateSize2ShouldFailWhenTooBigForOldGen) {
  int size = Universe::old_gen.free();
  oop result = doubleByteArrayPrimitives::allocateSize2(trueObj, as_smiOop(size + 1), dByteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQ(markSymbol(vmSymbols::failed_allocation()), result) << unmarkSymbol(result)->as_string();
}
