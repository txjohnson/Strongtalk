#include "gtest/gtest.h"

# include "incls/_precompiled.incl"
# include "incls/_lprintf.cpp.incl"

#include "testNotifier.hpp"


class errorTests : public ::testing::Test {
protected:
  HeapResourceMark *mark;
  Notifier* saved;
  TestNotifier* notifier;

virtual void SetUp() {
  mark = new HeapResourceMark();
  saved = Notifier::current;
  notifier = new TestNotifier;
  Notifier::current = notifier;
}

virtual void TearDown(){
  Notifier::current = saved;
  delete mark;
  mark = NULL;
}
};
TEST_F(errorTests, strcmp) {
  ASSERT_EQ(0, strcmp("format arg1", "format arg1"));
}

TEST_F(errorTests, errorShouldReportErrorWithOneArgToNotifier) {
  ::error("format %s", "arg1");
  
  ASSERT_EQ(1, notifier->errorCount());
  ASSERT_EQ(0, strcmp("format arg1", notifier->errorAt(0)));
}

TEST_F(errorTests, errorShouldReportErrorWithTwoArgToNotifier) {
  ::error("format %s %s", "arg1", "arg2");

  ASSERT_EQ(1, notifier->errorCount());
  ASSERT_EQ(0, strcmp("format arg1 arg2", notifier->errorAt(0)));
}

TEST_F(errorTests, warningShouldReportWarningWithOneArgToNotifier) {
  ::warning("format %s", "arg1");

  ASSERT_EQ(1, notifier->warningCount());
  ASSERT_EQ(0, strcmp("format arg1", notifier->warningAt(0)));
}

TEST_F(errorTests, compiler_warningShouldReportWarningWithOneArgToNotifier) {
  ::compiler_warning("format %s", "arg1");

  ASSERT_EQ(1, notifier->compilerWarningCount());
  ASSERT_EQ(0, strcmp("format arg1", notifier->compilerWarningAt(0)));
}

TEST_F(errorTests, compiler_warningShouldNotReportWarningWhenPrintCompilerWarningsIsFalse) {
  FlagSetting fl(PrintCompilerWarnings, false);

  ASSERT_EQ(0, notifier->compilerWarningCount());
  ::compiler_warning("format %s", "arg1");
  ASSERT_EQ(0, notifier->compilerWarningCount());
}
