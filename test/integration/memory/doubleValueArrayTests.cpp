#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_dValueArrayKlass.cpp.incl"


// FIXME: uncomment tests once status of dValueArray* is firmly established

extern "C" oop* eden_top;
extern "C" oop* eden_end;

class DoubleValueArrayKlassTests : public ::testing::Test {
protected:
  klassOop theClass;
  oop* oldEdenTop;

virtual void SetUp() {
  theClass = klassOop(Universe::find_global("DoubleValueArray"));
  oldEdenTop = eden_top;
}

virtual void TearDown() {
  eden_top = oldEdenTop;
  MarkSweep::collect();
}
};

//TEST_F(DoubleValueArrayKlassTests, shouldBeDoubleDoubleValueArray) {
//  eden_top = eden_end;
//  ASSERT_TRUE(theClass->klass_part()->oop_is_DoubleValueArray());
//}
//TEST_F(DoubleValueArrayKlassTests, allocateShouldFailWhenAllowedAndNoSpace) {
//  eden_top = eden_end;
//  ASSERT_EQ((int)NULL, (int)(theClass->klass_part()->allocateObjectSize(100, false)));
//}

//TEST_F(DoubleValueArrayKlassTests, allocateShouldAllocateTenuredWhenRequired) {
//  ASSERT_TRUE(Universe::old_gen.contains(theClass->klass_part()->allocateObjectSize(100, false, true)));
//}

//TEST_F(DoubleValueArrayKlassTests, allocateShouldNotFailWhenNotAllowedAndNoSpace) {
//  eden_top = eden_end;
//  ASSERT_TRUE(Universe::new_gen.eden()->free() < 4 * oopSize);
//  ASSERT_TRUE(Universe::new_gen.contains(theClass->klass_part()->allocateObjectSize(100, true)));
//}

class findDoubleValueArray : public klassOopClosure {
public:
  char* className;
  bool found;
  findDoubleValueArray() {
    className = NULL;
    found = false;
  }
  void do_klass(klassOop klass) {
    oop instance = klass->primitive_allocate_size(1);
    if (instance->is_doubleValueArray()){
      symbolOop name = symbolOop(klass->instVarAt(klassOopDesc::header_size()));
      char* sname = name->chars();
      className = sname;
      found = true;
      printf("Class name is : %s)", sname);
    }
  }
};

//TEST_F(DoubleValueArrayKlassTests, findDoubleValueArrayClass) {
//  findDoubleValueArray closure;
//  Universe::classes_do(&closure);
//  ASSERT_TRUE(closure.found) << "No matching class found :(";
//}
