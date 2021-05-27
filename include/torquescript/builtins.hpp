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
#include <stdexcept>

#include <torquescript/nativefunction.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/storedintegervalue.hpp>
#include <torquescript/storedstringvalue.hpp>
#include <torquescript/storedfloatvalue.hpp>
#include <torquescript/consoleobject.hpp>

namespace TorqueScript
{
    static void EchoBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        std::string outputString = "";

        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            // Parameters will flow right to left so we build the string considering this
            std::string printedPayload = state->mStack.popString(state);
            outputString = printedPayload + outputString;
        }

        state->mInterpreter->logEcho(outputString);
        state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
    }

    static void ActivatePackageBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string activatedPackage = state->mStack.popString(state);
            state->mInterpreter->activateFunctionRegistry(activatedPackage);
        }

        state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
    }

    static void DeactivatePackageBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            std::string deactivatedPackage = state->mStack.popString(state);
            state->mInterpreter->deactivateFunctionRegistry(deactivatedPackage);
        }

        state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
    }

    static void DeleteBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        state->mInterpreter->mConsoleObjectRegistry.removeConsoleObject(thisObject);
        state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
    }

    static void GetNameBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredStringValue(state->mInterpreter->mConsoleObjectRegistry.getConsoleObjectName(thisObject))));
    }

    static void GetRandomBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        // FIXME: If argC == 1, generate int between 0 and value, if argC == 2, generate int between min and max
        const float result = (float)std::rand() / RAND_MAX;
        state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(result)));
    }

    void registerBuiltIns(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(EchoBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "echo")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetRandomBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getRandom")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ActivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "activatePackage")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeactivatePackageBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "deactivatePackage")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetNameBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "getName")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeleteBuiltIn, PACKAGE_EMPTY, "ConsoleObject", "delete")));
    }
}
