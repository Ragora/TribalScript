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

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>

#include <tribalscript/stringtable.hpp>

namespace TribalScript
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
        SubfieldReference
    };

    union StoredValueUnion
    {
        int mInteger;
        float mFloat;
        char* mStringPointer;

        StoredValueUnion()
        {

        }

        explicit StoredValueUnion(const int value) : mInteger(value)
        {

        }

        explicit StoredValueUnion(const float value) : mFloat(value)
        {

        }

        explicit StoredValueUnion(char* value) : mStringPointer(value)
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
        StoredValue(void* memoryLocation, const StoredValueType type) : mType(type), mStorage(), mMemoryLocation(memoryLocation), mConsoleObject(nullptr), mReference(nullptr)
        {

        }

        explicit StoredValue(float* memoryLocation) : mType(StoredValueType::Float), mStorage(), mMemoryLocation(memoryLocation), mConsoleObject(nullptr), mReference(nullptr)
        {

        }

        explicit StoredValue(int* memoryLocation) : mType(StoredValueType::Integer), mStorage(), mMemoryLocation(memoryLocation), mConsoleObject(nullptr), mReference(nullptr)
        {

        }

        explicit StoredValue(const int value) : mType(StoredValueType::Integer), mStorage(value), mMemoryLocation(nullptr), mConsoleObject(nullptr), mReference(nullptr)
        {

        }

        explicit StoredValue(const float value) : mType(StoredValueType::Float), mStorage(value), mMemoryLocation(nullptr), mConsoleObject(nullptr), mReference(nullptr)
        {

        }

        explicit StoredValue(const char* value, const std::size_t stringLength = 0) : mType(StoredValueType::String), mStorage(), mMemoryLocation(nullptr), mConsoleObject(nullptr), mReference(nullptr)
        {
            // Initialize memory
            const std::size_t valueLength = stringLength ? stringLength : strlen(value);

            mStorage.mStringPointer = new char[valueLength + 1];
            std::memcpy(mStorage.mStringPointer, value, valueLength);
            mStorage.mStringPointer[valueLength] = 0x00;
        }

        //StoredValue(ConsoleObject* object, const std::size_t field) : mType(StoredValueType::SubfieldReference), mStorage(field), mMemoryLocation(nullptr), mConsoleObject(object), mReference(nullptr)
       // {

       // }

        explicit StoredValue(StoredValue* referenced) : mType(StoredValueType::NullType), mMemoryLocation(nullptr), mConsoleObject(nullptr), mReference(referenced)
        {

        }

        StoredValue(const StoredValue& copied) : mType(copied.mType), mStorage(copied.mStorage), mMemoryLocation(copied.mMemoryLocation), mConsoleObject(copied.mConsoleObject), mReference(copied.mReference)
        {
            if (copied.mType == StoredValueType::String)
            {
                const std::size_t valueLength = std::strlen(copied.mStorage.mStringPointer);

                mStorage.mStringPointer = new char[valueLength + 1];
                std::memcpy(mStorage.mStringPointer, copied.mStorage.mStringPointer, valueLength);
                mStorage.mStringPointer[valueLength] = 0x00;
            }
        }

        ~StoredValue()
        {
            if (mType == StoredValueType::String)
            {
                // delete mStorage.mStringPointer;
            }
        }

        friend StoredValue operator+(const StoredValue& lhs, const StoredValue& rhs)
        {
            // FIXME: For now we normalize to floats
            return StoredValue(lhs.toFloat() + rhs.toFloat());
        }

        friend StoredValue operator-(const StoredValue& lhs, const StoredValue& rhs)
        {
            // FIXME: For now we normalize to floats
            return StoredValue(lhs.toFloat() - rhs.toFloat());
        }

        friend StoredValue operator/(const StoredValue& lhs, const StoredValue& rhs)
        {
            // FIXME: For now we normalize to floats
            return StoredValue(lhs.toFloat() / rhs.toFloat());
        }

        friend StoredValue operator*(const StoredValue& lhs, const StoredValue& rhs)
        {
            // FIXME: For now we normalize to floats
            return StoredValue(lhs.toFloat() * rhs.toFloat());
        }

        /// @name Value Retrieval
        ///
        /// These functions are used to retrieve the data stored in this object.
        /// @{
        ///

        int toInteger() const;

        /**
         *  @brief Converts the value in question to a native floating point type.
         *  @param scope The execution scope within which this conversion is occurring.
         *  @return A floating point representation of this value.
         */
        float toFloat() const;

        /**
         *  @brief Converts the value in question to a native sting type.
         *  @param scope The execution scope within which this conversion is occurring.
         *  @return A string representation of this value.
         */
        std::string toString();

        bool toBoolean() const;

        ConsoleObject* toConsoleObject(ExecutionState* state);

        /// @}

        StoredValue getReferencedValueCopy() const;

        bool isInteger();

        /**
         *  @brief Sets the value of this object. Only has an effect if this object
         *  is a reference to a local or global variable.
         *  @param newValue The new value to set.
         *  @param state The execution state this assignment is taking place in.
         *  @return True if an assignment has taken place. False otherwise.
         */
        bool setValue(const StoredValue& newValue);
        void setValue(const float newValue);

        std::string getRepresentation();

    private:
        StoredValueType mType;
        StoredValueUnion mStorage;

        void* mMemoryLocation;
        ConsoleObject* mConsoleObject;
        StoredValue* mReference;
    };
}
