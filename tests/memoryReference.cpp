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
#include <torquescript/libraries/libraries.hpp>
#include <torquescript/executionstate.hpp>

static float aStaticFloat = 3.14f;
static int aStaticInt = 123456;

TEST(InterpreterTest, MemoryReference)
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerAllLibraries(&interpreter);

    // Set memory references
    interpreter.setGlobal("pi", TorqueScript::StoredValue(&aStaticFloat, TorqueScript::StoredValueType::Float));
    interpreter.setGlobal("numbers", TorqueScript::StoredValue(&aStaticInt, TorqueScript::StoredValueType::Integer));

    TorqueScript::ExecutionState state = TorqueScript::ExecutionState(&interpreter);
    interpreter.execute("cases/memoryReference.cs", &state);

    TorqueScript::StoredValue* result = interpreter.getGlobal("result");
    ASSERT_TRUE(result);

    ASSERT_EQ(result->toFloat(&state), 6.28f);

    // Check if the memory has been written
    ASSERT_EQ(aStaticFloat, 1337.0f);
    ASSERT_EQ(aStaticInt, 5);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
