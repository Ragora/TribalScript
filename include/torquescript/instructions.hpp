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

#include <torquescript/instruction.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/function.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/storedvalue.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
{
    namespace Instructions
    {
        /**
         *  @brief Push integer instruction. This will push an integer value to the system stack
         *  for later use in execution.
         */
        class PushIntegerInstruction : public Instruction
        {
        public:
            PushIntegerInstruction(const int value) : Instruction(PushIntegerInstruction::execute)
            {
                mParameters[0].mInteger = value;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(instruction->mParameters[0].mInteger));
                return 1;
            }
        };

        /**
         *  @brief Push string instruction. This will push a string value to the system stack for later
         *  use in execution.
         */
        class PushStringInstruction : public Instruction
        {
            public:
                PushStringInstruction(const StringTableEntry value) : Instruction(PushStringInstruction::execute)
                {
                    mParameters[0].mStringID = value;
                }

                static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValue(state->mInterpreter->mStringTable.getString(instruction->mParameters[0].mStringID).c_str()));
                    return 1;
                };
        };

        /**
         *  @brief Calls a function registered within the current interpreter.
         */
        class CallFunctionInstruction : public Instruction
        {
            public:
                /**
                 *  @brief Constructs a new CallFunctionInstruction instance.
                 *  @param space The namespace of the function to call.
                 *  @param name The name of the function to call.
                 *  @param argc The total number of arguments to pull off the stack for use as parameters.
                 */
                CallFunctionInstruction(const StringTableEntry space, const StringTableEntry name, const std::size_t argc) : Instruction(CallFunctionInstruction::execute)
                {
                    mParameters[0].mStringID = space;
                    mParameters[1].mStringID = name;
                    mParameters[2].mStringID = argc;
                }

                static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
                {
                    const std::string namespaceName = state->mInterpreter->mStringTable.getString(instruction->mParameters[0].mStringID);
                    const std::string functionName = state->mInterpreter->mStringTable.getString(instruction->mParameters[1].mStringID);

                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    // If we're calling a parent function, perform an alternative lookup
                    if (namespaceName == "parent")
                    {
                        Function* currentFunction = state->mExecutionScope.getCurrentFunction();
                        if (currentFunction == nullptr)
                        {
                            state->mInterpreter->mConfig.mPlatform->logError("Attempted to call parent:: function at root!");
                            stack.push_back(StoredValue(0));
                            return 1;
                        }

                        // Once we have a valid function pointer, ask the interpreter to find a super function higher up the chain
                        std::shared_ptr<Function> parentFunction = state->mInterpreter->getFunctionParent(currentFunction);
                        if (!parentFunction)
                        {
                            std::ostringstream stream;

                            stream << "Could not find parent function '" << functionName << "' for calling! Placing 0 on the stack.";
                            state->mInterpreter->mConfig.mPlatform->logError(stream.str());

                            stack.push_back(StoredValue(0));
                            return 1;
                        }

                        // Otherwise, call it
                        parentFunction->execute(nullptr, state, instruction->mParameters[2].mStringID);

                        return 1;
                    }

                    std::shared_ptr<Function> functionLookup = state->mInterpreter->getFunction(namespaceName, functionName);
                    if (functionLookup)
                    {
                        functionLookup->execute(nullptr, state, instruction->mParameters[2].mStringID);
                    }
                    else
                    {
                        std::ostringstream stream;

                        stream << "Could not find function '" << functionName << "' for calling! Placing 0 on the stack.";
                        state->mInterpreter->mConfig.mPlatform->logError(stream.str());

                        stack.push_back(StoredValue(0));
                    }
                    return 1;
                }
        };

        /**
         *  @brief Push float instruction. This will push a floating point value to the system stack
         *  for later use in execution.
         */
        class PushFloatInstruction : public Instruction
        {
        public:
            PushFloatInstruction(const float value) : Instruction(PushFloatInstruction::execute)
            {
                mParameters[0].mFloat = value;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(instruction->mParameters[0].mFloat));
                return 1;
            }
        };

        /**
         *  @brief Negate a value on the stack.
         */
        class NegateInstruction : public Instruction
        {
        public:
            NegateInstruction() : Instruction(NegateInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                // Pull two values off the stack
                StoredValue storedTarget = stack.back();
                stack.pop_back();

                stack.push_back(StoredValue(-storedTarget.toFloat()));
                return 1;
            }
        };

        /**
         *  @brief Compares two values on the stack using a less than relation.
         */
        class LessThanInstruction : public Instruction
        {
        public:
            LessThanInstruction() : Instruction(LessThanInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const int result = lhs < rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Invert the truthfulness of a value on the stack.
         */
        class NotInstruction : public Instruction
        {
        public:
            NotInstruction() : Instruction(NotInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                // Pull two values off the stack
                StoredValue storedTarget = stack.back();
                stack.pop_back();

                stack.push_back(StoredValue(!storedTarget.toBoolean() ? 1 : 0));
                return 1;
            }
        };


        /**
         *  @brief Performs an addition of two values on the stack and assigns the result.
         */
        class AddAssignmentInstruction : public Instruction
        {
        public:
            AddAssignmentInstruction() : Instruction(AddAssignmentInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                // Pull two values off the stack
                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                float resultRaw = 0.0f;
                resultRaw = lhsStored.toFloat();
                resultRaw += rhsStored.toFloat();

                StoredValue result = StoredValue(resultRaw);
                if (!lhsStored.setValue(result))
                {
                    state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                }

                // In Torque, the result of the assignment is pushed to stack
                stack.push_back(result);
                return 1;
            }
        };

        /**
         *  @brief Jumps to the specified instruction offset if a condition is false.
         *  The condition checked is the value at the top of the stack coerced to a
         *  boolean. If the condition is true, this instruction only pops the stack.
         */
        class JumpFalseInstruction : public Instruction
        {
        public:
            /**
             *  @brief Constructs a new instance of JumpFalseInstruction.
             *  @param offset The instruction offset to jump to if the condition is false.
             */
            JumpFalseInstruction(const AddressType offset) : Instruction(JumpFalseInstruction::execute)
            {
                mParameters[0].mOffset = offset;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue booleanStored = stack.back();
                stack.pop_back();

                if (!booleanStored.toBoolean())
                {
                    return instruction->mParameters[0].mOffset;
                }
                return 1;
            }
        };

        /**
         *  @brief Jumps to the specified instruction offset if a condition is true.
         *  The condition checked is the value at the top of the stack coerced to a
         *  boolean. If the condition is false, this instruction only pops the stack.
         */
        class JumpTrueInstruction : public Instruction
        {
        public:
            /**
             *  @brief Constructs a new instance of JumpTrueInstruction.
             *  @param offset The instruction offset to jump to if the condition is true.
             */
            JumpTrueInstruction(const AddressType offset) : Instruction(JumpTrueInstruction::execute)
            {
                mParameters[0].mOffset = offset;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue booleanStored = stack.back();
                stack.pop_back();

                if (booleanStored.toBoolean())
                {
                    return instruction->mParameters[0].mOffset;
                }
                return 1;
            }
        };

        /**
         *  @brief Unconditionally jumps to the specified instruction offset.
         */
        class JumpInstruction : public Instruction
        {
        public:
            /**
             *  @brief Constructs a new JumpInstruction instance.
             *  @param offset The instruction offset to unconditionally jump to.
             */
            JumpInstruction(const AddressType offset) : Instruction(JumpInstruction::execute)
            {
                mParameters[0].mOffset = offset;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                return instruction->mParameters[0].mOffset;
            }
        };

        /**
         *  @brief Instruction that does nothing. It usually is used to pad jumps in order
         *  to provide safe jump targets within the current program space.
         */
        class NOPInstruction : public Instruction
        {
        public:
            NOPInstruction() : Instruction(NOPInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                return 1;
            }
        };

        /**
         *  @brief Performs a logical or of two values on the stack and pushes back the result.
         */
        class LogicalOrInstruction : public Instruction
        {
        public:
            LogicalOrInstruction() : Instruction(LogicalOrInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                const bool lhs = lhsStored.toBoolean();
                const bool rhs = rhsStored.toBoolean();

                const int result = lhs || rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        class LogicalAndInstruction : public Instruction
        {
        public:
            LogicalAndInstruction() : Instruction(LogicalAndInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                const bool lhs = lhsStored.toBoolean();
                const bool rhs = rhsStored.toBoolean();

                const int result = lhs && rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Adds together two values on the stack and pushes the sum.
         */
        class AddInstruction : public Instruction
        {
        public:
            AddInstruction() : Instruction(AddInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const float result = lhs + rhs;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        class DivideInstruction : public Instruction
        {
        public:
            DivideInstruction() : Instruction(DivideInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const float result = lhs / rhs;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        class MultiplyInstruction : public Instruction
        {
        public:
            MultiplyInstruction() : Instruction(MultiplyInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const float result = lhs * rhs;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        class MinusInstruction : public Instruction
        {
        public:
            MinusInstruction() : Instruction(MinusInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const float result = lhs - rhs;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Adds together two values on the stack and pushes the sum.
         */
        class ModulusInstruction : public Instruction
        {
        public:
            ModulusInstruction() : Instruction(ModulusInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                int lhs = lhsStored.toInteger();
                int rhs = rhsStored.toInteger();

                const int result = lhs % rhs;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Push a reference to a named local variable. The parameter provided here
         *  should be excluding the '%' prefix.
         */
        class PushLocalReferenceInstruction : public Instruction
        {
        public:
            PushLocalReferenceInstruction(const StringTableEntry value) : Instruction(PushLocalReferenceInstruction::execute)
            {
                mParameters[0].mStringID = value;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.emplace_back(state->mExecutionScope.getVariableOrAllocate(instruction->mParameters[0].mStringID));
                return 1;
            };
        };

        /**
         *  @brief Push a reference to a named global variable. The parameter provided here
         *  should be excluding the '$' prefix.
         */
        class PushGlobalReferenceInstruction : public Instruction
        {
        public:
            PushGlobalReferenceInstruction(const StringTableEntry value) : Instruction(PushGlobalReferenceInstruction::execute)
            {
                mParameters[0].mStringID = value;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();
                stack.push_back(StoredValue(state->mInterpreter->getGlobalOrAllocate(instruction->mParameters[0].mStringID)));
                return 1;
            };
        };

        /**
         *  @brief Compares two values on the stack using an equality.
         */
        class EqualsInstruction : public Instruction
        {
        public:
            EqualsInstruction() : Instruction(EqualsInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const int result = lhs == rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Compares two values on the stack using an equality.
         */
        class NotEqualsInstruction : public Instruction
        {
        public:
            NotEqualsInstruction() : Instruction(NotEqualsInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const int result = lhs != rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Ends execution in the current function immediately. It will take one
         *  value from the top of the stack and push it to the parent stack before returning.
         */
        class ReturnInstruction : public Instruction
        {
        public:
            ReturnInstruction() : Instruction(ReturnInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                StoredValue targetStored = stack.back();
                stack.pop_back();

                // For if we return a variable reference, we want to pass back a copy
                StoredValueStack& returnStack = state->mExecutionScope.getReturnStack();
                returnStack.push_back(targetStored.getReferencedValueCopy(state));
                return 0;
            }
        };

        /**
         *  @brief Used to keep track of loops in the virtual instructions. This instruction
         *  marks the start of a loop control construct.
         */
        class PushLoopInstruction : public Instruction
        {
        public:
            PushLoopInstruction(const std::size_t loopSize) : Instruction(PushLoopInstruction::execute)
            {
                mParameters[0].mStringID = loopSize;
            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                const AddressType loopPosition = state->mInstructionPointer;
                state->mExecutionScope.pushLoop(loopPosition, instruction->mParameters[0].mStringID);
                return 1;
            }
        };

        /**
         *  @brief Used to keep track of loops in the virtual instructions. This instruction
         *  marks the end of a loop control construct.
         */
        class PopLoopInstruction : public Instruction
        {
        public:
            PopLoopInstruction() : Instruction(PopLoopInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                state->mExecutionScope.popLoop();
                return 1;
            }
        };

        /**
         *  @brief Pops a value from the stack, discarding it.
         */
        class PopInstruction : public Instruction
        {
        public:
            PopInstruction() : Instruction(PopInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);
                stack.pop_back();

                return 1;
            }
        };

        /**
         *  @brief Assign to lhs with whatever is on rhs.
         */
        class AssignmentInstruction : public Instruction
        {
        public:
            AssignmentInstruction() : Instruction(AssignmentInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                // Pull two values off the stack
                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                if (!lhsStored.setValue(rhsStored))
                {
                    state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                }

                // In Torque, the result of the assignment is pushed to stack
                stack.push_back(rhsStored);
                return 1;
            }
        };

        /**
         *  @brief Compares two values on the stack using an equality.
         */
        class StringEqualsInstruction : public Instruction
        {
        public:
            StringEqualsInstruction() : Instruction(StringEqualsInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                // NOTE: For now we normalize to floats
                std::string lhs = lhsStored.toString();
                std::string rhs = rhsStored.toString();

                const int result = lhs == rhs ? 1 : 0;
                stack.push_back(StoredValue(result));
                return 1;
            }
        };

        /**
         *  @brief Concatenates two values at the top of the stack and pushes back the
         *  result.
         */
        class ConcatInstruction : public Instruction
        {
        public:
            ConcatInstruction() : Instruction(ConcatInstruction::execute)
            {

            }

            static AddressOffsetType execute(ExecutionState* state, Instruction* instruction)
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                // Pull two values off the stack
                StoredValue rhsStored = stack.back();
                stack.pop_back();
                StoredValue lhsStored = stack.back();
                stack.pop_back();

                std::string lhs = lhsStored.toString();
                std::string rhs = rhsStored.toString();

                // Generate a new string ID
                const std::string requestedString = lhs + rhs;
                stack.emplace_back(requestedString.c_str());
                return 1;
            }
        };

    }
}
