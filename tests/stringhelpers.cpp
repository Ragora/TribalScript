/**
 *  Copyright 2021 Robert MacGregor
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <memory>

#include "gtest/gtest.h"

#include <tribalscript/stringhelpers.hpp>

TEST(StringHelpers, GetStringComponents)
{
    std::vector<std::string> result = TribalScript::getStringComponents("A^B^C^D", '^', 0, 4);
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], "A");
    ASSERT_EQ(result[1], "B");
    ASSERT_EQ(result[2], "C");
    ASSERT_EQ(result[3], "D");

    result = TribalScript::getStringComponents("A^B^C^D", '^', 0, 8);
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], "A");
    ASSERT_EQ(result[1], "B");
    ASSERT_EQ(result[2], "C");
    ASSERT_EQ(result[3], "D");

    result = TribalScript::getStringComponents("A^B^C^D", '^', 6, 4);
    ASSERT_EQ(result.size(), 0);

    result = TribalScript::getStringComponents("A^B^C^D", ' ', 0, 4);
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], "A^B^C^D");
}

TEST(StringHelpers, SetStringComponents)
{
    std::string result = TribalScript::setStringComponents("A^B^C^D", '^', 0, {
        "X",
        "Y",
        "Z"
    });

    ASSERT_EQ(result, "X^Y^Z^D");

    result = TribalScript::setStringComponents("A^B^C^D", '^', 0, {
        "X",
    });

    ASSERT_EQ(result, "X^B^C^D");
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
