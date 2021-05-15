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

#include <iostream>
#include <assert.h>
#include <variant>
#include <string>

namespace TorqueScript
{
    class Interpreter;
    class ExecutionScope;

    /**
     *  @brief Storage class used to keep variable values in-memory of arbitrary data types.
     *  This is the base class and should not be instantiated directly.
     */
    class StoredValue
    {
        public:
            StoredValue(Interpreter* interpreter);

            virtual int toInteger(ExecutionScope* scope) = 0;

            /**
             *  @brief Converts the value in question to a native floating point type.
             *  @param scope The execution scope within which this conversion is occurring.
             *  @return A floating point representation of this value.
             */
            virtual float toFloat(ExecutionScope* scope) = 0;

            /**
             *  @brief Converts the value in question to a native sting type.
             *  @param scope The execution scope within which this conversion is occurring.
             *  @return A string representation of this value.
             */
            virtual std::string toString(ExecutionScope* scope) = 0;

        protected:
            //! The interpreter this value is associated with. Required for global value lookups.
            Interpreter* mInterpreter;
    };
}
