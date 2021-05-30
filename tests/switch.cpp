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

#include <torquescript/interpreter.hpp>
#include <torquescript/storedvalue.hpp>
#include <torquescript/builtins.hpp>
#include <torquescript/executionstate.hpp>

TEST(InterpreterTest, Switch)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/switch.cs", state);

    TorqueScript::StoredValue* resultOne = interpreter.getGlobal("global::one");
    EXPECT_TRUE(resultOne);
    TorqueScript::StoredValue* resultTwo = interpreter.getGlobal("global::two");
    EXPECT_TRUE(resultTwo);
    TorqueScript::StoredValue* resultThree = interpreter.getGlobal("global::three");
    EXPECT_TRUE(resultThree);
    TorqueScript::StoredValue* resultFour = interpreter.getGlobal("global::four");
    EXPECT_TRUE(resultFour);

    ASSERT_EQ(resultOne->toInteger(state), 5);
    ASSERT_EQ(resultTwo->toInteger(state), 5);
    ASSERT_EQ(resultThree->toInteger(state), 10);
    ASSERT_EQ(resultFour->toInteger(state), -10);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
