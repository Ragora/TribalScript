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

#include <vector>
#include <assert.h>
#include <iostream>
#include <sstream>

#include <torquescript/function.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/storedvariable.hpp>

namespace TorqueScript
{
    /**
     *  @brief Base instruction class. All instructions in the interpreter should dervive
     *  from this class and implement all virtual members.
     */
    class Instruction
    {
        public:
            /**
             *  @brief Main execution method of the instruction. This serves as our
             *  switching statement that determines how opcodes will behave.
             *  @param bitstream The bitstream acting as our current stack.
             */
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) = 0;

            /**
             *  @brief Helper routine to produce a disassembly for this instruction.
             */
            virtual std::string disassemble() = 0;
    };

    /**
     *  @brief Push float instruction. This will push a floating point value to the system stack
     *  for later use in execution.
     */
    class PushFloatInstruction : public Instruction
    {
        public:
            PushFloatInstruction(const float value)
            {
                mParameter = value;
            }

            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                stack.push_back(new StoredVariable(mParameter));
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushFloat " << mParameter;
                return out.str();
            }

        private:
            //! The value to push.
            float mParameter;
    };

    /**
     *  @brief Push integer instruction. This will push an integer value to the system stack
     *  for later use in execution.
     */
    class PushIntegerInstruction : public Instruction
    {
        public:
            PushIntegerInstruction(const int value)
            {
                mParameter = value;
            }

            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                stack.push_back(new StoredVariable(mParameter));
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushInteger " << mParameter;
                return out.str();
            }

        private:
            //! The value to push.
            int mParameter;
    };

    /**
     *  @brief Push string instruction. This will push a string value to the system stack for later
     *  use in execution.
     */
    class PushStringInstruction : public Instruction
    {
        public:
            PushStringInstruction(const std::string& value)
            {
                mParameter = value;
            }

            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                stack.push_back(new StoredVariable(mParameter));
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushString " << mParameter;
                return out.str();
            }

        private:
            //! The value to push.
            std::string mParameter;
    };

    /**
     *  @brief Push a reference to a named local variable. The parameter provided here
     *  should be excluding the '%' prefix.
     */
    class PushLocalReferenceInstruction : public Instruction
    {
        public:
            PushLocalReferenceInstruction(const std::string& value)
            {
                mParameter = value;
            }

            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                stack.push_back(new StoredVariable(mParameter, StoredVariable::VariableType::LOCALREFERENCE));
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushLocalReference " << mParameter;
                return out.str();
            }

        private:
            //! The value to push.
            std::string mParameter;
    };

    /**
     *  @brief Assign to lhs with whatever is on rhs.
     */
    class AssignmentInstruction : public Instruction
    {
        public:
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                // Pull two values off the stack
                StoredVariable* rhsStored = stack.back();
                stack.pop_back();
                StoredVariable* lhsStored = stack.back();
                stack.pop_back();


                switch (lhsStored->getVariableType())
                {
                    case StoredVariable::VariableType::LOCALREFERENCE:
                        scope->setVariable(lhsStored->toString(), rhsStored);
                        break;
                    case StoredVariable::VariableType::GLOBALREFERENCE:
                        interpreter->setGlobal(lhsStored->toString(), rhsStored);
                        break;
                    default:
                        throw new std::runtime_error("Can only perform assignment ops against locals or globals!");
                }
            };

            virtual std::string disassemble() override
            {
                return "Assignment";
            }
    };

    /**
     *  @brief Push string instruction. This will push a string value to the system stack for later
     *  use in execution.
     */
    class ConcatInstruction : public Instruction
    {
        public:
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                // Pull two values off the stack
                StoredVariable* rhsStored = stack.back();
                stack.pop_back();
                StoredVariable* lhsStored = stack.back();
                stack.pop_back();

                std::string lhs = lhsStored->toString();
                std::string rhs = rhsStored->toString();

                stack.push_back(new StoredVariable(lhs + rhs));
            };

            virtual std::string disassemble() override
            {
                return "Concat";
            }
    };

    /**
     *  @brief Negate a value on the stack.
     */
    class NegateInstruction : public Instruction
    {
        public:
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                // Pull two values off the stack
                StoredVariable* storedTarget = stack.back();
                stack.pop_back();

                stack.push_back(new StoredVariable(-storedTarget->toFloat()));
            };

            virtual std::string disassemble() override
            {
                return "Negate";
            }
    };

    /**
     *  @brief Calls a function registered in memory somewhere.
     */
    class CallFunctionInstruction : public Instruction
    {
        public:
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                // Pop name from the stack
                StoredVariable* calledFunctionParameter = stack.back();
                stack.pop_back();

                // Ensure we have a string value here - it should be impossible to get anything else as a call name
                assert(calledFunctionParameter->getVariableType() == StoredVariable::VariableType::STRING);

                const std::string calledFunctionName = std::get<std::string>(calledFunctionParameter->getValue());

                Function* functionLookup = interpreter->getFunction(calledFunctionName);
                if (functionLookup)
                {
                    ExecutionScope newScope;
                    functionLookup->execute(interpreter, &newScope, stack);
                }
                else
                {
                    // FIXME: Virtual logging methods?
                    std::cerr << "Could not find function '" << calledFunctionName << "' for calling! Placing 0 on the stack." << std::endl;
                    stack.push_back(new StoredVariable(0));
                }
                //std::cout << "Calling" << calledFunctionName << std::endl;
            };

            virtual std::string disassemble() override
            {
                return "CallFunction";
            }
    };

    /**
     *  @brief Adds together two values on the stack and pushes the sum.
     */
    class AddInstruction : public Instruction
    {
        public:
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                StoredVariable* rhsStored = stack.back();
                stack.pop_back();
                StoredVariable* lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored->toFloat();
                float rhs = rhsStored->toFloat();

                const float result = lhs + rhs;
                stack.push_back(new StoredVariable(result));
            };

            virtual std::string disassemble() override
            {
                return "Add";
            }
    };

    /**
     *  @brief Adds together two values on the stack and pushes the sum.
     */
    class MultiplyInstruction : public Instruction
    {
        public:
            virtual void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack) override
            {
                StoredVariable* lhsStored = stack.back();
                stack.pop_back();
                StoredVariable* rhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats

                float lhs = lhsStored->toFloat();
                float rhs = rhsStored->toFloat();

                const float result = lhs * rhs;
                stack.push_back(new StoredVariable(result));
            };

            virtual std::string disassemble() override
            {
                return "Multiply";
            }
    };
}
