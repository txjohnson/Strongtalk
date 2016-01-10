#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_byteArray_prims.cpp.incl"


extern "C" int expansion_count;
class IndirectAlienPrimsTests : public ::testing::Test {
protected:
  HeapResourceMark * rm;
  klassOop byteArrayClass;
  byteArrayOop alien, invalidAlien;
  byteArrayOop largeUnsignedInteger;
  byteArrayOop largeSignedInteger;
  u_char alien_byte_region[16];
  doubleOop    doubleValue;

  int asInteger(oop largeInteger, bool& ok) {
    Integer* number = &byteArrayOop(largeInteger)->number();
    return number->as_int(ok);
  }
  void checkLargeInteger(oop result, int expected) {
    char message[200];
    ASSERT_TRUE(result->is_byteArray()) << "Should be integer";
    bool ok;
    int actual = asInteger(result, ok);
    ASSERT_TRUE(ok) << "should be integer";
    sprintf(message, "wrong value. expected: %d, was: %d", expected, actual);
    ASSERT_EQ(expected, actual) << message;
  }
  void checkLargeUnsigned(oop result, unsigned int expected) {
    char message[200];
    ASSERT_TRUE(result->is_byteArray()) << "Should be integer";
    bool ok;
    unsigned int actual = byteArrayOop(result)->number().as_unsigned_int(ok);
    ASSERT_TRUE(ok) << "should be integer";
    sprintf(message, "wrong value. expected: %d, was: %d", expected, actual);
    ASSERT_EQ(expected, actual) << message;
  }
  void checkSmallInteger(oop result, int expected) {
    char message[200];
    ASSERT_TRUE(result->is_smi()) << "Should be small integer";
    int actual = smiOop(result)->value();
    sprintf(message, "wrong value. expected: %d, was: %d", expected, actual);
    ASSERT_EQ(expected, actual) << message;
  }
  void checkMarkedSymbol(char* message, oop result, symbolOop expected) {
    char text[200];
    ASSERT_TRUE(result->is_mark()) << "Should be marked";
    sprintf(text,"Should be: %s, was: %s", message, unmarkSymbol(result)->as_string());
    ASSERT_TRUE(unmarkSymbol(result) == expected) << text;
  }

virtual void SetUp() {
  rm = new HeapResourceMark();
  klassOop byteArrayClass = Universe::byteArrayKlassObj();
  PersistentHandle ah(byteArrayClass->klass_part()->allocateObjectSize(8));
  PersistentHandle iah(byteArrayClass->klass_part()->allocateObjectSize(8));
  PersistentHandle lu(byteArrayClass->klass_part()->allocateObjectSize(8));
  PersistentHandle ls(byteArrayClass->klass_part()->allocateObjectSize(8));

  doubleValue = doubleOop(Universe::doubleKlassObj()->klass_part()->allocateObject());
  doubleValue->set_value(1.625);

  largeUnsignedInteger = byteArrayOop(lu.as_oop());
  largeSignedInteger = byteArrayOop(ls.as_oop());

  IntegerOps::unsigned_int_to_Integer((unsigned int)0xFFFFFFFF, byteArrayOop(largeUnsignedInteger)->number());
  IntegerOps::int_to_Integer(-1 << 31, byteArrayOop(largeSignedInteger)->number());

  alien = byteArrayOop(ah.as_oop());
  byteArrayPrimitives::alienSetSize(as_smiOop(-16), alien);
  byteArrayPrimitives::alienSetAddress(as_smiOop((int)alien_byte_region), alien);
  memset(alien_byte_region, 0, 16);

  invalidAlien = byteArrayOop(iah.as_oop());
  byteArrayPrimitives::alienSetSize(as_smiOop(-16), invalidAlien);
  byteArrayPrimitives::alienSetAddress(as_smiOop(0), invalidAlien);
}

virtual void TearDown(){
  delete rm;
  rm = NULL;
}
};

TEST_F(IndirectAlienPrimsTests, alienUnsignedByteAtPutShouldSetUnsignedByte) {
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedByteAt(as_smiOop(1), alien);

  checkSmallInteger(result, 255);
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedByteAtPutShouldReturnAssignedByte) {
  oop result = byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  
  checkSmallInteger(result, 255);
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtShouldReturnCorrectByte) {
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  checkSmallInteger(result, -1);
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtPutShouldReturnCorrectByte) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(-1), as_smiOop(1), alien);

  checkSmallInteger(result, -1);
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtPutShouldSetCorrectByte) {
  byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(-1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  checkSmallInteger(result, -1);
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtPutShouldSetMaxValue) {
  byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(127), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  checkSmallInteger(result, 127);
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtPutShouldSetMinValue) {
  byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(-128), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  checkSmallInteger(result, -128);
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedShortAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(2), alien);

  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(1), alien);
  checkSmallInteger(result, 65535);
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedShortAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(1), alien);

  checkSmallInteger(result, 65535);
}

TEST_F(IndirectAlienPrimsTests, alienSignedShortAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);

  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(1), alien);
  checkSmallInteger(result, -1);
}

TEST_F(IndirectAlienPrimsTests, alienSignedShortAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(-1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(1), alien);

  checkSmallInteger(result, -1);
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedLongAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(3), alien);

  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), alien);

  checkLargeUnsigned(result, 0xFFFFFFFF);
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedLongAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienUnsignedLongAtPut(largeUnsignedInteger, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), alien);

  checkLargeUnsigned(result, 0xFFFFFFFF);
}

TEST_F(IndirectAlienPrimsTests, alienSignedLongAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedLongAtPut(largeUnsignedInteger, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), alien);

  checkSmallInteger(result, -1);
}

TEST_F(IndirectAlienPrimsTests, alienSignedLongAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienSignedLongAtPut(largeSignedInteger, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), alien);

  checkLargeInteger(result, -1 << 31);
}

TEST_F(IndirectAlienPrimsTests, alienDoubleAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
  ASSERT_EQ(1.625, ((double*)alien_byte_region)[0]) << "value not set";
}

TEST_F(IndirectAlienPrimsTests, alienDoubleAtPutShouldSetValueAtSecondByte) {
  byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(2), alien);
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(2), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
  ASSERT_EQ(1.625, ((double*)(alien_byte_region + 1))[0]) << "value not set";
}

TEST_F(IndirectAlienPrimsTests, alienFloatAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
  ASSERT_EQ(1.625F, ((float*)(alien_byte_region))[0]) << "value not set";
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedByteAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienUnsignedByteAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedByteAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(1), as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(1), as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedShortAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(1), as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienSignedShortAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(1), as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedLongAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(as_smiOop(1), as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienSignedLongAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(as_smiOop(1), as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienDoubleAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienDoubleAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienFloatAtShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(1), invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}

TEST_F(IndirectAlienPrimsTests, alienFloatAtPutShouldReturnMarkedSymbolWhenAddressInvalid) {
  oop result = byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(1),invalidAlien);

  checkMarkedSymbol("invalid address", result, vmSymbols::illegal_state());
}
