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

#include <tribalscript/instructionsequence.hpp>
#include <tribalscript/executionstate.hpp>
#include <tribalscript/instructions.hpp>
#include <tribalscript/codeblock.hpp>

namespace TribalScript
{
    void InstructionSequence::execute(CodeBlock* block, ExecutionState* state)
    {
        AddressType instructionIndex = 0;

        const std::size_t instructionCount = this->size();

        std::string functionNamespace;
        std::string functionName;
        std::size_t functionArgc;
        std::shared_ptr<Function> functionLookup;
        std::vector<StoredValue> functionParameters;
        std::vector<StoredValue>::iterator parametersStart;

        StoredValue* rhsStored;
        StoredValue* lhsStored;
        StoredValue result;
        std::string requestedString;

        int intResult;
        float floatResult;

        while (instructionIndex < instructionCount && instructionIndex >= 0)
        {
            const Instructions::Instruction& nextInstruction = this->at(instructionIndex);

            StoredValueStack& returnStack = state->mExecutionScope.getReturnStack();
            StoredValueStack& stack = state->mExecutionScope.getStack();

            state->mInstructionPointer = instructionIndex;
            switch (nextInstruction.mInstruction)
            {
                case Instructions::InstructionType::RegisterFunction:
                    state->mInterpreter->addFunction(block->getFunction(nextInstruction.mOperands[0].toInteger()));

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::LessThan:
                    assert(stack.size() >= 2);

                    rhsStored = &(*(stack.end() - 1));
                    lhsStored = &(*(stack.end() - 2));

                    intResult = lhsStored->toFloat() < rhsStored->toFloat() ? 1 : 0;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(intResult);

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::PushInteger:
                    stack.emplace_back(nextInstruction.mOperands[0].toInteger());

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::Pop:
                    assert(stack.size() >= 1);
                    stack.pop_back();

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::JumpFalse:
                    assert(stack.size() >= 1);

                    if (!stack.back().toBoolean())
                    {
                        stack.pop_back();

                        instructionIndex += nextInstruction.mOperands[0].toInteger();
                        break;
                    }

                    stack.pop_back();
                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::NOP:
                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::Jump:
                    instructionIndex += nextInstruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::AddAssignment:
                    assert(stack.size() >= 2);

                    // Pull two values off the stack
                    rhsStored = &(*(stack.end() - 1));
                    lhsStored = &(*(stack.end() - 2));

                    floatResult = 0.0f;
                    floatResult = lhsStored->toFloat();
                    floatResult += rhsStored->toFloat();

                    result = StoredValue(floatResult);
                    if (!lhsStored->setValue(result))
                    {
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                    }

                    stack.erase(stack.end() - 2, stack.end());

                    // In Torque, the result of the assignment is pushed to stack
                    stack.push_back(result);

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::PushLocalReference:
                    stack.emplace_back(state->mExecutionScope.getRegisterOrAllocate(nextInstruction.mOperands[0].toInteger()));

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::StringEquals:
                    assert(stack.size() >= 2);

                    rhsStored = &(*(stack.end() - 1));
                    lhsStored = &(*(stack.end() - 2));

                    intResult = lhsStored->toString() == rhsStored->toString() ? 1 : 0;

                    if (intResult)
                    {
                        std::cout << "EQ" << std::endl;
                    }
                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::Return:
                    assert(stack.size() >= 1);

                    rhsStored = &(*(stack.end() - 1));

                    // For if we return a variable reference, we want to pass back a copy
                    returnStack.push_back(rhsStored->getReferencedValueCopy());

                    stack.pop_back();

                case Instructions::InstructionType::PushString:
                    stack.emplace_back(nextInstruction.mOperands[0].toString());

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::Concat:
                    assert(stack.size() >= 2);

                    // Pull two values off the stack
                    rhsStored = &(*(stack.end() - 1));
                    lhsStored = &(*(stack.end() - 2));

                    // Generate a new string ID
                    requestedString = lhsStored->toString() + nextInstruction.mOperands[0].toString() + rhsStored->toString();

                    stack.erase(stack.end() - 2, stack.end());

                    stack.emplace_back(requestedString);

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::PushFloat:
                    stack.emplace_back(nextInstruction.mOperands[0].toFloat());

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::Assignment:
                    assert(stack.size() >= 2);

                    // Pull two values off the stack
                    rhsStored = &(*(stack.end() - 1));
                    lhsStored = &(*(stack.end() - 2));

                    if (!lhsStored->setValue(*rhsStored))
                    {
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                    }

                    stack.erase(stack.end() - 2, stack.end());

                    // In Torque, the result of the assignment is pushed to stack
                    stack.push_back(*rhsStored);

                    ++instructionIndex;
                    break;

                case Instructions::InstructionType::CallFunction:
                    functionNamespace = nextInstruction.mOperands[0].toString();
                    functionNamespace = toLowerCase(functionNamespace);

                    functionName = nextInstruction.mOperands[1].toString();
                    functionArgc = nextInstruction.mOperands[2].toInteger();

                    // FIXME: At the moment we're loading parameters like this which is incurring unnecessary copies
                    parametersStart = stack.end() - functionArgc;

                    for (std::size_t iteration = 0; iteration < functionArgc; ++iteration)
                    {
                        auto iterator = parametersStart + iteration;
                        functionParameters.push_back(*iterator);
                    }
                    stack.erase(parametersStart, stack.end());

                    // If we're calling a parent function, perform an alternative lookup
                    if (functionNamespace == "parent")
                    {
                        Function* currentFunction = state->mExecutionScope.getCurrentFunction();
                        if (!currentFunction)
                        {
                            state->mInterpreter->mConfig.mPlatform->logError("Attempted to call parent:: function at root!");
                            stack.push_back(StoredValue(0));

                            ++instructionIndex;
                            break;
                        }

                        // Once we have a valid function pointer, ask the interpreter to find a super function higher up the chain
                        std::shared_ptr<Function> parentFunction = state->mInterpreter->getFunctionParent(currentFunction);
                        if (!parentFunction)
                        {
                            std::ostringstream stream;

                            stream << "Could not find parent function '" << functionName << "' for calling! Placing 0 on the stack.";
                            state->mInterpreter->mConfig.mPlatform->logError(stream.str());

                            stack.push_back(StoredValue(0));

                            ++instructionIndex;
                            break;
                        }

                        // Otherwise, call it
                        parentFunction->execute(nullptr, state, functionParameters);

                        ++instructionIndex;
                        break;
                    }

                    functionLookup = state->mInterpreter->getFunction(functionNamespace, functionName);
                    if (functionLookup)
                    {
                        functionLookup->execute(nullptr, state, functionParameters);
                    }
                    else
                    {
                        std::ostringstream stream;

                        stream << "Could not find function '" << functionName << "' for calling! Placing 0 on the stack.";
                        state->mInterpreter->mConfig.mPlatform->logError(stream.str());

                        stack.push_back(StoredValue(0));
                    }

                    ++instructionIndex;
                    break;

                default:
                    std::cout << "Invalid Opcode " << nextInstruction.mInstruction << std::endl;
                    return;
            }

           // const AddressOffsetType advance = nextInstruction->execute(state);
           // if (advance == 0)
           // {
           //     break;
          //  }
           // instructionIndex += advance;
        }
    }
}
