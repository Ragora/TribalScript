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

#include <assert.h>

#include <torquescript/libraries/math.hpp>

namespace TorqueScript
{
    void GetRandomBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // FIXME: If argC == 1, generate int between 0 and value, if argC == 2, generate int between min and max
        const float result = (float)std::rand() / RAND_MAX;
        stack.push_back(state->mExecutionScope.allocateStoredValue(result));
    }

    void registerMathLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetRandomBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getRandom")));
    }
}
