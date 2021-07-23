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
#include <memory>
#include <string>

#include <torquescript/storedvalue.hpp>

#define STACK_SIZE 256

namespace TorqueScript
{
    class ExecutionState;

    /**
     *  @brief Storage class used to keep variable values in-memory of arbitrary data types.
     *  This is the base class and should not be instantiated directly.
     */
    class StoredValueStack
    {
        public:
            StoredValueStack();

            int popInteger(ExecutionState* state);
            std::string popString(ExecutionState* state);

            std::vector<std::string> dump();

            bool empty();

            inline float popFloat(ExecutionState* state)
            {
                assert(!this->empty());

                StoredValue& currentValue = this->back();
                const float result = currentValue.toFloat();
                this->pop_back();
                return result;
            }

            inline StoredValue& back()
            {
                assert(mCurrentIndex > 0 && mCurrentIndex <= STACK_SIZE - 1);
                return mStoredValues[mCurrentIndex - 1];
            }

            inline void pop_back()
            {
                assert(mCurrentIndex > 0 && mCurrentIndex <= STACK_SIZE - 1);
                --mCurrentIndex;
                // mStoredValues[mCurrentIndex--] = StoredValue(0);
            }

            std::size_t size();

            inline void push_back(const StoredValue& newValue)
            {
                assert(mCurrentIndex <= STACK_SIZE - 1);
                mStoredValues[mCurrentIndex++] = newValue;
            }

            template <typename... parameters>
            inline void emplace_back(parameters... constructorParameters)
            {
                assert(mCurrentIndex <= STACK_SIZE - 1);
                mStoredValues[mCurrentIndex++] = StoredValue(constructorParameters...);
            }

        private:
            std::size_t mCurrentIndex;

            StoredValue mStoredValues[STACK_SIZE];
    };
}
