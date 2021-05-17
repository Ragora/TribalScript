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
#include <torquescript/storedvalue.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/storedfloatvalue.hpp>
#include <torquescript/storedstringvalue.hpp>
#include <torquescript/storedintegervalue.hpp>
#include <torquescript/storedlocalreferencevalue.hpp>
#include <torquescript/storedglobalreferencevalue.hpp>

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
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) = 0;

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
            PushFloatInstruction(const float value) : mParameter(value)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                stack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(mParameter, interpreter)));
                return 1;
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
            PushIntegerInstruction(const int value) : mParameter(value)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                stack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(mParameter, interpreter)));
                return 1;
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
            PushStringInstruction(const std::string& value) : mParameter(value)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                stack.push_back(std::shared_ptr<StoredValue>(new StoredStringValue(mParameter, interpreter)));
                return 1;
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
            PushLocalReferenceInstruction(const std::string& value) : mParameter(value)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                stack.push_back(std::shared_ptr<StoredValue>(new StoredLocalReferenceValue(mParameter, interpreter)));
                return 1;
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
     *  @brief Push a reference to a named global variable. The parameter provided here
     *  should be excluding the '$' prefix.
     */
    class PushGlobalReferenceInstruction : public Instruction
    {
        public:
            PushGlobalReferenceInstruction(const std::string& value) : mParameter(value)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                stack.push_back(std::shared_ptr<StoredValue>(new StoredGlobalReferenceValue(mParameter, interpreter)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushGlobalReference " << mParameter;
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
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 2);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> rhsStored = stack.back();
                stack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = stack.back();
                stack.pop_back();

                std::shared_ptr<StoredLocalReferenceValue> localReference = std::dynamic_pointer_cast<StoredLocalReferenceValue>(lhsStored);
                std::shared_ptr<StoredGlobalReferenceValue> globalReference = std::dynamic_pointer_cast<StoredGlobalReferenceValue>(lhsStored);

                assert(localReference || globalReference);

                if (localReference)
                {
                    localReference->setValue(rhsStored, scope);
                }
                else
                {
                    globalReference->setValue(rhsStored);
                }

                // In Torque, the result of the assignment is pushed to stack
                stack.push_back(rhsStored);
                return 1;
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
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 2);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> rhsStored = stack.back();
                stack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = stack.back();
                stack.pop_back();

                std::string lhs = lhsStored->toString(scope);
                std::string rhs = rhsStored->toString(scope);

                stack.push_back(std::shared_ptr<StoredValue>(new StoredStringValue(lhs + rhs, interpreter)));
                return 1;
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
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 1);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> storedTarget = stack.back();
                stack.pop_back();

                stack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(-storedTarget->toFloat(scope), interpreter)));
                return 1;
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
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 1);

                // Pop name from the stack
                std::shared_ptr<StoredValue> calledFunctionParameter = stack.back();
                stack.pop_back();

                // Ensure we have a string value here - it should be impossible to get anything else as a call name
                // assert(calledFunctionParameter->getVariableType() == StoredValue::VariableType::STRING);

                const std::string calledFunctionName = calledFunctionParameter->toString(scope);

                std::shared_ptr<Function> functionLookup = interpreter->getFunction(calledFunctionName);
                if (functionLookup)
                {
                    scope->push();
                    functionLookup->execute(interpreter, scope, stack);
                    scope->pop();
                }
                else
                {
                    // FIXME: Virtual logging methods?
                    std::cerr << "Could not find function '" << calledFunctionName << "' for calling! Placing 0 on the stack." << std::endl;
                    stack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0, interpreter)));
                }
                return 1;
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
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 2);

                std::shared_ptr<StoredValue> rhsStored = stack.back();
                stack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored->toFloat(scope);
                float rhs = rhsStored->toFloat(scope);

                const float result = lhs + rhs;
                stack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(result, interpreter)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Add";
            }
    };

    /**
     *  @brief Performs a bitwise AND against two values.
     */
    class BitwiseAndInstruction : public Instruction
    {
        public:
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 2);

                std::shared_ptr<StoredValue> rhsStored = stack.back();
                stack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                int lhs = lhsStored->toInteger(scope);
                int rhs = rhsStored->toInteger(scope);

                const int result = lhs & rhs;
                stack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(result, interpreter)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "BitwiseAnd";
            }
    };

    /**
     *  @brief Adds together two values on the stack and pushes the sum.
     */
    class MultiplyInstruction : public Instruction
    {
        public:
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 2);

                std::shared_ptr<StoredValue> lhsStored = stack.back();
                stack.pop_back();
                std::shared_ptr<StoredValue> rhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats

                float lhs = lhsStored->toFloat(scope);
                float rhs = rhsStored->toFloat(scope);

                const float result = lhs * rhs;
                stack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(result, interpreter)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Multiply";
            }
    };

    /**
     *  @brief Pops a value from the stack, discarding it.
     */
    class PopInstruction : public Instruction
    {
        public:
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 1);
                stack.pop_back();

                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Pop";
            }
    };

    class JumpInstruction : public Instruction
    {
        public:
            JumpInstruction(const int offset) : mOffset(offset)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                return mOffset;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "Jump " << mOffset;
                return out.str();
            }

        private:
            //! The unconditional jump offset.
            int mOffset;
    };

    class JumpTrueInstruction : public Instruction
    {
        public:
            JumpTrueInstruction(const int offset) : mOffset(offset)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 1);

                std::shared_ptr<StoredValue> booleanStored = stack.back();
                stack.pop_back();

                if (booleanStored->toBoolean(scope))
                {
                    return mOffset;
                }
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "JumpTrue " << mOffset;
                return out.str();
            }

        private:
            //! The jump offset.
            int mOffset;
    };

    class JumpFalseInstruction : public Instruction
    {
        public:
            JumpFalseInstruction(const int offset) : mOffset(offset)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                assert(stack.size() >= 1);

                std::shared_ptr<StoredValue> booleanStored = stack.back();
                stack.pop_back();

                if (!booleanStored->toBoolean(scope))
                {
                    return mOffset;
                }
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "JumpFalse " << mOffset;
                return out.str();
            }

        private:
            //! The jump offset.
            int mOffset;
    };

    class NOPInstruction : public Instruction
    {
        public:
            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "NOP";
            }
    };

    class FunctionDeclarationInstruction : public Instruction
    {
        public:
            FunctionDeclarationInstruction(const std::string& name, const std::vector<std::shared_ptr<Instruction>>& instructions) : mName(name), mInstructions(instructions)
            {

            }

            virtual int execute(Interpreter* interpreter, ExecutionScope* scope, StoredValueStack& stack) override
            {
                // Register the function
                std::shared_ptr<Function> newFunction = std::shared_ptr<Function>(new Function(mName));
                newFunction->addInstructions(mInstructions);
                interpreter->addFunction(newFunction);

                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "FunctionDeclaration " << mName << std::endl;

                for (auto&& instruction : mInstructions)
                {
                    out << "    " << instruction->disassemble() << std::endl;
                }
                return out.str();
            }

        private:
            std::string mName;
            std::vector<std::shared_ptr<Instruction>> mInstructions;
    };
}
