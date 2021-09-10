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
#include <chrono>

#include <tribalscript/libraries/core.hpp>

namespace TribalScript
{
    StoredValue EchoBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        std::string outputString = "";

        for (unsigned int iteration = 0; iteration < parameters.size(); ++iteration)
        {
            // Parameters will flow right to left so we build the string considering this
            std::string printedPayload = parameters.back().toString();
            parameters.pop_back();

            outputString = printedPayload + outputString;
        }

        state->mInterpreter->mConfig.mPlatform->logEcho(outputString);
        return StoredValue(0);
    }

    StoredValue GetRealTimeBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        typedef std::chrono::duration<float, std::milli> millisecondFloat;

        auto currentTime = std::chrono::steady_clock::now().time_since_epoch();
        auto result = std::chrono::duration_cast<millisecondFloat>(currentTime);

        return StoredValue(result.count());
    }

    StoredValue ExecBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < parameters.size(); ++iteration)
        {
            std::string executedFile = parameters.back().toString();
            parameters.pop_back();

            std::ostringstream output;
            output << "Executing " << executedFile << " ...";
            state->mInterpreter->mConfig.mPlatform->logEcho(output.str());

            state->mInterpreter->execute(executedFile, nullptr);
        }

        return StoredValue(0);
    }

    StoredValue ActivatePackageBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < parameters.size(); ++iteration)
        {
            std::string activatedPackage = parameters.back().toString();
            state->mInterpreter->activateFunctionRegistry(activatedPackage);
            parameters.pop_back();
        }

        return StoredValue(0);
    }

    StoredValue DeactivatePackageBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < parameters.size(); ++iteration)
        {
            std::string deactivatedPackage = parameters.back().toString();
            state->mInterpreter->deactivateFunctionRegistry(deactivatedPackage);
            parameters.pop_back();
        }

        return StoredValue(0);
    }

    StoredValue DeleteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

		if (thisObject->destroy())
		{
			state->mInterpreter->mConfig.mConsoleObjectRegistry->removeConsoleObject(state->mInterpreter, thisObject);
		}

        return StoredValue(0);
    }

    StoredValue GetNameBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const std::string stringData = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObjectName(state->mInterpreter, thisObject);
        return StoredValue(stringData.c_str());
    }

    StoredValue GetClassNameBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const std::string stringData = thisObject->getClassName();
        return StoredValue(stringData.c_str());
    }

    StoredValue GetIDBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const int objectID = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObjectID(state->mInterpreter, thisObject);
        return StoredValue(objectID);
    }

    void registerCoreLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(EchoBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "echo")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ExecBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "exec")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ActivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "activatePackage")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeactivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "deactivatePackage")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetRealTimeBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getRealTime")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetClassNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getClassName")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getName")));
		interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetIDBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getID")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeleteBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "delete")));
    }
}
