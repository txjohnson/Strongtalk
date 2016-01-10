#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_byteArray_prims.cpp.incl"


extern "C" int expansion_count;
class DirectAlienPrimsTests : public ::testing::Test {
protected:
  HeapResourceMark *rm;
  byteArrayOop alien;
  byteArrayOop largeUnsignedInteger;
  byteArrayOop largeUnsignedInteger2;
  byteArrayOop veryLargeUnsignedInteger;
  byteArrayOop largeSignedInteger;
  doubleOop    doubleValue;

  void checkMarkedSymbol(char* message, oop result, symbolOop expected) {
    char text[200];
    ASSERT_TRUE(result->is_mark()) << "Should be marked";
    sprintf(text,"Should be: %s, was: %s", message, unmarkSymbol(result)->as_string());
    ASSERT_TRUE(unmarkSymbol(result) == expected) << text;
  }
  void checkLargeInteger(oop result, int expected) {
    char message[200];
    ASSERT_TRUE(result->is_byteArray()) << "Should be integer";
    bool ok;
    int actual = byteArrayOop(result)->number().as_int(ok);
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

virtual void SetUp() {
  rm = new HeapResourceMark();
  PersistentHandle bac(Universe::byteArrayKlassObj());
  PersistentHandle ah(klassOop(bac.as_oop())->klass_part()->allocateObjectSize(12));
  PersistentHandle lu(klassOop(bac.as_oop())->klass_part()->allocateObjectSize(8));
  PersistentHandle lu2(klassOop(bac.as_oop())->klass_part()->allocateObjectSize(8));
  PersistentHandle vlu(klassOop(bac.as_oop())->klass_part()->allocateObjectSize(12));
  PersistentHandle ls(klassOop(bac.as_oop())->klass_part()->allocateObjectSize(8));

  alien = byteArrayOop(ah.as_oop());
  largeUnsignedInteger = byteArrayOop(lu.as_oop());
  largeUnsignedInteger2 = byteArrayOop(lu2.as_oop());
  veryLargeUnsignedInteger = byteArrayOop(vlu.as_oop());
  largeSignedInteger = byteArrayOop(ls.as_oop());
  doubleValue = doubleOop(Universe::doubleKlassObj()->klass_part()->allocateObject());
  doubleValue->set_value(1.625);

  IntegerOps::unsigned_int_to_Integer((unsigned int)0xFFFFFFFF, byteArrayOop(largeUnsignedInteger)->number());
  IntegerOps::int_to_Integer(1 << 30, byteArrayOop(largeUnsignedInteger2)->number());
  IntegerOps::int_to_Integer(-1 << 31, byteArrayOop(largeSignedInteger)->number());
  IntegerOps::mul(largeUnsignedInteger->number(), largeUnsignedInteger->number(), veryLargeUnsignedInteger->number());
  byteArrayPrimitives::alienSetSize(as_smiOop(8), alien);
}

virtual void TearDown(){
  MarkSweep::collect();
  delete rm;
  rm = NULL;
}
};

TEST_F(DirectAlienPrimsTests, alienUnsignedByteAtPutShouldSetUnsignedByte) {
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedByteAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "result should be SmallInteger";
  ASSERT_EQ(255, smiOop(result)->value()) << "Should set value";
}

TEST_F(DirectAlienPrimsTests, alienUnsignedByteAtPutShouldReturnAssignedByte) {
  oop result = byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  ASSERT_TRUE(result->is_smi()) << "result should be SmallInteger";
  ASSERT_EQ(255, smiOop(result)->value()) << "Should return value";
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtShouldReturnSignedValue) {
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "result should be SmallInteger";
  ASSERT_EQ(-1, smiOop(result)->value()) << "Should return correct value";
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtShouldReturnMarkedSymbolWhenIndexNotSmallInteger) {
  oop result = byteArrayPrimitives::alienSignedByteAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(0), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(9), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnSignedValue) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(-1), as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "result should be SmallInteger";
  ASSERT_EQ(-1, smiOop(result)->value()) << "Should return correct value";
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(-1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "result should be SmallInteger";
  ASSERT_EQ(-1, smiOop(result)->value()) << "Should return correct value";
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldSetCorrectValueWhenPositive) {
  byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedByteAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "result should be SmallInteger";
  ASSERT_EQ(1, smiOop(result)->value()) << "Should return correct value";
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(1), as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenIndexNotSmallInteger) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(1), alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenValieNotSmallInteger) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(alien, as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(0), as_smiOop(0), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenIndexTooBig) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(0), as_smiOop(9), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenValueTooSmall) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(-129), as_smiOop(1), alien);

  checkMarkedSymbol("invalid value", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienSignedByteAtPutShouldReturnMarkedSymbolWhenValueTooBig) {
  oop result = byteArrayPrimitives::alienSignedByteAtPut(as_smiOop(128), as_smiOop(1), alien);

  checkMarkedSymbol("invalid value", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(1), alien);
  byteArrayPrimitives::alienUnsignedByteAtPut(as_smiOop(255), as_smiOop(2), alien);

  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(1), alien);
  ASSERT_TRUE(result->is_smi()) << "should be smi";
  ASSERT_EQ(65535, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienUnsignedShortAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(0), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtShouldReturnMarkedSymbolWhenIndexTooBig) {
  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(8), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnCorrectValue) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "should be smi";
  ASSERT_EQ(65535, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedShortAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "should be smi";
  ASSERT_EQ(65535, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(1), as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(1), alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenValueNotSMI) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(alien, as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(1), as_smiOop(0), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenIndexTooBig) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(1), as_smiOop(8), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenValueTooSmall) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(-1), as_smiOop(1), alien);

  checkMarkedSymbol("value too small", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedShortAtPutShouldReturnMarkedSymbolWhenValueTooBig) {
  oop result = byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65536), as_smiOop(1), alien);

  checkMarkedSymbol("value too small", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);

  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(1), alien);
  ASSERT_TRUE(result->is_smi()) << "should be smi";
  ASSERT_EQ(-1, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienSignedShortAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(0), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtShouldReturnMarkedSymbolWhenIndexTooBig) {
  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(8), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnCorrectValue) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(-1), as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "should be smi";
  ASSERT_EQ(-1, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(-1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedShortAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "should be smi";
  ASSERT_EQ(-1, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(1), as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(1), alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenValueNotSMI) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(alien, as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(1), as_smiOop(0), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenIndexTooBig) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(1), as_smiOop(8), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenValueTooSmall) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(-32769), as_smiOop(1), alien);

  checkMarkedSymbol("value too small", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienSignedShortAtPutShouldReturnMarkedSymbolWhenValueTooBig) {
  oop result = byteArrayPrimitives::alienSignedShortAtPut(as_smiOop(32768), as_smiOop(1), alien);

  checkMarkedSymbol("value too small", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(1), alien);
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(65535), as_smiOop(3), alien);

  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), alien);

  checkLargeUnsigned(result, 0xFFFFFFFF);
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtShouldReturnSmallIntegerWhenResultSmall) {
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(1), as_smiOop(1), alien);
  byteArrayPrimitives::alienUnsignedShortAtPut(as_smiOop(0), as_smiOop(3), alien);

  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "Should be small integer";
  ASSERT_EQ(1, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienUnsignedLongAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(0), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtShouldReturnMarkedSymbolWhenIndexTooBig) {
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(6), alien);

  checkMarkedSymbol("index invalid", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnCorrectValue) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(largeUnsignedInteger, as_smiOop(1), alien);

  checkLargeUnsigned(result, 0xFFFFFFFF);
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienUnsignedLongAtPut(largeUnsignedInteger, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), alien);

  checkLargeUnsigned(result, 0xFFFFFFFF);
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenValueTooLarge) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(veryLargeUnsignedInteger, as_smiOop(1), alien);

  checkMarkedSymbol("invalid value", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldSetCorrectValueWithSMI) {
  byteArrayPrimitives::alienUnsignedLongAtPut(as_smiOop(1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienUnsignedLongAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "Should be small integer";
  ASSERT_TRUE(1 == smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(as_smiOop(1), as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(as_smiOop(1), alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenValueNotInteger) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(Universe::byteArrayKlassObj(), as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(as_smiOop(1), as_smiOop(0), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienUnsignedLongAtPutShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienUnsignedLongAtPut(as_smiOop(1), as_smiOop(6), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtShouldReturnCorrectValue) {
  byteArrayPrimitives::alienUnsignedLongAtPut(largeUnsignedInteger, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_smi()) << "Should be integer";
  ASSERT_EQ(-1, smiOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtShouldReturnLargeInteger) {
  byteArrayPrimitives::alienUnsignedLongAtPut(largeUnsignedInteger2, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), alien);

  checkLargeInteger(result, 1 << 30);
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienSignedLongAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(0), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(6), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnCorrectValue) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(largeSignedInteger, as_smiOop(1), alien);

  checkLargeInteger(result, -1 << 31);
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienSignedLongAtPut(largeSignedInteger, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), alien);

  checkLargeInteger(result, -1 << 31);
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutWithSMIShouldSetCorrectValue) {
  byteArrayPrimitives::alienSignedLongAtPut(as_smiOop(1), as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienSignedLongAt(as_smiOop(1), alien);

  checkSmallInteger(result, 1);
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(as_smiOop(1), as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(as_smiOop(1), alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenValueNotInteger) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(Universe::byteArrayKlassObj(), as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(as_smiOop(1), as_smiOop(0), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(as_smiOop(1), as_smiOop(6), alien);

  checkMarkedSymbol("invalid index", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienSignedLongAtPutShouldReturnMarkedSymbolWhenValueTooLarge) {
  oop result = byteArrayPrimitives::alienSignedLongAtPut(veryLargeUnsignedInteger, as_smiOop(1), alien);

  checkMarkedSymbol("invalid value", result, vmSymbols::argument_is_invalid());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldReturnValue) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(1), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
  ASSERT_EQ(1.625, ((double*)(alien->bytes() + 4))[0]) << "value not set";
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(doubleValue, alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienDoubleAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldReturnMarkedSymbolWhenValueNotDouble) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(as_smiOop(1), as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(0), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(0), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtPutShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienDoubleAtPut(doubleValue, as_smiOop(2), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienDoubleAtShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienDoubleAt(as_smiOop(2), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldSetCorrectValue) {
  byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(1), alien);
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(1), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
  ASSERT_EQ(1.625F, ((float*)(alien->bytes() + 4))[0]) << "value not set";
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldSetCorrectValueAt2) {
  byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(2), alien);
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(2), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
  ASSERT_EQ(1.625F, ((float*)(alien->bytes() + 5))[0]) << "value not set";
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldReturnCorrectValue) {
  oop result = byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(1), alien);

  ASSERT_TRUE(result->is_double()) << "should be double";
  ASSERT_EQ(1.625, doubleOop(result)->value()) << "wrong value";
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtShouldReturnMarkedSymbolWhenNotAlien) {
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(1), as_smiOop(0));

  checkMarkedSymbol("wrong type", result, vmSymbols::receiver_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienFloatAtPut(doubleValue, alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::first_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtShouldReturnMarkedSymbolWhenIndexNotSMI) {
  oop result = byteArrayPrimitives::alienFloatAt(alien, alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldReturnMarkedSymbolWhenValueNotDouble) {
  oop result = byteArrayPrimitives::alienFloatAtPut(as_smiOop(1), as_smiOop(1), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::second_argument_has_wrong_type());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(0), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldReturnMarkedSymbolWhenIndexTooSmall) {
  oop result = byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(0), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtPutShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienFloatAtPut(doubleValue, as_smiOop(6), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}

TEST_F(DirectAlienPrimsTests, alienFloatAtShouldReturnMarkedSymbolWhenIndexTooLarge) {
  oop result = byteArrayPrimitives::alienFloatAt(as_smiOop(6), alien);

  checkMarkedSymbol("wrong type", result, vmSymbols::index_not_valid());
}
