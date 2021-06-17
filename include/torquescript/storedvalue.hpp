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
#include <memory>
#include <string>

#include <torquescript/stringtable.hpp>

namespace TorqueScript
{
    class ExecutionScope;
    class ConsoleObject;
    class ExecutionState;

    enum StoredValueType
    {
        NullType,
        Integer,
        Float,
        String,
        LocalReference,
        GlobalReference,
        MemoryReference,
        SubfieldReference
    };

    enum MemoryReferenceType
    {
        NullReferenceType,
        FloatMemory,
        IntegerMemory,
        StringMemory
    };

    /**
     *  @brief Storage class used to keep variable values in-memory of arbitrary data types.
     *  The data types supported as integers, floats and strings (via string ID table references).
     */
    class StoredValue
    {
        public:
            StoredValue(void* memoryLocation, const MemoryReferenceType type) : mType(StoredValueType::MemoryReference), mMemoryReferenceType(type), mMemoryLocation(memoryLocation), mConsoleObject(nullptr)
            {

            }

            StoredValue(const int value) : mType(StoredValueType::Integer), mInteger(value), mMemoryReferenceType(MemoryReferenceType::NullReferenceType), mMemoryLocation(nullptr), mConsoleObject(nullptr)
            {

            }

            StoredValue(const float value) : mType(StoredValueType::Float), mFloat(value), mMemoryReferenceType(MemoryReferenceType::NullReferenceType), mMemoryLocation(nullptr), mConsoleObject(nullptr)
            {

            }

            StoredValue(const std::size_t value, const StoredValueType type) : mType(type), mStringID(value), mMemoryReferenceType(MemoryReferenceType::NullReferenceType), mMemoryLocation(nullptr), mConsoleObject(nullptr)
            {

            }

            StoredValue(ConsoleObject* object, const std::size_t field) : mType(StoredValueType::SubfieldReference), mStringID(field), mMemoryReferenceType(MemoryReferenceType::NullReferenceType), mMemoryLocation(nullptr), mConsoleObject(object)
            {

            }

            /// @name Value Retrieval
            ///
            /// These functions are used to retrieve the data stored in this object.
            /// @{
            ///

            int toInteger(ExecutionState* state);

            /**
             *  @brief Converts the value in question to a native floating point type.
             *  @param scope The execution scope within which this conversion is occurring.
             *  @return A floating point representation of this value.
             */
            float toFloat(ExecutionState* state);

            /**
             *  @brief Converts the value in question to a native sting type.
             *  @param scope The execution scope within which this conversion is occurring.
             *  @return A string representation of this value.
             */
            std::string toString(ExecutionState* state);

            bool toBoolean(ExecutionState* state);

            ConsoleObject* toConsoleObject(ExecutionState* state);

            /// @}

            StoredValue getReferencedValueCopy(ExecutionState* state);

            bool isInteger(ExecutionState* state);

            /**
             *  @brief Sets the value of this object. Only has an effect if this object
             *  is a reference to a local or global variable.
             *  @param newValue The new value to set.
             *  @param state The execution state this assignment is taking place in.
             *  @return True if an assignment has taken place. False otherwise.
             */
            bool setValue(StoredValue newValue, ExecutionState* state);

            std::string getRepresentation();

        private:
            int mInteger;
            float mFloat;
            StringTableEntry mStringID;

            StoredValueType mType;
            MemoryReferenceType mMemoryReferenceType;

            void* mMemoryLocation;
            ConsoleObject* mConsoleObject;
    };
}
