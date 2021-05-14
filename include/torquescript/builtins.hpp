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

#include <iostream>
#include <stdexcept>

#include <torquescript/function.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>

namespace TorqueScript
{
    class Echo : public Function
    {
        public:
            Echo() : Function("echo") { }

            /**
             *  @brief Default implementation will execute virtual instructions but can be overriden to implement native
             *  functions.
             */
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                // Retrieve string to print from stack
                StoredVariable* printedPayload = stack.back();
                stack.pop_back();

                // Ensure we have a string value here - it should be impossible to get anything else as a call name
                // assert(calledFunctionParameter.getVariableType() == StoredVariable::VariableType::STRING);

                std::string printedValue = printedPayload->toString();
                if (printedPayload->getVariableType() == StoredVariable::VariableType::LOCALREFERENCE)
                {
                    StoredVariable* variable = scope->getVariable(printedValue);
                    printedValue = variable ? variable->toString() : "";
                }
                else if (printedPayload->getVariableType() == StoredVariable::VariableType::GLOBALREFERENCE)
                {
                    StoredVariable* variable = interpreter->getGlobal(printedValue);
                    printedValue = variable ? variable->toString() : "";
                }

                std::cout << "Echo > " << printedValue << std::endl;
            }
    };

    void registerBuiltIns(Interpreter* interpreter)
    {
        interpreter->addFunction(new Echo());
    }
}
