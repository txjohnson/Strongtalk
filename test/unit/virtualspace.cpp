#include "gtest/gtest.h"

#include "incls/_precompiled.incl"
#include "incls/_virtualspace.cpp.incl"


void checkAlignment(int size, int expected_alignment) {
  int adjustment = ReservedSpace::page_align_size(size);
  ASSERT_EQ(expected_alignment, adjustment) << "adjustment for " << size << ": " << adjustment;
}
void checkArbitraryAlignment(int size, int expected_alignment, int page_size) {
  int adjustment = ReservedSpace::align_size(size, page_size);
  ASSERT_EQ(expected_alignment, adjustment) << "adjustment for " << size << ": " << adjustment;
}

TEST(ReservedSpace, page_align_size)
{
  // at least one page
  checkAlignment(0, os::vm_page_size());
  checkAlignment(1, os::vm_page_size());
  checkAlignment(os::vm_page_size() - 1, os::vm_page_size());
  checkAlignment(os::vm_page_size(), os::vm_page_size());

  // in chunks of one page in size
  checkAlignment(os::vm_page_size() + 1, 2 * os::vm_page_size());
  checkAlignment(2 * os::vm_page_size() - 1, 2 * os::vm_page_size());
  checkAlignment(2 * os::vm_page_size(), 2 * os::vm_page_size());
  checkAlignment(2 * os::vm_page_size() + 1, 3 * os::vm_page_size());
}

TEST(ReservedSpace, arbitrary_align_size)
{
  // at least one page
  checkArbitraryAlignment(0, 10000, 10000);
  checkArbitraryAlignment(1, 10000, 10000);
  checkArbitraryAlignment(9999, 10000, 10000);
  checkArbitraryAlignment(10000, 10000, 10000);

  // in chunks of one page in size
  checkArbitraryAlignment(10001, 20000, 10000);
  checkArbitraryAlignment(19999, 20000, 10000);
  checkArbitraryAlignment(20000, 20000, 10000);
  checkArbitraryAlignment(20001, 30000, 10000);
}
