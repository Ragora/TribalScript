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
    namespace Instructions
    {
        class Instruction;

        typedef AddressOffsetType (*OpcodeResolutionPointer)(ExecutionState* state, Instruction* instruction);

    #define INSTRUCTION_PARAMETER_COUNT 4
        union InstructionParameters
        {
            int mInteger;
            float mFloat;
            AddressType mOffset;
            std::size_t mStringID;
        };

        class Instruction
        {
            public:
                Instruction(OpcodeResolutionPointer op) : mOp(op)
                {

                }

                const OpcodeResolutionPointer mOp;

                InstructionParameters mParameters[INSTRUCTION_PARAMETER_COUNT];
        };

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
    }
}
