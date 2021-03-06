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

TEST(InterpreterTest, Array)
{
    TribalScript::InterpreterConfiguration config;
    config.mCaseSensitive = true;

    TribalScript::Interpreter interpreter(config);
    TribalScript::registerAllLibraries(&interpreter);

    TribalScript::ExecutionState state = TribalScript::ExecutionState(&interpreter);
    interpreter.execute("cases/caseSensitive.cs", &state);

    TribalScript::StoredValue* resultLower = interpreter.getGlobal("result");
    ASSERT_TRUE(resultLower);
    TribalScript::StoredValue* resultUpper = interpreter.getGlobal("RESULT");
    ASSERT_TRUE(resultUpper);

    ASSERT_EQ(resultLower->toFloat(), 2.0f);
    ASSERT_EQ(resultUpper->toFloat(), 0.5f);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
