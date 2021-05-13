#include "gtest/gtest.h"

#include <torquescript/compiler/interpreter.hpp>

TEST(FactorialTest, Zero) {
    EXPECT_EQ(1, 1);
}

int main()
{
    return RUN_ALL_TESTS();
}
