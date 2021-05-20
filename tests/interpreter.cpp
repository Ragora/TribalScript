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

TEST(InterpreterTest, ForLoop)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/for.cs", state);

    // After execution, the result of $global should be 50
    std::shared_ptr<TorqueScript::StoredValue> result = interpreter.getGlobal("global");
    EXPECT_TRUE(result);

    ASSERT_EQ(result->toInteger(state), 50);
}

TEST(InterpreterTest, WhileLoop)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/while.cs", state);

    // After execution, the result of $global should be 110
    std::shared_ptr<TorqueScript::StoredValue> result = interpreter.getGlobal("global");
    EXPECT_TRUE(result);

    ASSERT_EQ(result->toInteger(state), 110);
}

TEST(InterpreterTest, If)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/if.cs", state);

    // Here we have three values
    std::shared_ptr<TorqueScript::StoredValue> resultOne = interpreter.getGlobal("one");
    EXPECT_TRUE(resultOne);
    std::shared_ptr<TorqueScript::StoredValue> resultTwo = interpreter.getGlobal("two");
    EXPECT_TRUE(resultTwo);
    std::shared_ptr<TorqueScript::StoredValue> resultThree = interpreter.getGlobal("three");
    EXPECT_TRUE(resultThree);
    std::shared_ptr<TorqueScript::StoredValue> resultFour = interpreter.getGlobal("four");
    EXPECT_TRUE(resultFour);

    ASSERT_EQ(resultOne->toInteger(state), 10);
    ASSERT_EQ(resultTwo->toInteger(state), -10);
    ASSERT_EQ(resultThree->toInteger(state), 200);
    ASSERT_EQ(resultFour->toInteger(state), 500);
}

TEST(InterpreterTest, Array)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/array.cs", state);

    // The assignment performed is: $result[1,2,3] = %value;
    // However, in Torque Script this is treated as a single variable key $result1_2_3
    std::shared_ptr<TorqueScript::StoredValue> result = interpreter.getGlobal("result1_2_3");
    EXPECT_TRUE(result);

    ASSERT_EQ(result->toInteger(state), 5);
}

TEST(InterpreterTest, Variables)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/variables.cs", state);

    // We have a global and a global value within a namespace
    std::shared_ptr<TorqueScript::StoredValue> resultGlobal = interpreter.getGlobal("global");
    EXPECT_TRUE(resultGlobal);
    std::shared_ptr<TorqueScript::StoredValue> resultGlobalNameSpace = interpreter.getGlobal("global::namespaced");
    EXPECT_TRUE(resultGlobalNameSpace);

    ASSERT_EQ(resultGlobal->toInteger(state), 50);
    ASSERT_EQ(resultGlobalNameSpace->toInteger(state), 123);
}

TEST(InterpreterTest, Switch)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/switch.cs", state);

    // We have several globals here
    std::shared_ptr<TorqueScript::StoredValue> resultOne = interpreter.getGlobal("global::one");
    EXPECT_TRUE(resultOne);
    std::shared_ptr<TorqueScript::StoredValue> resultTwo = interpreter.getGlobal("global::two");
    EXPECT_TRUE(resultTwo);
    std::shared_ptr<TorqueScript::StoredValue> resultThree = interpreter.getGlobal("global::three");
    EXPECT_TRUE(resultThree);
    std::shared_ptr<TorqueScript::StoredValue> resultFour = interpreter.getGlobal("global::four");
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
