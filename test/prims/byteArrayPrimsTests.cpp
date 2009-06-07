# include "incls/_precompiled.incl"
# include "incls/_byteArray_prims.cpp.incl"
#include "test.h"
#include "handle.hpp"

using namespace easyunit;

extern "C" int expansion_count;
DECLARE(ByteArrayPrimsTests)
  klassOop byteArrayClass;
END_DECLARE

SETUP(ByteArrayPrimsTests) {
  byteArrayClass = klassOop(Universe::find_global("ByteArray"));
}

TEARDOWN(ByteArrayPrimsTests){
  MarkSweep::collect();
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldAllocateByteArrayOfCorrectSize) {
  HandleMark handles;
  Handle byteArrayClassHandle(byteArrayClass);
  oop result = byteArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), byteArrayClass);
  ASSERT_TRUE(result->is_byteArray());
  ASSERT_EQUALS(10, byteArrayOop(result)->length());
  ASSERT_EQUALS((char*)byteArrayClassHandle.as_klass(), (char*)result->klass());
  for (int index = 10; index > 0; index--)
    ASSERT_EQUALS(u_char(0), byteArrayOop(result)->byte_at(index));
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldAllocateTenuredWhenRequested) {
  HandleMark handles;
  Handle byteArrayClassHandle(byteArrayClass);
  int size = Universe::new_gen.eden()->free() + 1;
  oop result = byteArrayPrimitives::allocateSize2(trueObj,
                                                  as_smiOop(size),
                                                  byteArrayClass);
  ASSERT_TRUE(result->is_byteArray());
  ASSERT_TRUE(Universe::old_gen.contains(result));
  ASSERT_EQUALS(size, byteArrayOop(result)->length());
  ASSERT_EQUALS((char*)byteArrayClassHandle.as_klass(), (char*)result->klass());
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldFailWithNonByteArray) {
  oop result = byteArrayPrimitives::allocateSize2(falseObj, as_smiOop(10), Universe::find_global("Object"));
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQUALS((char*)markSymbol(vmSymbols::invalid_klass()), (char*)result);
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldFailWithNonInteger) {
  oop result = byteArrayPrimitives::allocateSize2(falseObj, byteArrayClass, byteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQUALS((char*)markSymbol(vmSymbols::first_argument_has_wrong_type()), (char*)result);
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldFailWithNegativeSize) {
  oop result = byteArrayPrimitives::allocateSize2(falseObj, as_smiOop(-1), byteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQUALS((char*)markSymbol(vmSymbols::negative_size()), (char*)result);
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldFailWhenTenuredNotBoolean) {
  oop result = byteArrayPrimitives::allocateSize2(Universe::nilObj(), as_smiOop(10), byteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQUALS((char*)markSymbol(vmSymbols::second_argument_has_wrong_type()), (char*)result);
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldFailWhenInsufficientSpace) {
  int size = Universe::new_gen.eden()->free();
  oop result = byteArrayPrimitives::allocateSize2(falseObj, as_smiOop(size + 1), byteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQUALS_M(markSymbol(vmSymbols::failed_allocation()), result, unmarkSymbol(result)->as_string());
}

TESTF(ByteArrayPrimsTests, allocateSize2ShouldFailWhenTooBigForOldGen) {
  int size = Universe::old_gen.free();
  oop result = byteArrayPrimitives::allocateSize2(trueObj, as_smiOop(size + 1), byteArrayClass);
  ASSERT_TRUE(result->is_mark());
  ASSERT_EQUALS_M(markSymbol(vmSymbols::failed_allocation()), result, unmarkSymbol(result)->as_string());
}
