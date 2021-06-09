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
        FloatMemory,
        IntegerMemory,
        StringMemory
    };

    union StoredValueUnion
    {
        int mInteger;
        float mFloat;
        std::size_t mStringID;

        StoredValueUnion()
        {

        }

        StoredValueUnion(const int value) : mInteger(value)
        {

        }

        StoredValueUnion(const float value) : mFloat(value)
        {

        }

        StoredValueUnion(const std::size_t value) : mStringID(value)
        {

        }
    };

    /**
     *  @brief Storage class used to keep variable values in-memory of arbitrary data types.
     *  The data types supported as integers, floats and strings (via string ID table references).
     */
    class StoredValue
    {
        public:
            StoredValue(void* memoryLocation, const MemoryReferenceType type) : mType(StoredValueType::MemoryReference), mStorage(), mConsoleObject(nullptr), mMemoryLocation(memoryLocation)
            {

            }

            StoredValue(const int value) : mType(StoredValueType::Integer), mStorage(value), mConsoleObject(nullptr), mMemoryLocation(nullptr)
            {

            }

            StoredValue(const float value) : mType(StoredValueType::Float), mStorage(value), mConsoleObject(nullptr), mMemoryLocation(nullptr)
            {

            }

            StoredValue(const std::size_t value, const StoredValueType type) : mType(type), mStorage(value), mConsoleObject(nullptr), mMemoryLocation(nullptr)
            {

            }

            StoredValue(std::shared_ptr<ConsoleObject> object, const std::size_t field) : mType(StoredValueType::SubfieldReference), mStorage(field), mConsoleObject(object), mMemoryLocation(nullptr)
            {

            }

            int toInteger(std::shared_ptr<ExecutionState> state);

            /**
             *  @brief Converts the value in question to a native floating point type.
             *  @param scope The execution scope within which this conversion is occurring.
             *  @return A floating point representation of this value.
             */
            float toFloat(std::shared_ptr<ExecutionState> state);

            /**
             *  @brief Converts the value in question to a native sting type.
             *  @param scope The execution scope within which this conversion is occurring.
             *  @return A string representation of this value.
             */
            std::string toString(std::shared_ptr<ExecutionState> state);

            bool toBoolean(std::shared_ptr<ExecutionState> state);

            std::shared_ptr<ConsoleObject> toConsoleObject(std::shared_ptr<ExecutionState> state);

            StoredValue getReferencedValueCopy(std::shared_ptr<ExecutionState> state);

            bool isInteger(std::shared_ptr<ExecutionState> state);

            // In Torque, if we end up trying to set a value of ie. a float it does nothing
            bool setValue(StoredValue newValue, std::shared_ptr<ExecutionState> state);

            std::string getRepresentation();

        private:
            StoredValueType mType;
            StoredValueUnion mStorage;
            MemoryReferenceType mMemoryReferenceType;

            void* mMemoryLocation;
            std::shared_ptr<ConsoleObject> mConsoleObject;
    };
}
