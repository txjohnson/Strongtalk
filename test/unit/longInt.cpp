#include "gtest/gtest.h"

#include "incls/_precompiled.incl"
#include "incls/_longInt.cpp.incl"


TEST(longint, as_double)
{
	long_int value(1.0);
	ASSERT_DOUBLE_EQ(1.0, value.as_double());
}

TEST(longint, asDoubleFromInt)
{
	long_int value(1000, 0);
	ASSERT_DOUBLE_EQ(1000.0, value.as_double());
}

TEST(longint, asDoubleFromLongInt)
{
	long_int value(3, 1);
	double expected = (((int64_t)1)<<32) + 3;
	ASSERT_DOUBLE_EQ(expected, value.as_double());
}

TEST(longint, equality)
{
	long_int lhs(5, 10);
	long_int rhs(5, 10);
	long_int different(0,0);
	ASSERT_TRUE(lhs == rhs);
	ASSERT_TRUE(!(different == rhs));
	ASSERT_TRUE(!(lhs == different));
}

TEST(longint, inequality)
{
	long_int lhs(5, 10);
	long_int rhs(5, 10);
	long_int different(0,0);
	ASSERT_TRUE(different != rhs);
	ASSERT_TRUE(lhs != different);
	ASSERT_TRUE(!(lhs != rhs));
}

TEST(longint, subtraction)
{
	long_int minuend(5, 10);
	long_int subtrahend(2, 3);
	long_int expected(3, 7);
	ASSERT_TRUE(expected == minuend - subtrahend);
}

TEST(longint, addition)
{
	long_int augend(5, 10);
	long_int addend(2, 3);
	long_int expected(7, 13);
	ASSERT_TRUE(expected == augend + addend);
}
