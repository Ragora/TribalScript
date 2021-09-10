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

#include <tribalscript/libraries/simset.hpp>

namespace TribalScript
{
    StoredValue GetCountBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        SimSet* set = reinterpret_cast<SimSet*>(thisObject);

        return StoredValue((int)set->getCount());
    }

	StoredValue GetObjectBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
	{
		SimSet* set = reinterpret_cast<SimSet*>(thisObject);

		int index = parameters[0].toInteger();

		return StoredValue((int)state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObjectID(state->mInterpreter, set->getObject(index)));
	}

    void registerSimSetLibrary(Interpreter* interpreter)
    {
		interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetCountBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getCount")));

        INTERPRETER_REGISTER_CONSOLEOBJECT_TYPE(interpreter, SimSet, ConsoleObject);
        //interpreter->registerConsoleObjectType<FileObject>();
    }
}
