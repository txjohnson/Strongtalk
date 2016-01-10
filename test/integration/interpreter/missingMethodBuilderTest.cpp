#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_missingMethodBuilder.cpp.incl"


class missingMethodBuilderTests : public ::testing::Test {
protected:
  HeapResourceMark *rm;
  char msg[200];

#define CHECK_OOPS(expectedOops, oops, index)\
    oop expected = expectedOops[index];\
    oop actual = oops->obj_at(index + 1);\
    sprintf(msg, "Incorrect oop at %d. Expected: 0x%x, but was: 0x%x",\
      index, expected, actual);\
    ASSERT_EQ(int(expected), int(actual)) << msg

  int instVarIndex(klassOop targetClass, char* instVarName) {
    symbolOop varNameSymbol = oopFactory::new_symbol(instVarName);
    return targetClass->klass_part()->lookup_inst_var(varNameSymbol);
  }

virtual void SetUp() {
  rm = new HeapResourceMark();
}

virtual void TearDown() {
  delete rm;
  rm = NULL;
}
};

TEST_F(missingMethodBuilderTests, buildWithNoArgSelectorShouldBuildCorrectBytes) {
  BlockScavenge bs;
  char msg[200];
  symbolOop selector = oopFactory::new_symbol("value");

  unsigned char expectedBytes[52] = {
    Bytecodes::push_global, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_self,
    Bytecodes::push_literal, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_literal, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::interpreted_send_n, 3, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::interpreted_send_self, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::return_tos_pop_0, 0xFF, 0xFF, 0xFF
  };

  MissingMethodBuilder builder(selector);
  builder.build();

  byteArrayOop bytes = builder.bytes();

  ASSERT_EQ(52, bytes->length()) << "wrong length";

  for (int index = 0; index < 52; index++) {
    unsigned char expected = expectedBytes[index];
    unsigned char actual = bytes->byte_at(index + 1);
    sprintf(msg, "Incorrect byte at %d. Expected: %d, but was: %d",
      index, expected, actual);
    ASSERT_EQ(expected, actual) << msg;
  }
}
TEST_F(missingMethodBuilderTests, buildWithNoArgSelectorShouldBuildCorrectOops) {
  BlockScavenge bs;
  char msg[200];
  symbolOop selector = oopFactory::new_symbol("value");

  MissingMethodBuilder builder(selector);
  builder.build();
  objArrayOop oops = builder.oops();
  oop expectedOops[13] = {
    as_smiOop(0),
    Universe::find_global_association("Message"),
    as_smiOop(0),
    selector,
    as_smiOop(0),
    as_smiOop(0),
    as_smiOop(0),
    oopFactory::new_symbol("receiver:selector:arguments:"),
    as_smiOop(0),
    as_smiOop(0),
    oopFactory::new_symbol("doesNotUnderstand:"),
    as_smiOop(0),
    as_smiOop(0)
  };
  for (int index = 0; index < 5; index++) {
    CHECK_OOPS(expectedOops, oops, index);
  }
  oop obj = oops->obj_at(6);
  ASSERT_TRUE(obj->is_objArray()) << "Wrong type";
  objArrayOop array = objArrayOop(obj);
  ASSERT_EQ(0, array->length()) << "Wrong length";
  for (int index = 6; index < 13; index++) {
    CHECK_OOPS(expectedOops, oops, index);
  }
}

typedef oop (call_delta_func)(void* method, oop receiver, int nofArgs, oop* args);

TEST_F(missingMethodBuilderTests, buildWithNoArgSelectorShouldBuildCorrectMethod) {
  call_delta_func* _call_delta = (call_delta_func*)StubRoutines::call_delta();
  BlockScavenge bs;
  symbolOop selector = oopFactory::new_symbol("value");

  MissingMethodBuilder builder(selector);
  builder.build();
  methodOop method = builder.method();

  klassOop objectClass = klassOop(Universe::find_global("DoesNotUnderstandFixture"));
  klassOop messageClass = klassOop(Universe::find_global("Message"));

  oop fixture = objectClass->klass_part()->allocateObject();
  memOop result = memOop(_call_delta(method, fixture, 0, NULL));

  ASSERT_TRUE(result->is_mem()) << "Wrong type";
  ASSERT_TRUE(result->klass() == messageClass) << "Wrong class";
  int receiverIndex = instVarIndex(messageClass, "receiver");
  int selectorIndex = instVarIndex(messageClass, "selector");
  int argumentsIndex = instVarIndex(messageClass, "arguments");

  ASSERT_TRUE(fixture == result->instVarAt(receiverIndex)) << "Wrong receiver";
  ASSERT_TRUE(selector == result->instVarAt(selectorIndex)) << "Wrong selector";

  objArrayOop arguments = objArrayOop(result->instVarAt(argumentsIndex));
  ASSERT_TRUE(arguments->is_objArray()) << "Wrong type";
  ASSERT_TRUE(0 == arguments->length()) << "Wrong length";
}
TEST_F(missingMethodBuilderTests, buildWithOneArgSelectorShouldBuildCorrectMethod) {
  call_delta_func* _call_delta = (call_delta_func*)StubRoutines::call_delta();
  BlockScavenge bs;
  symbolOop selector = oopFactory::new_symbol("value:");

  MissingMethodBuilder builder(selector);
  builder.build();
  methodOop method = builder.method();

  klassOop objectClass = klassOop(Universe::find_global("DoesNotUnderstandFixture"));
  klassOop messageClass = klassOop(Universe::find_global("Message"));

  oop fixture = objectClass->klass_part()->allocateObject();
  oop arg1in =  as_smiOop(53);
  memOop result = memOop(_call_delta(method, fixture, 1, &arg1in));

  ASSERT_TRUE(result->is_mem()) << "Wrong type";
  ASSERT_TRUE(result->klass() == messageClass) << "Wrong class";
  int receiverIndex = instVarIndex(messageClass, "receiver");
  int selectorIndex = instVarIndex(messageClass, "selector");
  int argumentsIndex = instVarIndex(messageClass, "arguments");

  ASSERT_TRUE(fixture == result->instVarAt(receiverIndex)) << "Wrong receiver";
  ASSERT_TRUE(selector == result->instVarAt(selectorIndex)) << "Wrong selector";

  objArrayOop arguments = objArrayOop(result->instVarAt(argumentsIndex));
  ASSERT_TRUE(arguments->is_objArray()) << "Arguments has wrong type";
  ASSERT_TRUE(1 == arguments->length()) << "Wrong argument count";
  smiOop arg1 = smiOop(arguments->obj_at(1));
  ASSERT_TRUE(arg1->is_smi()) << "Argument 1 has wrong type";
  ASSERT_EQ(53, arg1->value()) << "Argument 1 has wrong value";
}
TEST_F(missingMethodBuilderTests, buildWithTwoArgSelectorShouldBuildCorrectMethod) {
  call_delta_func* _call_delta = (call_delta_func*)StubRoutines::call_delta();
  BlockScavenge bs;
  symbolOop selector = oopFactory::new_symbol("value:value:");

  MissingMethodBuilder builder(selector);
  builder.build();
  methodOop method = builder.method();

  klassOop objectClass = klassOop(Universe::find_global("DoesNotUnderstandFixture"));
  klassOop messageClass = klassOop(Universe::find_global("Message"));

  oop fixture = objectClass->klass_part()->allocateObject();
  oop args[] = { as_smiOop(42), as_smiOop(53) };
  memOop result = memOop(_call_delta(method, fixture, 2, args));

  ASSERT_TRUE(result->is_mem()) << "Wrong type";
  ASSERT_TRUE(result->klass() == messageClass) << "Wrong class";
  int receiverIndex = instVarIndex(messageClass, "receiver");
  int selectorIndex = instVarIndex(messageClass, "selector");
  int argumentsIndex = instVarIndex(messageClass, "arguments");

  ASSERT_TRUE(fixture == result->instVarAt(receiverIndex)) << "Wrong receiver";
  ASSERT_TRUE(selector == result->instVarAt(selectorIndex)) << "Wrong selector";

  objArrayOop arguments = objArrayOop(result->instVarAt(argumentsIndex));
  ASSERT_TRUE(arguments->is_objArray()) << "Arguments has wrong type";
  ASSERT_TRUE(2 == arguments->length()) << "Wrong argument count";

  smiOop arg1 = smiOop(arguments->obj_at(1));
  ASSERT_TRUE(arg1->is_smi()) << "Argument 1 has wrong type";
  ASSERT_EQ(42, arg1->value()) << "Argument 1 has wrong value";

  smiOop arg2 = smiOop(arguments->obj_at(2));
  ASSERT_TRUE(arg2->is_smi()) << "Argument 2 has wrong type";
  ASSERT_EQ(53, arg2->value()) << "Argument 2 has wrong value";
}
TEST_F(missingMethodBuilderTests, buildWithThreeArgSelectorShouldBuildCorrectMethod) {
  call_delta_func* _call_delta = (call_delta_func*)StubRoutines::call_delta();
  BlockScavenge bs;
  symbolOop selector = oopFactory::new_symbol("value:value:value:");

  MissingMethodBuilder builder(selector);
  builder.build();
  methodOop method = builder.method();

  klassOop objectClass = klassOop(Universe::find_global("DoesNotUnderstandFixture"));
  klassOop messageClass = klassOop(Universe::find_global("Message"));

  oop fixture = objectClass->klass_part()->allocateObject();
  oop args[] = { as_smiOop(11), as_smiOop(42), as_smiOop(53) };
  memOop result = memOop(_call_delta(method, fixture, 3, args));

  ASSERT_TRUE(result->is_mem()) << "Wrong type";
  ASSERT_TRUE(result->klass() == messageClass) << "Wrong class";
  int receiverIndex = instVarIndex(messageClass, "receiver");
  int selectorIndex = instVarIndex(messageClass, "selector");
  int argumentsIndex = instVarIndex(messageClass, "arguments");

  ASSERT_TRUE(fixture == result->instVarAt(receiverIndex)) << "Wrong receiver";
  ASSERT_TRUE(selector == result->instVarAt(selectorIndex)) << "Wrong selector";

  objArrayOop arguments = objArrayOop(result->instVarAt(argumentsIndex));
  ASSERT_TRUE(arguments->is_objArray()) << "Arguments has wrong type";
  ASSERT_TRUE(3 == arguments->length()) << "Wrong argument count";

  smiOop arg1 = smiOop(arguments->obj_at(1));
  ASSERT_TRUE(arg1->is_smi()) << "Argument 1 has wrong type";
  ASSERT_EQ(11, arg1->value()) << "Argument 1 has wrong value";

  smiOop arg2 = smiOop(arguments->obj_at(2));
  ASSERT_TRUE(arg2->is_smi()) << "Argument 2 has wrong type";
  ASSERT_EQ(42, arg2->value()) << "Argument 2 has wrong value";

  smiOop arg3 = smiOop(arguments->obj_at(3));
  ASSERT_TRUE(arg3->is_smi()) << "Argument 3 has wrong type";
  ASSERT_EQ(53, arg3->value()) << "Argument 3 has wrong value";
}
TEST_F(missingMethodBuilderTests, buildWithOneArgSelectorShouldBuildCorrectBytes) {
  BlockScavenge bs;
  char msg[200];
  symbolOop selector = oopFactory::new_symbol("value:");

  unsigned char expectedBytes[80] = {
    Bytecodes::allocate_temp_1,
    Bytecodes::push_global, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_self,
    Bytecodes::push_literal, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_global, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_succ_n, 0,
    Bytecodes::interpreted_send_1, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::store_temp_n, 0xFF,
    Bytecodes::push_succ_n, 0,
    Bytecodes::push_arg_n, 0,
    Bytecodes::interpreted_send_2_pop, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::push_temp_0,
    Bytecodes::interpreted_send_n, 3, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::interpreted_send_self, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::return_tos_pop_1, 0xff, 0xff, 0xff
  };

  MissingMethodBuilder builder(selector);
  builder.build();

  byteArrayOop bytes = builder.bytes();

  sprintf(msg, "Wrong length. Expected: %d, but was: %d", 65, bytes->length());
  ASSERT_EQ(80, bytes->length()) << msg;

  for (int index = 0; index < 80; index++) {
    unsigned char expected = expectedBytes[index];
    unsigned char actual = bytes->byte_at(index + 1);
    sprintf(msg, "Incorrect byte at %d. Expected: %d, but was: %d",
      index, expected, actual);
    ASSERT_EQ(expected, actual) << msg;
  }
}
TEST_F(missingMethodBuilderTests, buildWithOneArgSelectorShouldBuildCorrectOops) {
  BlockScavenge bs;
  char msg[200];
  symbolOop selector = oopFactory::new_symbol("value:");

  oop expectedOops[20] = {
    as_smiOop(0),
    Universe::find_global_association("Message"),
    as_smiOop(0),
    selector,
    as_smiOop(0),
    Universe::find_global_association("Array"),
    as_smiOop(0),
    oopFactory::new_symbol("new:"),
    as_smiOop(0),
    as_smiOop(0),
    as_smiOop(0),
    oopFactory::new_symbol("at:put:"),
    as_smiOop(0),
    as_smiOop(0),
    oopFactory::new_symbol("receiver:selector:arguments:"),
    as_smiOop(0),
    as_smiOop(0),
    oopFactory::new_symbol("doesNotUnderstand:"),
    as_smiOop(0),
    as_smiOop(0)
  };

  MissingMethodBuilder builder(selector);
  builder.build();
  objArrayOop oops = builder.oops();

  for (int index = 0; index < 20; index++) {
    CHECK_OOPS(expectedOops, oops, index);
  }
}
TEST_F(missingMethodBuilderTests, buildWithTwoArgSelectorShouldBuildCorrectBytes) {
  BlockScavenge bs;
  char msg[200];
  symbolOop selector = oopFactory::new_symbol("value:value:");

  unsigned char expectedBytes[96] = {
    Bytecodes::allocate_temp_1,
    Bytecodes::push_global, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_self,
    Bytecodes::push_literal, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_global, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_succ_n, 1,
    Bytecodes::interpreted_send_1, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::store_temp_n, 0xFF,
    Bytecodes::push_succ_n, 0,
    Bytecodes::push_arg_n, 1,
    Bytecodes::interpreted_send_2_pop, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::push_temp_0,
    Bytecodes::push_succ_n, 1,
    Bytecodes::push_arg_n, 0,
    Bytecodes::interpreted_send_2_pop, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::push_temp_0,
    Bytecodes::interpreted_send_n, 3, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::interpreted_send_self, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::return_tos_pop_2, 0xff, 0xff, 0xff
  };

  MissingMethodBuilder builder(selector);
  builder.build();

  byteArrayOop bytes = builder.bytes();

  sprintf(msg, "Wrong length. Expected: %d, but was: %d", 93, bytes->length());
  ASSERT_EQ(96, bytes->length()) << msg;

  for (int index = 0; index < 96; index++) {
    unsigned char expected = expectedBytes[index];
    unsigned char actual = bytes->byte_at(index + 1);
    sprintf(msg, "Incorrect byte at %d. Expected: %d, but was: %d",
      index, expected, actual);
    ASSERT_EQ(expected, actual) << msg;
  }
}
TEST_F(missingMethodBuilderTests, buildWithThreeArgSelectorShouldBuildCorrectBytes) {
  BlockScavenge bs;
  char msg[200];
  symbolOop selector = oopFactory::new_symbol("value:value:value:");

  unsigned char expectedBytes[112] = {
    Bytecodes::allocate_temp_1,
    Bytecodes::push_global, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_self,
    Bytecodes::push_literal, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_global, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    Bytecodes::push_succ_n, 2,
    Bytecodes::interpreted_send_1, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::store_temp_n, 0xFF,
    Bytecodes::push_succ_n, 0,
    Bytecodes::push_arg_n, 2,
    Bytecodes::interpreted_send_2_pop, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::push_temp_0,
    Bytecodes::push_succ_n, 1,
    Bytecodes::push_arg_n, 1,
    Bytecodes::interpreted_send_2_pop, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::push_temp_0,
    Bytecodes::push_succ_n, 2,
    Bytecodes::push_arg_n, 0,
    Bytecodes::interpreted_send_2_pop, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::push_temp_0,
    Bytecodes::interpreted_send_n, 3, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::interpreted_send_self, 0xFF, 0xFF, 0xFF,
    0, 0, 0, 0,
    0, 0, 0, 0,
    Bytecodes::return_tos_pop_n, 3, 0xff, 0xff
  };

  MissingMethodBuilder builder(selector);
  builder.build();

  byteArrayOop bytes = builder.bytes();

  sprintf(msg, "Wrong length. Expected: %d, but was: %d", 110, bytes->length());
  ASSERT_EQ(112, bytes->length()) << msg;

  for (int index = 0; index < 112; index++) {
    unsigned char expected = expectedBytes[index];
    unsigned char actual = bytes->byte_at(index + 1);
    sprintf(msg, "Incorrect byte at %d. Expected: %d, but was: %d",
      index, expected, actual);
    ASSERT_EQ(expected, actual) << msg;
  }
}

