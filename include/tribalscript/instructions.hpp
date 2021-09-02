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
#include <cassert>
#include <iostream>
#include <sstream>

#include <tribalscript/function.hpp>
#include <tribalscript/interpreter.hpp>
#include <tribalscript/executionscope.hpp>
#include <tribalscript/storedvalue.hpp>
#include <tribalscript/storedvaluestack.hpp>
#include <tribalscript/executionstate.hpp>
#include <tribalscript/instructionsequence.hpp>

namespace TribalScript
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
                    stack.emplace_back(mParameter);
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
                    stack.emplace_back(mParameter);
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
                PushStringInstruction(const std::string& value) : mString(value)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    stack.emplace_back(mString.c_str());
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    std::ostringstream out;
                    out << "PushString " << mString;
                    return out.str();
                }

            private:
                //! The string table ID of the parameter to push.
                std::string mString;
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
                    stack.emplace_back(state->mExecutionScope.getVariableOrAllocate(mStringID));
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
                    stack.emplace_back(state->mInterpreter->getGlobalOrAllocate(mStringID));
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
                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    float resultRaw = 0.0f;
                    resultRaw = lhsStored.toFloat();
                    resultRaw += rhsStored.toFloat();

                    StoredValue result = StoredValue(resultRaw);
                    if (!lhsStored.setValue(result))
                    {
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                    }

                    stack.erase(stack.end() - 2, stack.end());

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
                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    if (!lhsStored.setValue(rhsStored))
                    {
                        state->mInterpreter->mConfig.mPlatform->logError("Attempted to perform no-op assignment!");
                    }

                    stack.erase(stack.end() - 2, stack.end());

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
                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    std::string lhs = lhsStored.toString();
                    std::string rhs = rhsStored.toString();

                    // Generate a new string ID
                    const std::string requestedString = lhs + mSeperator + rhs;

                    stack.erase(stack.end() - 2, stack.end());

                    stack.emplace_back(requestedString.c_str());
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
                    StoredValue& storedTarget = stack.back();

                    const float result = -storedTarget.toFloat();
                    stack.pop_back();
                    stack.emplace_back(result);
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

                assert(!stack.empty());

                StoredValue& storedTarget = stack.back();

                const int result = !storedTarget.toBoolean() ? 1 : 0;

                stack.pop_back();
                stack.emplace_back(result);
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
                CallFunctionInstruction(std::string space, std::string name, const std::size_t argc) : mNameSpace(std::move(space)), mName(std::move(name)), mArgc(argc)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    const std::string namespaceName = toLowerCase(mNameSpace);
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    // FIXME: At the moment we're loading parameters like this which is incurring unnecessary copies
                    std::vector<StoredValue> parameters;
                    auto parametersStart = stack.end() - mArgc;

                    for (std::size_t iteration = 0; iteration < mArgc; ++iteration)
                    {
                        auto iterator = parametersStart + iteration;
                        parameters.push_back(*iterator);
                    }
                    stack.erase(parametersStart, stack.end());

                    // If we're calling a parent function, perform an alternative lookup
                    if (namespaceName == "parent")
                    {
                        Function* currentFunction = state->mExecutionScope.getCurrentFunction();
                        if (!currentFunction)
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
                        parentFunction->execute(nullptr, state, parameters);

                        return 1;
                    }

                    std::shared_ptr<Function> functionLookup = state->mInterpreter->getFunction(mNameSpace, mName);
                    if (functionLookup)
                    {
                        functionLookup->execute(nullptr, state, parameters);
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    const bool lhs = lhsStored.toBoolean();
                    const bool rhs = rhsStored.toBoolean();

                    const int result = lhs && rhs ? 1 : 0;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    const bool lhs = lhsStored.toBoolean();
                    const bool rhs = rhsStored.toBoolean();

                    const int result = lhs || rhs ? 1 : 0;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    // NOTE: For now we normalize to floats
                    float lhs = lhsStored.toFloat();
                    float rhs = rhsStored.toFloat();

                    const float result = lhs + rhs;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "Add";
                }
        };

        /**
         *  @brief Adds together two values on the stack and pushes the sum.
         */
        class MinusInstruction : public Instruction
        {
        public:
            virtual AddressOffsetType execute(ExecutionState* state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue& rhsStored = *(stack.end() - 1);
                StoredValue& lhsStored = *(stack.end() - 2);

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const float result = lhs - rhs;

                stack.erase(stack.end() - 2, stack.end());
                stack.emplace_back(result);
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Minus";
            }
        };

        /**
         *  @brief Adds together two values on the stack and pushes the sum.
         */
        class ModulusInstruction : public Instruction
        {
        public:
            virtual AddressOffsetType execute(ExecutionState* state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue& rhsStored = *(stack.end() - 1);
                StoredValue& lhsStored = *(stack.end() - 2);

                // NOTE: For now we normalize to floats
                int lhs = lhsStored.toInteger();
                int rhs = rhsStored.toInteger();

                const int result = lhs % rhs;

                stack.erase(stack.end() - 2, stack.end());
                stack.emplace_back(result);
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "Modulus";
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    // NOTE: For now we normalize to floats
                    float lhs = lhsStored.toFloat();
                    float rhs = rhsStored.toFloat();

                    const int result = lhs < rhs ? 1 : 0;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "LessThan";
                }
        };

        class GreaterThanInstruction : public Instruction
        {
        	public:
	            virtual AddressOffsetType execute(ExecutionState* state) override
	            {
	                StoredValueStack& stack = state->mExecutionScope.getStack();

	                assert(stack.size() >= 2);

	                StoredValue& rhsStored = *(stack.end() - 1);
	                StoredValue& lhsStored = *(stack.end() - 2);

	                // NOTE: For now we normalize to floats
	                float lhs = lhsStored.toFloat();
	                float rhs = rhsStored.toFloat();

	                const int result = lhs > rhs ? 1 : 0;

	                stack.erase(stack.end() - 2, stack.end());
	                stack.emplace_back(result);
	                return 1;
	            };

	            virtual std::string disassemble() override
	            {
	                return "GreaterThan";
	            }
        };

		class GreaterThanOrEqualInstruction : public Instruction
		{
			public:
				virtual AddressOffsetType execute(ExecutionState* state) override
				{
					StoredValueStack& stack = state->mExecutionScope.getStack();

					assert(stack.size() >= 2);

					StoredValue& rhsStored = *(stack.end() - 1);
					StoredValue& lhsStored = *(stack.end() - 2);

					// NOTE: For now we normalize to floats
					float lhs = lhsStored.toFloat();
					float rhs = rhsStored.toFloat();

					const int result = lhs >= rhs ? 1 : 0;

					stack.erase(stack.end() - 2, stack.end());
					stack.emplace_back(result);
					return 1;
				};

				virtual std::string disassemble() override
				{
					return "GreaterThanOrEqual";
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    // NOTE: For now we normalize to floats
                    float lhs = lhsStored.toFloat();
                    float rhs = rhsStored.toFloat();

                    const int result = lhs == rhs ? 1 : 0;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "Equals";
                }
        };

        /**
         *  @brief Compares two values on the stack using an equality.
         */
        class NotEqualsInstruction : public Instruction
        {
        public:
            virtual AddressOffsetType execute(ExecutionState* state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue& rhsStored = *(stack.end() - 1);
                StoredValue& lhsStored = *(stack.end() - 2);

                // NOTE: For now we normalize to floats
                float lhs = lhsStored.toFloat();
                float rhs = rhsStored.toFloat();

                const int result = lhs != rhs ? 1 : 0;

                stack.erase(stack.end() - 2, stack.end());
                stack.emplace_back(result);
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "NotEquals";
            }
        };

        /**
         *  @brief Compares two values on the stack using an equality.
         */
        class StringEqualsInstruction : public Instruction
        {
        public:
            virtual AddressOffsetType execute(ExecutionState* state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue& rhsStored = *(stack.end() - 1);
                StoredValue& lhsStored = *(stack.end() - 2);

                // NOTE: For now we normalize to floats
                std::string lhs = lhsStored.toString();
                std::string rhs = rhsStored.toString();

                const int result = lhs == rhs ? 1 : 0;

                stack.erase(stack.end() - 2, stack.end());
                stack.emplace_back(result);
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "StringEquals";
            }
        };

        class StringNotEqualInstruction : public Instruction
        {
        public:
            virtual AddressOffsetType execute(ExecutionState* state) override
            {
                StoredValueStack& stack = state->mExecutionScope.getStack();

                assert(stack.size() >= 2);

                StoredValue& rhsStored = *(stack.end() - 1);
                StoredValue& lhsStored = *(stack.end() - 2);

                // NOTE: For now we normalize to floats
                std::string lhs = lhsStored.toString();
                std::string rhs = rhsStored.toString();

                const int result = lhs != rhs ? 1 : 0;

                stack.erase(stack.end() - 2, stack.end());
                stack.emplace_back(result);
                return 1;
            };

            virtual std::string disassemble() override
            {
                return "StringNotEqual";
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    // NOTE: For now we normalize to floats
                    int lhs = lhsStored.toInteger();
                    int rhs = rhsStored.toInteger();

                    const int result = lhs & rhs;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    // NOTE: For now we normalize to floats

                    float lhs = lhsStored.toFloat();
                    float rhs = rhsStored.toFloat();

                    const float result = lhs * rhs;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
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

                    StoredValue& rhsStored = *(stack.end() - 1);
                    StoredValue& lhsStored = *(stack.end() - 2);

                    // NOTE: For now we normalize to floats

                    float lhs = lhsStored.toFloat();
                    float rhs = rhsStored.toFloat();

                    const float result = lhs / rhs;

                    stack.erase(stack.end() - 2, stack.end());
                    stack.emplace_back(result);
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
                JumpInstruction(const AddressOffsetType offset) : mOffset(offset)
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
                AddressOffsetType mOffset;
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
                JumpTrueInstruction(const AddressOffsetType offset) : mOffset(offset)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValue& booleanStored = stack.back();
                    if (booleanStored.toBoolean())
                    {
                        stack.pop_back();
                        return mOffset;
                    }

                    stack.pop_back();
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
                AddressOffsetType mOffset;
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
                JumpFalseInstruction(const AddressOffsetType offset) : mOffset(offset)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();

                    assert(stack.size() >= 1);

                    StoredValue& booleanStored = stack.back();
                    if (!booleanStored.toBoolean())
                    {
                        stack.pop_back();
                        return mOffset;
                    }

                    stack.pop_back();
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
                AddressOffsetType mOffset;
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

                    StoredValue targetStored = stack.back();
                    stack.pop_back();

                    ConsoleObject* referenced = targetStored.toConsoleObject(state);
                    if (referenced)
                    {
                        // Obtain a reference to the console object's field
                        stack.emplace_back(referenced->getTaggedFieldOrAllocate(arrayName));
                        return 1;
                    }

                    stack.emplace_back(0);
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

                    StoredValue& targetStored = stack.back();

                    // For if we return a variable reference, we want to pass back a copy
                    StoredValueStack& returnStack = state->mExecutionScope.getReturnStack();
                    returnStack.push_back(targetStored.getReferencedValueCopy());

                    stack.pop_back();
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
         *  @brief Breaks out of the current loop, ending all possible loop iterations immediately.
         */
        class BreakInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    // This is a placeholder instruction for the compiler
                    state->mInterpreter->mConfig.mPlatform->logWarning("Break outside of loop, ignoring ...");
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "Break";
                }
        };

        /**
         *  @brief Moves to the next iteration of the current loop.
         */
        class ContinueInstruction : public Instruction
        {
            public:
                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    // This is a placeholder instruction for the compiler
                    state->mInterpreter->mConfig.mPlatform->logWarning("Continue outside of loop, ignoring ...");
                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "Continue";
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
                        stack.emplace_back(state->mInterpreter->getGlobalOrAllocate(stringID));
                    }
                    else
                    {
                        stack.emplace_back(state->mExecutionScope.getVariableOrAllocate(stringID));
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

                    // FIXME: At the moment, parameters for bound functions are *after* the target object on the stack
                    std::vector<StoredValue> parameters;
                    auto parametersStart = stack.end() - mArgc;

                    for (std::size_t iteration = 0; iteration < mArgc; ++iteration)
                    {
                        auto iterator = parametersStart + iteration;
                        parameters.push_back(*iterator);
                    }
                    stack.erase(parametersStart, stack.end());

                    // Next load the target
                    StoredValue targetStored = stack.back();
                    stack.pop_back();

                    // Retrieve the referenced ConsoleObject
                    ConsoleObject* targetObject = targetStored.toConsoleObject(state);
                    if (!targetObject)
                    {
                        std::ostringstream output;
                        output << "Cannot find object '" << targetStored.toString() << "' to call function '" << mName << "'!";
                        state->mInterpreter->mConfig.mPlatform->logWarning(output.str());

                        stack.push_back(StoredValue(0));
                        return 1;
                    }

                    // Walk the class hierarchy
                    ConsoleObjectDescriptor* descriptor = state->mInterpreter->lookupDescriptor(targetObject->getClassName());
                    assert(descriptor);
                    assert(descriptor->mHierarchy.size() != 0);

                    for (const std::string& className : descriptor->mHierarchy)
                    {
                        // Ask the interpreter to lookup the function
                        std::shared_ptr<Function> calledFunction = state->mInterpreter->getFunction(className, mName);
                        if (calledFunction)
                        {
                            calledFunction->execute(targetObject, state, parameters);
                            return 1;
                        }
                    }

                    stack.emplace_back(0);
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

                    StoredValue objectName = stack.back();
                    stack.pop_back();
                    StoredValue objectTypeName = stack.back();
                    stack.pop_back();

                    state->mExecutionScope.pushObjectInstantiation(objectTypeName.toString(), objectName.toString());

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
                    assert(stack.size() >= 2);

                    StoredValue rvalue = stack.back();
                    stack.pop_back();

                    // Load array components
                    std::vector<std::string> arrayComponents;
                    for (unsigned int iteration = 0; iteration < mFieldComponentCount; ++iteration)
                    {
                        arrayComponents.push_back(stack.popString(state));
                    }

                    // Load base name
                    StoredValue fieldBaseName = stack.back();
                    stack.pop_back();

                    std::ostringstream out;
                    out << fieldBaseName.toString();
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
                        search->second = rvalue;
                    }
                    else
                    {
                        descriptor.mFieldAssignments.insert(std::make_pair(out.str(), rvalue));
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
                PopObjectInstantiationInstruction(std::size_t childrenCount) : mChildrenCount(childrenCount)
                {

                }

                virtual AddressOffsetType execute(ExecutionState* state) override
                {
                    StoredValueStack& stack = state->mExecutionScope.getStack();
                    ObjectInstantiationDescriptor descriptor = state->mExecutionScope.popObjectInstantiation();

                    // Ask the interpreter to initialize the resulting tree
                    ConsoleObject* result = state->mInterpreter->initializeConsoleObjectTree(descriptor);

                    if (result)
                    {
                        // Append children
                        for (std::size_t iteration = 0; iteration < mChildrenCount; ++iteration)
                        {
                            StoredValue nextChildID = stack.back();
                            stack.pop_back();

                            ConsoleObject* nextChild = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(nextChildID.toInteger());
                            result->addChild(nextChild);
                        }

                        stack.emplace_back((int)state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObjectID(result));
                    }
                    else
                    {
                        stack.emplace_back(-1);
                    }

                    return 1;
                };

                virtual std::string disassemble() override
                {
                    return "PopObjectInstantiation";
                }

                private:
                    std::size_t mChildrenCount;
        };
    }
}
