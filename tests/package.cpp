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

TEST(InterpreterTest, Package)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::shared_ptr<TorqueScript::ExecutionState> state = interpreter.getExecutionState();
    interpreter.execute("cases/package.cs", state);

    // We have several globals here
    TorqueScript::StoredValue* before = interpreter.getGlobal("before");
    ASSERT_TRUE(before);
    TorqueScript::StoredValue* afterA = interpreter.getGlobal("afterA");
    ASSERT_TRUE(afterA);
    TorqueScript::StoredValue* afterB = interpreter.getGlobal("afterB");
    ASSERT_TRUE(afterB);

    TorqueScript::StoredValue* beforeNamespace = interpreter.getGlobal("beforenamespace");
    ASSERT_TRUE(beforeNamespace);
    TorqueScript::StoredValue* afterANamespace = interpreter.getGlobal("afterAnamespace");
    ASSERT_TRUE(afterANamespace);
    TorqueScript::StoredValue* afterBNamespace = interpreter.getGlobal("afterBnamespace");
    ASSERT_TRUE(afterBNamespace);

    ASSERT_EQ(before->toInteger(state), 1);
    ASSERT_EQ(afterA->toInteger(state), 2);
    ASSERT_EQ(afterB->toInteger(state), 3);

    ASSERT_EQ(beforeNamespace->toInteger(state), 2);
    ASSERT_EQ(afterANamespace->toInteger(state), 4);
    ASSERT_EQ(afterBNamespace->toInteger(state), 6);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
