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

#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <torquescript/nativefunction.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/consoleobject.hpp>
#include <torquescript/fileobject.hpp>

namespace TorqueScript
{
    static void EchoBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
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

    static void ExecBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
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

    static void ActivatePackageBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string activatedPackage = stack.popString(state);
            state->mInterpreter->activateFunctionRegistry(activatedPackage);
        }

        stack.push_back(StoredValue(0));
    }

    static void DeactivatePackageBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string deactivatedPackage = stack.popString(state);
            state->mInterpreter->deactivateFunctionRegistry(deactivatedPackage);
        }

        stack.push_back(StoredValue(0));
    }

    static void DeleteBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        state->mInterpreter->mConsoleObjectRegistry.removeConsoleObject(thisObject);
        stack.push_back(StoredValue(0));
    }

    static void GetNameBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const std::size_t stringID = state->mInterpreter->mStringTable.getOrAssign(state->mInterpreter->mConsoleObjectRegistry.getConsoleObjectName(thisObject));
        stack.push_back(StoredValue(stringID, StoredValueType::String));
    }

    static void GetClassNameBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        const std::size_t stringID = state->mInterpreter->mStringTable.getOrAssign(thisObject->getClassName());
        stack.push_back(StoredValue(stringID, StoredValueType::String));
    }

    static void registerCoreLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(EchoBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "echo")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ExecBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "exec")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ActivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "activatePackage")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeactivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "deactivatePackage")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetClassNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getClassName")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getName")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeleteBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "delete")));
    }
}