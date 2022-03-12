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

#include <tribalscript/interpreter.hpp>
#include <tribalscript/storedvalue.hpp>
#include <tribalscript/libraries/libraries.hpp>
#include <tribalscript/executionstate.hpp>

TEST(InterpreterTest, If)
{
    TribalScript::Interpreter interpreter;
    TribalScript::registerAllLibraries(&interpreter);

    TribalScript::ExecutionState state = TribalScript::ExecutionState(&interpreter, nullptr);
    interpreter.execute("cases/if.cs", &state);

    // Here we have three values
    TribalScript::StoredValue* resultOne = interpreter.getGlobal("one");
    ASSERT_TRUE(resultOne);
    TribalScript::StoredValue* resultTwo = interpreter.getGlobal("two");
    ASSERT_TRUE(resultTwo);
    TribalScript::StoredValue* resultThree = interpreter.getGlobal("three");
    ASSERT_TRUE(resultThree);
    TribalScript::StoredValue* resultFour = interpreter.getGlobal("four");
    ASSERT_TRUE(resultFour);

    ASSERT_EQ(resultOne->toInteger(), 10);
    ASSERT_EQ(resultTwo->toInteger(), -10);
    ASSERT_EQ(resultThree->toInteger(), 200);
    ASSERT_EQ(resultFour->toInteger(), 500);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
