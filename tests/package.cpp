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

TEST(InterpreterTest, Package)
{
    TribalScript::Interpreter interpreter;
    TribalScript::registerAllLibraries(&interpreter);

    TribalScript::ExecutionState state = TribalScript::ExecutionState(&interpreter);
    interpreter.execute("cases/package.cs", &state);

    // We have several globals here
    TribalScript::StoredValue* before = interpreter.getGlobal("before");
    ASSERT_TRUE(before);
    TribalScript::StoredValue* afterA = interpreter.getGlobal("afterA");
    ASSERT_TRUE(afterA);
    TribalScript::StoredValue* afterB = interpreter.getGlobal("afterB");
    ASSERT_TRUE(afterB);

    TribalScript::StoredValue* beforeNamespace = interpreter.getGlobal("beforenamespace");
    ASSERT_TRUE(beforeNamespace);
    TribalScript::StoredValue* afterANamespace = interpreter.getGlobal("afterAnamespace");
    ASSERT_TRUE(afterANamespace);
    TribalScript::StoredValue* afterBNamespace = interpreter.getGlobal("afterBnamespace");
    ASSERT_TRUE(afterBNamespace);

    ASSERT_EQ(before->toInteger(), 1);
    ASSERT_EQ(afterA->toInteger(), 2);
    ASSERT_EQ(afterB->toInteger(), 3);

    ASSERT_EQ(beforeNamespace->toInteger(), 2);
    ASSERT_EQ(afterANamespace->toInteger(), 4);
    ASSERT_EQ(afterBNamespace->toInteger(), 6);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
