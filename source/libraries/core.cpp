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

#include <chrono>
#include <assert.h>

#include <torquescript/libraries/core.hpp>

namespace TorqueScript
{
    void EchoBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        std::string outputString = "";

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            // Parameters will flow right to left so we build the string considering this
            std::string printedPayload = stack.popString(state);
            outputString = printedPayload + outputString;
        }

        state->mInterpreter->mConfig.mPlatform->logEcho(outputString);
        stack.push_back(StoredValue(0));
    }

    void ExecBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string executedFile = stack.popString(state);

            std::ostringstream output;
            output << "Executing " << executedFile << " ...";
            state->mInterpreter->mConfig.mPlatform->logEcho(output.str());

            state->mInterpreter->execute(executedFile, nullptr);
        }

        stack.push_back(StoredValue(0));
    }

    void ActivatePackageBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string activatedPackage = stack.popString(state);
            state->mInterpreter->activateFunctionRegistry(activatedPackage);
        }

        stack.push_back(StoredValue(0));
    }

    void DeactivatePackageBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string deactivatedPackage = stack.popString(state);
            state->mInterpreter->deactivateFunctionRegistry(deactivatedPackage);
        }

        stack.push_back(StoredValue(0));
    }

    void DeleteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        state->mInterpreter->mConfig.mConsoleObjectRegistry->removeConsoleObject(thisObject);
        stack.push_back(StoredValue(0));
    }

    void GetNameBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const std::string stringData = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObjectName(thisObject);
        stack.push_back(StoredValue(stringData.c_str()));
    }

    void GetClassNameBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const std::string stringData = thisObject->getClassName();
        stack.push_back(StoredValue(stringData.c_str()));
    }

    void GetRealTimeBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        auto result = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        stack.push_back(StoredValue((int)result));
    }

    void CallBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        std::vector<StoredValue> callArguments;
        for (unsigned int iteration = 0; iteration < argumentCount - 1; ++iteration)
        {
            callArguments.insert(callArguments.begin(), stack.back());
            stack.pop_back();
        }

        std::string calledFunctionName = stack.popString(state);

        // Lookup the function to call
        std::shared_ptr<Function> calledFunction = state->mInterpreter->getFunction(NAMESPACE_EMPTY, calledFunctionName);
        if (calledFunction)
        {
            ExecutionState callState = ExecutionState(state->mInterpreter);

            // Insert all arguments into the callstate
            StoredValueStack& callStack = callState.mExecutionScope.getStack();

            for (StoredValue& callArgument : callArguments)
            {
                callStack.push_back(callArgument);
            }

            calledFunction->execute(nullptr, &callState, callArguments.size());

            // FIXME: Return function result?
            stack.push_back(0);
        }
        else
        {
            stack.push_back(StoredValue(0));
        }
    }

    void registerCoreLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(EchoBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "echo")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ExecBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "exec")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(CallBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "call")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetRealTimeBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getRealTime")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ActivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "activatePackage")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeactivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "deactivatePackage")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetClassNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getClassName")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getName")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeleteBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "delete")));
    }
}
