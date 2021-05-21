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

#include <torquescript/function.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/storedintegervalue.hpp>

namespace TorqueScript
{
    class Echo : public Function
    {
        public:
            Echo() : Function(NAMESPACE_EMPTY, "echo") { }

            virtual void execute(std::shared_ptr<ExecutionState> state, const unsigned int argumentCount) override
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
    };

    class ActivatePackage : public Function
    {
        public:
            ActivatePackage() : Function(NAMESPACE_EMPTY, "activatepackage") { }

            virtual void execute(std::shared_ptr<ExecutionState> state, const unsigned int argumentCount) override
            {
                std::string outputString = "";

                for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
                {
                    std::string activatedPackage = state->mStack.popString(state);
                    state->mInterpreter->activateFunctionRegistry(activatedPackage);
                }

                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
            }
    };

    class DeactivatePackage : public Function
    {
        public:
            DeactivatePackage() : Function(NAMESPACE_EMPTY, "deactivatepackage") { }

            virtual void execute(std::shared_ptr<ExecutionState> state, const unsigned int argumentCount) override
            {
                std::string outputString = "";

                for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
                {
                    std::string deactivatedPackage = state->mStack.popString(state);
                    state->mInterpreter->deactivateFunctionRegistry(deactivatedPackage);
                }

                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
            }
    };

    void registerBuiltIns(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new Echo()), PACKAGE_EMPTY);
        interpreter->addFunction(std::shared_ptr<Function>(new DeactivatePackage()), PACKAGE_EMPTY);
        interpreter->addFunction(std::shared_ptr<Function>(new ActivatePackage()), PACKAGE_EMPTY);
    }
}
