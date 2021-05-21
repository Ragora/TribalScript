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
#include <torquescript/storedfieldreferencevalue.hpp>
#include <torquescript/storedlocalreferencevalue.hpp>
#include <torquescript/storedglobalreferencevalue.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/instructionsequence.hpp>

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
             *  @param state The current execution state to act upon.
             */
            virtual int execute(std::shared_ptr<ExecutionState> state) = 0;

            /**
             *  @brief Helper routine to produce a disassembly for this instruction.
             */
            virtual std::string disassemble() = 0;

            //! Compiler generated comment, used for generating easier to follow disassembly
            std::string mComment;
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(mParameter)));
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(mParameter)));
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredStringValue(expandEscapeSequences(mParameter))));
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredLocalReferenceValue(mParameter)));
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredGlobalReferenceValue(mParameter)));
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
     *  @brief Performs an addition of two values on the stack and assigns the result
     */
    class AddAssignmentInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                float resultRaw = 0.0f;
                resultRaw = lhsStored->toFloat(state);
                resultRaw += rhsStored->toFloat(state);

                std::shared_ptr<StoredValue> result = std::shared_ptr<StoredValue>(new StoredFloatValue(resultRaw));
                if (!lhsStored->setValue(result, state))
                {
                    state->mInterpreter->logError("Attempted to perform no-op assignment!");
                }

                // In Torque, the result of the assignment is pushed to stack
                state->mStack.push_back(result);
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "AddAssignment";
            }
    };

    /**
     *  @brief Assign to lhs with whatever is on rhs.
     */
    class AssignmentInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                if (!lhsStored->setValue(rhsStored, state))
                {
                    state->mInterpreter->logError("Attempted to perform no-op assignment!");
                }

                // In Torque, the result of the assignment is pushed to stack
                state->mStack.push_back(rhsStored);
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
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                std::string lhs = lhsStored->toString(state);
                std::string rhs = rhsStored->toString(state);

                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredStringValue(lhs + rhs)));
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
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);

                // Pull two values off the stack
                std::shared_ptr<StoredValue> storedTarget = state->mStack.back();
                state->mStack.pop_back();

                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(-storedTarget->toFloat(state))));
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
            CallFunctionInstruction(const std::string& space, const std::string& name, const unsigned int argc) : mNameSpace(space), mName(name), mArgc(argc)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                std::shared_ptr<Function> functionLookup = state->mInterpreter->getFunction(mNameSpace, mName);
                if (functionLookup)
                {
                    functionLookup->execute(state, mArgc);
                }
                else
                {
                    std::ostringstream stream;

                    stream << "Could not find function '" << mName << "' for calling! Placing 0 on the stack.";
                    state->mInterpreter->logError(stream.str());

                    state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
                }
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;

                if (mNameSpace == "")
                {
                    out << "CallFunction " << mName << " argc=" << mArgc;
                }
                else
                {
                    out << "CallFunction " << mNameSpace << "::" << mName << " argc=" << mArgc;
                }
                return out.str();
            }

            private:
                //! The namespace of the function to call.
                std::string mNameSpace;

                //! The name of the function to call.
                std::string mName;

                //! How many arguments are being passed to the function to call.
                unsigned int mArgc;
    };

    /**
     *  @brief Adds together two values on the stack and pushes the sum.
     */
    class AddInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored->toFloat(state);
                float rhs = rhsStored->toFloat(state);

                const float result = lhs + rhs;
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(result)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Add";
            }
    };

    /**
     *  @brief Compares two values on the stack using a less than relation.
     */
    class LessThanInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored->toFloat(state);
                float rhs = rhsStored->toFloat(state);

                const int result = lhs < rhs ? 1 : 0;
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(result)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "LessThan";
            }
    };

    /**
     *  @brief Compares two values on the stack using an equality.
     */
    class EqualsInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored->toFloat(state);
                float rhs = rhsStored->toFloat(state);

                const int result = lhs == rhs ? 1 : 0;
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(result)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Equals";
            }
    };

    /**
     *  @brief Performs a bitwise AND against two values.
     */
    class BitwiseAndInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();

                // NOTE: For now we normalize to floats
                int lhs = lhsStored->toInteger(state);
                int rhs = rhsStored->toInteger(state);

                const int result = lhs & rhs;
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(result)));
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
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 2);

                std::shared_ptr<StoredValue> lhsStored = state->mStack.back();
                state->mStack.pop_back();
                std::shared_ptr<StoredValue> rhsStored = state->mStack.back();
                state->mStack.pop_back();

                // NOTE: For now we normalize to floats

                float lhs = lhsStored->toFloat(state);
                float rhs = rhsStored->toFloat(state);

                const float result = lhs * rhs;
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredFloatValue(result)));
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
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);
                state->mStack.pop_back();

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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);

                std::shared_ptr<StoredValue> booleanStored = state->mStack.back();
                state->mStack.pop_back();

                if (booleanStored->toBoolean(state))
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

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);

                std::shared_ptr<StoredValue> booleanStored = state->mStack.back();
                state->mStack.pop_back();

                if (!booleanStored->toBoolean(state))
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
            virtual int execute(std::shared_ptr<ExecutionState> state) override
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
            FunctionDeclarationInstruction(const std::string& space, const std::string& name, const std::vector<std::string> parameterNames, const InstructionSequence& instructions) : mNameSpace(space), mName(name), mParameterNames(parameterNames), mInstructions(instructions)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                // Register the function
                std::shared_ptr<Function> newFunction = std::shared_ptr<Function>(new Function(mNameSpace, mName, mParameterNames));
                newFunction->addInstructions(mInstructions);
                state->mInterpreter->addFunction(newFunction, "");

                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;

                if (mNameSpace == "")
                {
                    out << "FunctionDeclaration " << mName << "(";
                }
                else
                {
                    out << "FunctionDeclaration " << mNameSpace << "::" << mName << "(";
                }

                // Generate parameter list
                for (auto iterator = mParameterNames.begin(); iterator != mParameterNames.end(); ++iterator)
                {
                    if (iterator != mParameterNames.begin())
                    {
                        out << ", ";
                    }

                    out << *iterator;
                }

                // Output all instructions
                out << ")" << std::endl;
                for (auto&& instruction : mInstructions)
                {
                    out << "    " << instruction->disassemble();
                    if (instruction->mComment != "")
                    {
                        out << " // " << instruction->mComment;
                    }
                    out << std::endl;
                }
                return out.str();
            }

        private:
            std::string mNameSpace;
            std::string mName;
            std::vector<std::string> mParameterNames;
            InstructionSequence mInstructions;
    };

    class SubReferenceInstruction : public Instruction
    {
        public:
            SubReferenceInstruction(const std::string& name) : mName(name)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);

                std::shared_ptr<StoredValue> targetStored = state->mStack.back();
                state->mStack.pop_back();

                std::shared_ptr<SimObject> referenced = targetStored->toSimObject(state);
                if (referenced)
                {
                    state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredFieldReferenceValue(referenced, mName)));
                    return 1;
                }

                // If lookup failed, return empty string
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredStringValue("")));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "SubReference " << mName;
                return out.str();
            }

        private:
            std::string mName;
    };

    class ReturnInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);

                std::shared_ptr<StoredValue> targetStored = state->mStack.back();
                state->mStack.pop_back();

                // For if we return a variable reference, we want to pass back a copy
                state->mStack.push_back(std::shared_ptr<StoredValue>(targetStored->getReferencedValueCopy(state)));
                return 0;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "Return";
                return out.str();
            }
    };

    class PushLoopInstruction : public Instruction
    {
        public:
            PushLoopInstruction(const unsigned int loopSize) : mLoopSize(loopSize)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                const unsigned int loopPosition = state->mInstructionPointer;
                state->mExecutionScope.pushLoop(loopPosition, mLoopSize);
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushLoop " << mLoopSize;
                return out.str();
            }

        private:
            unsigned int mLoopSize;
    };

    class PopLoopInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                state->mExecutionScope.popLoop();
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "PopLoop";
            }
    };

    class BreakInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                if (state->mExecutionScope.isLoopStackEmpty())
                {
                    state->mInterpreter->logWarning("Break outside of loop, ignoring ...");
                    return 1;
                }

                LoopDescriptor descriptor = state->mExecutionScope.currentLoopDescriptor();
                // This should lead to a PopLoop instruction so we don't pop here
                const unsigned int loopProgress = state->mInstructionPointer - descriptor.mInstructionPointer;
                return descriptor.mLoopSize - loopProgress;
            };

            virtual std::string disassemble() override
            {
                return "Break";
            }
    };

    /**
     *  @brief Calls a function registered in memory somewhere.
     */
    class AccessArrayInstruction : public Instruction
    {
        public:
            AccessArrayInstruction(const std::string& name, const unsigned int argc, bool global) : mName(name), mArgc(argc), mGlobal(global)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                // When we encounter this instruction, we generate a new variable reference by appending all string representations together
                // This is what T2 does - it has no concept of arrays despite pretending to

                // FIXME: With more clever use of the ostream we might be able to make this work without the vector
                std::vector<std::string> variableComponents;
                for (unsigned int iteration = 0; iteration < mArgc; ++iteration)
                {
                    variableComponents.push_back(state->mStack.popString(state));
                }

                std::ostringstream out;
                out << mName;
                for (auto iterator = variableComponents.rbegin(); iterator != variableComponents.rend(); ++iterator)
                {
                    if (iterator != variableComponents.rbegin())
                    {
                        out << "_";
                    }
                    out << *iterator;
                }

                if (mGlobal)
                {
                    state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredGlobalReferenceValue(out.str())));
                }
                else
                {
                    state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredLocalReferenceValue(out.str())));
                }
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "AccessArray " << mName << " argc=" << mArgc << " global=" << mGlobal;
                return out.str();
            }

            private:
                std::string mName;
                unsigned int mArgc;
                bool mGlobal;
    };

    /**
     *  @brief Calls a function registered in memory somewhere.
     */
    class CallBoundFunctionInstruction : public Instruction
    {
        public:
            CallBoundFunctionInstruction(const std::string& name, const unsigned int argc) : mName(name), mArgc(argc)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                assert(state->mStack.size() >= 1);

                std::shared_ptr<StoredValue> targetStored = state->mStack.back();
                state->mStack.pop_back();

                // Retrieve the referenced SimObject
                std::shared_ptr<SimObject> targetSim = targetStored->toSimObject(state);
                if (!targetSim)
                {
                    std::ostringstream output;
                    output << "Cannot find object '" << targetStored->toString(state) << "' to call function '" << mName << "'!";
                    state->mInterpreter->logWarning(output.str());
                }

                // TODO: Implement calling for when this lookup does succeed
                state->mStack.push_back(std::shared_ptr<StoredValue>(new StoredIntegerValue(0)));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "CallBoundFunctionInstruction " << mName << " argc=" << mArgc;
                return out.str();
            }

            private:
                std::string mName;
                unsigned int mArgc;
    };
}
