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

        typedef AddressOffsetType (*OpcodeFunctionPointer)(Instruction* instruction, ExecutionState* state);

        #define OP_PARAM_COUNT 4

        /**
         *  @brief Base instruction class. All instructions in the interpreter should dervive
         *  from this class and implement all virtual members.
         */
        class Instruction
        {
            public:
                Instruction(OpcodeFunctionPointer function) : mFunction(function)
                {

                }

                union InstructionParameter
                {
                    StringTableEntry mStringTableEntry;
                    AddressOffsetType mAddressOffset;
                    std::size_t mSize;
                    int mInt;
                    float mFloat;
                };

                std::string disassemble()
                {
                    return "<Not Implemented>";
                }

                std::string mComment;

                OpcodeFunctionPointer mFunction;
                InstructionParameter mParameters[OP_PARAM_COUNT];
        };

        class NOPInstruction : public Instruction
        {
            public:
                NOPInstruction() : Instruction(NOPInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    return 1;
                }
        };

        class PushIntegerInstruction : public Instruction
        {
            public:
                PushIntegerInstruction(const int value) : Instruction(PushIntegerInstruction::execute)
                {
                    mParameters[0].mInt = value;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValue(instruction->mParameters[0].mInt));
                    return 1;
                }
        };

        class CallFunctionInstruction : public Instruction
        {
            public:
                CallFunctionInstruction(const StringTableEntry namespaceEntry, const StringTableEntry functionEntry, const std::size_t parameterCount) : Instruction(CallFunctionInstruction::execute)
                {
                    mParameters[0].mStringTableEntry = namespaceEntry;
                    mParameters[1].mStringTableEntry = functionEntry;
                    mParameters[2].mSize = parameterCount;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    const std::string namespaceName = toLowerCase(state->mInterpreter->mStringTable.getString(instruction->mParameters[0].mStringTableEntry));
                    const std::string functionName = state->mInterpreter->mStringTable.getString(instruction->mParameters[1].mStringTableEntry);

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
                        parentFunction->execute(nullptr, state, instruction->mParameters[2].mSize);

                        return 1;
                    }

                    std::shared_ptr<Function> functionLookup = state->mInterpreter->getFunction(namespaceName, functionName);
                    if (functionLookup)
                    {
                        functionLookup->execute(nullptr, state, instruction->mParameters[2].mSize);
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

        class JumpFalseInstruction : public Instruction
        {
            public:
                JumpFalseInstruction(const AddressOffsetType offset) : Instruction(JumpFalseInstruction::execute)
                {
                    mParameters[0].mAddressOffset = offset;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValue booleanStored = stack.back();
                    stack.pop_back();

                    if (!booleanStored.toBoolean(state))
                    {
                        return instruction->mParameters[0].mAddressOffset;
                    }
                    return 1;
                }
        };

        class JumpTrueInstruction : public Instruction
        {
            public:
                JumpTrueInstruction(const AddressOffsetType offset) : Instruction(JumpTrueInstruction::execute)
                {
                    mParameters[0].mAddressOffset = offset;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValue booleanStored = stack.back();
                    stack.pop_back();

                    if (booleanStored.toBoolean(state))
                    {
                        return instruction->mParameters[0].mAddressOffset;
                    }
                    return 1;
                }
        };

        class JumpInstruction : public Instruction
        {
            public:
                JumpInstruction(const AddressOffsetType offset) : Instruction(JumpInstruction::execute)
                {
                    mParameters[0].mAddressOffset = offset;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    return instruction->mParameters[0].mAddressOffset;
                }
        };

        class PushFloatInstruction : public Instruction
        {
            public:
                PushFloatInstruction(const float value) : Instruction(PushFloatInstruction::execute)
                {
                    mParameters[0].mFloat = value;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValue(instruction->mParameters[0].mFloat));
                    return 1;
                }
        };

        class PushLoopInstruction : public Instruction
        {
            public:
                PushLoopInstruction(const std::size_t value) : Instruction(PushLoopInstruction::execute)
                {
                    mParameters[0].mSize = value;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    const AddressType loopPosition = state->mInstructionPointer;
                    state->mExecutionScope.pushLoop(loopPosition, instruction->mParameters[0].mSize);
                    return 1;
                }
        };

        class PopLoopInstruction : public Instruction
        {
            public:
                PopLoopInstruction() : Instruction(PopLoopInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    state->mExecutionScope.popLoop();
                    return 1;
                }
        };

        class PopInstruction : public Instruction
        {
            public:
                PopInstruction() : Instruction(PopInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);
                    stack.pop_back();

                    return 1;
                }
        };

        class EqualsInstruction : public Instruction
        {
            public:
                EqualsInstruction() : Instruction(EqualsInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
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
                }
        };

        class PushLocalReferenceInstruction : public Instruction
        {
            public:
                PushLocalReferenceInstruction(const StringTableEntry value) : Instruction(PushLocalReferenceInstruction::execute)
                {
                    mParameters[0].mStringTableEntry = value;
                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValue(instruction->mParameters[0].mStringTableEntry, StoredValueType::LocalReference));
                    return 1;
                }
        };

        class AddInstruction : public Instruction
        {
            public:
                AddInstruction() : Instruction(AddInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
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
                }
        };

        class LessThanInstruction : public Instruction
        {
            public:
                LessThanInstruction() : Instruction(LessThanInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
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
                }
        };

        class AssignmentInstruction : public Instruction
        {
            public:
                AssignmentInstruction() : Instruction(AssignmentInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
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
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                    }

                    // In Torque, the result of the assignment is pushed to stack
                    stack.push_back(result);
                    return 1;
                }
        };

        class AddAssignmentInstruction : public Instruction
        {
            public:
                AddAssignmentInstruction() : Instruction(AddAssignmentInstruction::execute)
                {

                }

                static AddressOffsetType execute(Instruction* instruction, ExecutionState* state)
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
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                    }

                    // In Torque, the result of the assignment is pushed to stack
                    stack.push_back(result);
                    return 1;
                }
        };
    }
}
