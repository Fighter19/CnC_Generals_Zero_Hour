#include <gtest/gtest.h>

#include "Lib/BaseType.h"
#include "Common/GameMemory.h"

TEST(FastFloat, Float2Long_Round)
{
  EXPECT_EQ(fast_float2long_round(1.0f), 1);
  EXPECT_EQ(fast_float2long_round(1.5f), 2);
  EXPECT_EQ(fast_float2long_round(1.9f), 2); 
  EXPECT_EQ(fast_float2long_round(0.0f), 0);
  EXPECT_EQ(fast_float2long_round(-1.0f), -1);
  EXPECT_EQ(fast_float2long_round(-1.5f), -2);
  EXPECT_EQ(fast_float2long_round(-1.9f), -2);
}

TEST(FastFloat, Float2Long_Trunc)
{
  EXPECT_EQ(fast_float_trunc(1.0f), 1.0f);
  EXPECT_EQ(fast_float_trunc(1.1f), 1.0f);
  EXPECT_EQ(fast_float_trunc(1.9f), 1.0f); 
  EXPECT_EQ(fast_float_trunc(0.0f), 0.0f);
  EXPECT_EQ(fast_float_trunc(-1.0f), -1.0f);
  EXPECT_EQ(fast_float_trunc(-1.5f), -1.0f);
  EXPECT_EQ(fast_float_trunc(-1.9f), -1.0f);
}

TEST(FastFloat, Float2Long_Floor)
{
  EXPECT_EQ(fast_float_floor(1.0f), 1.0f);
  EXPECT_EQ(fast_float_floor(1.1f), 1.0f);
  EXPECT_EQ(fast_float_floor(1.9f), 1.0f); 
  EXPECT_EQ(fast_float_floor(0.0f), 0.0f);
  EXPECT_EQ(fast_float_floor(-1.0f), -2.0f); // THIS IS UNEXPECTED
  EXPECT_EQ(fast_float_floor(-1.5f), -2.0f);
  EXPECT_EQ(fast_float_floor(-1.9f), -2.0f);
}

TEST(FastFloat, Float2Long_Ceil)
{
  EXPECT_EQ(fast_float_ceil(1.0f), 2.0f);  // THIS IS UNEXPECTED
  EXPECT_EQ(fast_float_ceil(1.1f), 2.0f);
  EXPECT_EQ(fast_float_ceil(1.9f), 2.0f); 
  EXPECT_EQ(fast_float_ceil(0.0f), 0.0f);
  EXPECT_EQ(fast_float_ceil(-1.0f), -1.0f);
  EXPECT_EQ(fast_float_ceil(-1.5f), -1.0f);
  EXPECT_EQ(fast_float_ceil(-1.9f), -1.0f);
}