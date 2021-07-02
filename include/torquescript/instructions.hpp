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
#include <torquescript/storedvaluereference.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
{
    namespace Instructions
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
                virtual AddressOffsetType execute(ExecutionState* state) = 0;

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

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValueReference(mParameter));
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

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValueReference(mParameter));
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
                PushStringInstruction(const StringTableEntry value) : mStringID(value)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValueReference(mStringID, StoredValueType::String));
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
                StringTableEntry mStringID;
        };

        /**
         *  @brief Push a reference to a named local variable. The parameter provided here
         *  should be excluding the '%' prefix.
         */
        class PushLocalReferenceInstruction : public Instruction
        {
            public:
                PushLocalReferenceInstruction(const StringTableEntry value) : mStringID(value)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.push_back(StoredValueReference(mStringID, StoredValueType::LocalReference));
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
                StringTableEntry mStringID;
        };

        /**
         *  @brief Push a reference to a named global variable. The parameter provided here
         *  should be excluding the '$' prefix.
         */
        class PushGlobalReferenceInstruction : public Instruction
        {
            public:
                PushGlobalReferenceInstruction(const StringTableEntry value) : mStringID(value)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
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
                StringTableEntry mStringID;
        };

        /**
         *  @brief Performs an addition of two values on the stack and assigns the result.
         */
        class AddAssignmentInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    // Pull two values off the stack
                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    float resultRaw = 0.0f;
                    resultRaw = lhsStored.mValue->toFloat(state);
                    resultRaw += rhsStored.mValue->toFloat(state);

                    StoredValue result = StoredValue(resultRaw);
                    if (!lhsStored.mValue->setValue(result, state))
                    {
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
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
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    // Pull two values off the stack
                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    if (!lhsStored.mValue->setValue(*rhsStored.mValue, state))
                    {
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
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
         *  @brief Concatenates two values at the top of the stack and pushes back the
         *  result.
         */
        class ConcatInstruction : public Instruction
        {
            public:
                ConcatInstruction(const std::string& seperator) : mSeperator(seperator)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    // Pull two values off the stack
                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    std::string lhs = lhsStored.mValue->toString(state);
                    std::string rhs = rhsStored.mValue->toString(state);

                    // Generate a new string ID
                    const StringTableEntry requestedStringID = state->mInterpreter->mStringTable.getOrAssign(lhs + mSeperator + rhs);
                    stack.push_back(StoredValue(requestedStringID, StoredValueType::String));
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    std::ostringstream result;
                    result << "Concat " << mSeperator;
                    return result.str();
                }

            private:
                std::string mSeperator;
        };

        /**
         *  @brief Negate a value on the stack.
         */
        class NegateInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    // Pull two values off the stack
                    StoredValueReference storedTarget = stack.back();
                    stack.pop_back();

                    stack.push_back(StoredValueReference(-storedTarget.mValue->toFloat(state)));
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "Negate";
                }
        };

        /**
         *  @brief Invert the truthfulness of a value on the stack.
         */
        class NotInstruction : public Instruction
        {
        public:
            virtual AddressOffsetType execute(ExecutionState* state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 1);

                // Pull two values off the stack
                StoredValueReference storedTarget = stack.back();
                stack.pop_back();

                stack.push_back(StoredValueReference(!storedTarget.mValue->toBoolean(state) ? 1 : 0));
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Not";
            }
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
                CallFunctionInstruction(const std::string& space, const std::string& name, const std::size_t argc) : mNameSpace(space), mName(name), mArgc(argc)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    const std::string namespaceName = toLowerCase(mNameSpace);
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

                            stream << "Could not find parent function '" << mName << "' for calling! Placing 0 on the stack.";
                            state->mInterpreter->mConfig.mPlatform->logError(stream.str());

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
                        state->mInterpreter->mConfig.mPlatform->logError(stream.str());

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
                    std::size_t mArgc;
        };

        /**
         *  @brief Performs a logical and of two values on the stack and pushes back the result.
         */
        class LogicalAndInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    const bool lhs = lhsStored.mValue->toBoolean(state);
                    const bool rhs = rhsStored.mValue->toBoolean(state);

                    const int result = lhs && rhs ? 1 : 0;
                    stack.push_back(StoredValue(result));
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "LogicalAnd";
                }
        };

        /**
         *  @brief Performs a logical or of two values on the stack and pushes back the result.
         */
        class LogicalOrInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    const bool lhs = lhsStored.mValue->toBoolean(state);
                    const bool rhs = rhsStored.mValue->toBoolean(state);

                    const int result = lhs || rhs ? 1 : 0;
                    stack.push_back(StoredValueReference(result));
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "LogicalOr";
                }
        };

        /**
         *  @brief Adds together two values on the stack and pushes the sum.
         */
        class AddInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    // NOTE: For now we normalize to floats
                    float lhs = lhsStored.mValue->toFloat(state);
                    float rhs = rhsStored.mValue->toFloat(state);

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
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    // NOTE: For now we normalize to floats
                    float lhs = lhsStored.mValue->toFloat(state);
                    float rhs = rhsStored.mValue->toFloat(state);

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
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    // NOTE: For now we normalize to floats
                    float lhs = lhsStored.mValue->toFloat(state);
                    float rhs = rhsStored.mValue->toFloat(state);

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
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    // NOTE: For now we normalize to floats
                    int lhs = lhsStored.mValue->toInteger(state);
                    int rhs = rhsStored.mValue->toInteger(state);

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
         *  @brief Multiplies together two values on the stack and pushes the result back
         *  to the stack.
         */
        class MultiplyInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();

                    // NOTE: For now we normalize to floats

                    float lhs = lhsStored.mValue->toFloat(state);
                    float rhs = rhsStored.mValue->toFloat(state);

                    const float result = lhs * rhs;
                    stack.push_back(StoredValue(result));
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "Multiply";
                }
        };

        /**
         *  @brief Performs a divide on two values at the top of the stack, pushing
         *  the result back.
         */
        class DivideInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 2);

                    StoredValueReference rhsStored = stack.back();
                    stack.pop_back();
                    StoredValueReference lhsStored = stack.back();
                    stack.pop_back();

                    // NOTE: For now we normalize to floats

                    float lhs = lhsStored.mValue->toFloat(state);
                    float rhs = rhsStored.mValue->toFloat(state);

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
                virtual AddressOffsetType execute(ExecutionState* state) override
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
                JumpInstruction(const AddressType offset) : mOffset(offset)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
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
                AddressType mOffset;
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
                JumpTrueInstruction(const AddressType offset) : mOffset(offset)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValueReference booleanStored = stack.back();
                    stack.pop_back();

                    if (booleanStored.mValue->toBoolean(state))
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
                AddressType mOffset;
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
                JumpFalseInstruction(const AddressType offset) : mOffset(offset)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValueReference booleanStored = stack.back();
                    stack.pop_back();

                    if (!booleanStored.mValue->toBoolean(state))
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
                AddressType mOffset;
        };

        /**
         *  @brief Instruction that does nothing. It usually is used to pad jumps in order
         *  to provide safe jump targets within the current program space.
         */
        class NOPInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "NOP";
                }
        };

        /**
         *  @brief Registers a callable function to the registry in the current interpreter.
         */
        class FunctionDeclarationInstruction : public Instruction
        {
            public:
                /**
                 *  @brief Constructs a new instance of FunctionDeclarationInstruction.
                 *  @param package The package this function is defined in.
                 *  @param space The namespace this function is defined in.
                 *  @param name The name of this function.
                 *  @param parameterNames The names of all parameters this function is expecting.
                 *  @param instructions The instructions that make up the body of this function.
                 */
                FunctionDeclarationInstruction(const std::string package, const std::string& space, const std::string& name, const std::vector<std::string> parameterNames, const InstructionSequence& instructions) : mPackageName(package), mNameSpace(space), mName(name), mParameterNames(parameterNames), mInstructions(instructions)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
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
                SubReferenceInstruction(const StringTableEntry value, const std::size_t arrayIndices) : mStringID(value), mArrayIndices(arrayIndices)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    const std::string arrayName = resolveArrayNameFromStack(stack, state, state->mInterpreter->mStringTable.getString(mStringID), mArrayIndices);

                    StoredValueReference targetStored = stack.back();
                    stack.pop_back();
                    ConsoleObject* referenced = targetStored.mValue->toConsoleObject(state);

                    if (referenced)
                    {
                        const StringTableEntry stringID = state->mInterpreter->mStringTable.getOrAssign(arrayName);

                        stack.push_back(StoredValueReference(referenced, mStringID));
                        return 1;
                    }

                    stack.push_back(StoredValueReference(0));
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    std::ostringstream out;
                    out << "SubReference " << mStringID << " argc=" << mArrayIndices;
                    return out.str();
                }

            private:
                StringTableEntry mStringID;
                std::size_t mArrayIndices;
        };

        /**
         *  @brief Ends execution in the current function immediately. It will take one
         *  value from the top of the stack and push it to the parent stack before returning.
         */
        class ReturnInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValueReference targetStored = stack.back();
                    stack.pop_back();

                    // For if we return a variable reference, we want to pass back a copy
                    StoredValueStack& returnStack = state->mExecutionScope.getReturnStack();
                    returnStack.push_back(targetStored.mValue->getReferencedValueCopy(state));
                    return 0;
                };

                virtual std::string disassemble() override
                {
                    std::ostringstream out;
                    out << "Return";
                    return out.str();
                }
        };

        /**
         *  @brief Used to keep track of loops in the virtual instructions. This instruction
         *  marks the start of a loop control construct.
         */
        class PushLoopInstruction : public Instruction
        {
            public:
                /**
                 *  @brief Constructs a new instance of PushLoopInstruction.
                 *  @param loopSize The number of instructions contained in this loop.
                 */
                PushLoopInstruction(const std::size_t loopSize) : mLoopSize(loopSize)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    const AddressType loopPosition = state->mInstructionPointer;
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
                std::size_t mLoopSize;
        };

        /**
         *  @brief Used to keep track of loops in the virtual instructions. This instruction
         *  marks the end of a loop control construct.
         */
        class PopLoopInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    state->mExecutionScope.popLoop();
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "PopLoop";
                }
        };

        /**
         *  @brief Breaks out of the current loop, ending all possible loop iterations immediately.
         */
        class BreakInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    if (state->mExecutionScope.isLoopStackEmpty())
                    {
                        state->mInterpreter->mConfig.mPlatform->logWarning("Break outside of loop, ignoring ...");
                        return 1;
                    }

                    LoopDescriptor descriptor = state->mExecutionScope.currentLoopDescriptor();
                    // This should lead to a PopLoop instruction so we don't pop here
                    const AddressType loopProgress = state->mInstructionPointer - descriptor.mInstructionPointer;
                    return descriptor.mLoopSize - loopProgress;
                };

                virtual std::string disassemble() override
                {
                    return "Break";
                }
        };

        /**
         *  @brief Accesses an array on a local or global variable. Technically, we just take all
         *  array indices and use them to generate a new variable name to emulate array accesses.
         */
        class AccessArrayInstruction : public Instruction
        {
            public:
                /**
                 *  @brief Constructs a new instance of AccessArrayInstruction.
                 *  @param name The base name of the array to access.
                 *  @param argc The number of array indices to load from the stack.
                 *  @param global Whether or not the array access is against a global or not.
                 */
                AccessArrayInstruction(const std::string& name, const std::size_t argc, bool global) : mName(name), mArgc(argc), mGlobal(global)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    // When we encounter this instruction, we generate a new variable reference by appending all string representations together
                    // This is what T2 does - it has no concept of arrays despite pretending to

                    const std::string arrayName = resolveArrayNameFromStack(stack, state, mName, mArgc);

                    const std::size_t stringID = state->mInterpreter->mStringTable.getOrAssign(arrayName);
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
                    std::size_t mArgc;
                    bool mGlobal;
        };

        /**
         *  @brief Calls a function that is bound to an object identified on the stack.
         */
        class CallBoundFunctionInstruction : public Instruction
        {
            public:
                /**
                 *  @brief Constructs a new instance of CallBoundFunctionInstruction.
                 *  @param name The name of the function to call.
                 *  @param argc The number of arguments to pull from the stack when resolving the call.
                 */
                CallBoundFunctionInstruction(const std::string& name, const std::size_t argc) : mName(name), mArgc(argc)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValueReference targetStored = stack.back();
                    stack.pop_back();

                    // Retrieve the referenced ConsoleObject
                    ConsoleObject* targetObject = targetStored.mValue->toConsoleObject(state);
                    if (!targetObject)
                    {
                        std::ostringstream output;
                        output << "Cannot find object '" << targetStored.mValue->toString(state) << "' to call function '" << mName << "'!";
                        state->mInterpreter->mConfig.mPlatform->logWarning(output.str());

                        stack.push_back(StoredValueReference(0));
                        return 1;
                    }

                    // Walk the class hierarchy
                    ConsoleObjectDescriptor* descriptor = sConsoleObjectDescriptors->at(targetObject->getClassName());
                    assert(descriptor->mHierarchy.size() != 0);

                    for (const std::string& className : descriptor->mHierarchy)
                    {
                        // Ask the interpreter to lookup the function
                        std::shared_ptr<Function> calledFunction = state->mInterpreter->getFunction(className, mName);
                        if (calledFunction)
                        {
                            calledFunction->execute(targetObject, state, mArgc);
                            return 1;
                        }
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
                    std::size_t mArgc;
        };

        class PushObjectInstantiationInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    assert(stack.size() >= 2);

                    StoredValueReference objectName = stack.back();
                    stack.pop_back();
                    StoredValueReference objectTypeName = stack.back();
                    stack.pop_back();

                    state->mExecutionScope.pushObjectInstantiation(objectTypeName.mValue->toString(state), objectName.mValue->toString(state));

                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "PushObjectInstantiation";
                }
        };

        class PushObjectFieldInstruction : public Instruction
        {
            public:
                PushObjectFieldInstruction(const std::size_t fieldComponentCount) : mFieldComponentCount(fieldComponentCount)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    StoredValueReference rvalue = stack.back();
                    stack.pop_back();

                    // Load array components
                    std::vector<std::string> arrayComponents;
                    for (unsigned int iteration = 0; iteration < mFieldComponentCount; ++iteration)
                    {
                        arrayComponents.push_back(stack.popString(state));
                    }

                    // Load base name
                    StoredValueReference fieldBaseName = stack.back();
                    stack.pop_back();

                    std::ostringstream out;
                    out << fieldBaseName.mValue->toString(state);
                    for (auto iterator = arrayComponents.rbegin(); iterator != arrayComponents.rend(); ++iterator)
                    {
                        if (iterator != arrayComponents.rbegin())
                        {
                            out << "_";
                        }
                        out << *iterator;
                    }

                    // Final field assignment
                    ObjectInstantiationDescriptor& descriptor = state->mExecutionScope.currentObjectInstantiation();

                    auto search = descriptor.mFieldAssignments.find(out.str());
                    if (search != descriptor.mFieldAssignments.end())
                    {
                        search->second = rvalue.mValue;
                    }
                    else
                    {
                        descriptor.mFieldAssignments.insert(std::make_pair(out.str(), rvalue.mValue));
                    }

                    return 1;
                };

                virtual std::string disassemble() override
                {
                    std::ostringstream out;
                    out << "PushObjectField argc=" << mFieldComponentCount;
                    return out.str();
                }

            private:
                std::size_t mFieldComponentCount;
        };

        class PopObjectInstantiationInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    ObjectInstantiationDescriptor descriptor = state->mExecutionScope.popObjectInstantiation();

                    // Track parent/child relationships so we can walk the tree later
                    if (state->mExecutionScope.isAwaitingParentInstantiation())
                    {
                        ObjectInstantiationDescriptor& parentDescriptor = state->mExecutionScope.currentObjectInstantiation();
                        parentDescriptor.mChildren.push_back(descriptor);
                    }
                    else
                    {
                        // Ask the interpreter to initialize the resulting tree
                        ConsoleObject* result = state->mInterpreter->initializeConsoleObjectTree(descriptor);

                        if (result)
                        {
                            stack.push_back(StoredValue((int)state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObjectID(result)));
                        }
                        else
                        {
                            stack.push_back(StoredValue(-1));
                        }
                    }


                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "PopObjectInstantiation";
                }
        };
    }
}
