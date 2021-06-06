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
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(mParameter));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(mParameter));
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
            PushStringInstruction(const std::size_t value) : mStringID(value)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(mStringID, StoredValueType::String));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushString " << mStringID;
                return out.str();
            }

        private:
            //! The string table ID of the parameter to push.
            std::size_t mStringID;
    };

    /**
     *  @brief Push a reference to a named local variable. The parameter provided here
     *  should be excluding the '%' prefix.
     */
    class PushLocalReferenceInstruction : public Instruction
    {
        public:
            PushLocalReferenceInstruction(const std::size_t value) : mStringID(value)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(mStringID, StoredValueType::LocalReference));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushLocalReference " << mStringID;
                return out.str();
            }

        private:
            //! The value to push.
            std::size_t mStringID;
    };

    /**
     *  @brief Push a reference to a named global variable. The parameter provided here
     *  should be excluding the '$' prefix.
     */
    class PushGlobalReferenceInstruction : public Instruction
    {
        public:
            PushGlobalReferenceInstruction(const std::size_t value) : mStringID(value)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(mStringID, StoredValueType::GlobalReference));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "PushGlobalReference " << mStringID;
                return out.str();
            }

        private:
            //! The value to push.
            std::size_t mStringID;
    };

    /**
     *  @brief Performs an addition of two values on the stack and assigns the result
     */
    class AddAssignmentInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                // Pull two values off the stack
                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                float resultRaw = 0.0f;
                resultRaw = lhsStored.toFloat(state);
                resultRaw += rhsStored.toFloat(state);

                StoredValue result = StoredValue(resultRaw);
                if (!lhsStored.setValue(result, state))
                {
                    state->mInterpreter->logError("Attempted to perform no-op assignment!");
                }

                // In Torque, the result of the assignment is pushed to stack
                stack.push_back(result);
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                // Pull two values off the stack
                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                if (!lhsStored.setValue(rhsStored, state))
                {
                    state->mInterpreter->logError("Attempted to perform no-op assignment!");
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
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                // Pull two values off the stack
                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                std::string lhs = lhsStored.toString(state);
                std::string rhs = rhsStored.toString(state);

                // Generate a new string ID
                const std::size_t requestedStringID = state->mInterpreter->mStringTable.getOrAssign(lhs + rhs);
                stack.push_back(StoredValue(requestedStringID, StoredValueType::String));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                // Pull two values off the stack
                StoredValue storedTarget = stack.back();
                stack.pop_back();

                stack.push_back(StoredValue(-storedTarget.toFloat(state)));
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
                const std::string namespaceName = toLowerCase(mNameSpace);
                StoredValueStack& stack = state->mExecutionScope.getStack();

                // If we're calling a parent function, perform an alternative lookup
                if (namespaceName == "parent")
                {
                    Function* currentFunction = state->mExecutionScope.getCurrentFunction();
                    if (currentFunction == nullptr)
                    {
                        state->mInterpreter->logError("Attempted to call parent:: function at root!");
                        stack.push_back(StoredValue(0));
                        return 1;
                    }

                    // Once we have a valid function pointer, ask the interpreter to find a super function higher up the chain
                    std::shared_ptr<Function> parentFunction = state->mInterpreter->getFunctionParent(currentFunction);
                    if (!parentFunction)
                    {
                        std::ostringstream stream;

                        stream << "Could not find parent function '" << mName << "' for calling! Placing 0 on the stack.";
                        state->mInterpreter->logError(stream.str());

                        stack.push_back(StoredValue(0));
                        return 1;
                    }

                    // Otherwise, call it
                    parentFunction->execute(nullptr, state, mArgc);

                    return 1;
                }

                std::shared_ptr<Function> functionLookup = state->mInterpreter->getFunction(mNameSpace, mName);
                if (functionLookup)
                {
                    functionLookup->execute(nullptr, state, mArgc);
                }
                else
                {
                    std::ostringstream stream;

                    stream << "Could not find function '" << mName << "' for calling! Placing 0 on the stack.";
                    state->mInterpreter->logError(stream.str());

                    stack.push_back(StoredValue(0));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat(state);
                float rhs = rhsStored.toFloat(state);

                const float result = lhs + rhs;
                stack.push_back(StoredValue(result));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat(state);
                float rhs = rhsStored.toFloat(state);

                const int result = lhs < rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat(state);
                float rhs = rhsStored.toFloat(state);

                const int result = lhs == rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                int lhs = lhsStored.toInteger(state);
                int rhs = rhsStored.toInteger(state);

                const int result = lhs & rhs;
                stack.push_back(StoredValue(result));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue lhsStored = stack.back();
                stack.pop_back();
                StoredValue rhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats

                float lhs = lhsStored.toFloat(state);
                float rhs = rhsStored.toFloat(state);

                const float result = lhs * rhs;
                stack.push_back(StoredValue(result));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Multiply";
            }
    };

    class DivideInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats

                float lhs = lhsStored.toFloat(state);
                float rhs = rhsStored.toFloat(state);

                const float result = lhs / rhs;
                stack.push_back(StoredValue(result));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Divide";
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue booleanStored = stack.back();
                stack.pop_back();

                if (booleanStored.toBoolean(state))
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue booleanStored = stack.back();
                stack.pop_back();

                if (!booleanStored.toBoolean(state))
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
            FunctionDeclarationInstruction(const std::string package, const std::string& space, const std::string& name, const std::vector<std::string> parameterNames, const InstructionSequence& instructions) : mPackageName(package), mNameSpace(space), mName(name), mParameterNames(parameterNames), mInstructions(instructions)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                // Register the function
                std::shared_ptr<Function> newFunction = std::shared_ptr<Function>(new Function(mPackageName, mNameSpace, mName, mParameterNames));
                newFunction->addInstructions(mInstructions);
                state->mInterpreter->addFunction(newFunction);

                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;

                if (mNameSpace == NAMESPACE_EMPTY)
                {
                    out << "FunctionDeclaration " << mName;
                }
                else
                {
                    out << "FunctionDeclaration " << mNameSpace << "::" << mName;
                }

                if (mPackageName != PACKAGE_EMPTY)
                {
                    out << "[in Package " << mPackageName << "] ";
                }

                out << "(";

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
            std::string mPackageName;
            std::string mNameSpace;
            std::string mName;
            std::vector<std::string> mParameterNames;
            InstructionSequence mInstructions;
    };

    class SubReferenceInstruction : public Instruction
    {
        public:
            SubReferenceInstruction(const std::size_t value) : mStringID(value)
            {

            }

            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue targetStored = stack.back();
                stack.pop_back();

                std::shared_ptr<ConsoleObject> referenced = targetStored.toConsoleObject(state);
                if (referenced)
                {
                    stack.push_back(StoredValue(referenced, mStringID));
                    return 1;
                }

                stack.push_back(StoredValue(0));
                return 1;
            };

            virtual std::string disassemble() override
            {
                std::ostringstream out;
                out << "SubReference " << mStringID;
                return out.str();
            }

        private:
            std::size_t mStringID;
    };

    class ReturnInstruction : public Instruction
    {
        public:
            virtual int execute(std::shared_ptr<ExecutionState> state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue targetStored = stack.back();
                stack.pop_back();

                // For if we return a variable reference, we want to pass back a copy
                StoredValueStack& returnStack = state->mExecutionScope.getReturnStack();
                returnStack.push_back(targetStored.getReferencedValueCopy(state));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                // When we encounter this instruction, we generate a new variable reference by appending all string representations together
                // This is what T2 does - it has no concept of arrays despite pretending to

                // FIXME: With more clever use of the ostream we might be able to make this work without the vector
                std::vector<std::string> variableComponents;
                for (unsigned int iteration = 0; iteration < mArgc; ++iteration)
                {
                    variableComponents.push_back(stack.popString(state));
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

                const std::size_t stringID = state->mInterpreter->mStringTable.getOrAssign(out.str());
                if (mGlobal)
                {
                    stack.push_back(StoredValue(stringID, StoredValueType::GlobalReference));
                }
                else
                {
                    stack.push_back(StoredValue(stringID, StoredValueType::LocalReference));
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
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue targetStored = stack.back();
                stack.pop_back();

                // Retrieve the referenced ConsoleObject
                std::shared_ptr<ConsoleObject> targetSim = targetStored.toConsoleObject(state);
                if (!targetSim)
                {
                    std::ostringstream output;
                    output << "Cannot find object '" << targetStored.toString(state) << "' to call function '" << mName << "'!";
                    state->mInterpreter->logWarning(output.str());
                }

                // FIXME: For now we assume 'ConsoleObject' is the classname
                const std::string className = "ConsoleObject";

                // Ask the interpreter to lookup the function
                std::shared_ptr<Function> calledFunction = state->mInterpreter->getFunction(className, mName);
                if (calledFunction)
                {
                    calledFunction->execute(targetSim, state, mArgc);
                    return 1;
                }

                stack.push_back(StoredValue(0));
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
