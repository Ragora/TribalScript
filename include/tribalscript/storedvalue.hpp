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

#include <tribalscript/common.hpp>
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

    struct StoredValueUnion
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
        StoredValue() : mType(StoredValueType::Integer), mStorage(0), mMemoryLocation(nullptr), mConsoleObject(nullptr), mReference(nullptr)
        {

        }

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

        explicit StoredValue(const std::string& value) : StoredValue(value.c_str(), value.length())
        {
 
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

        friend  StoredValue operator-(const StoredValue& lhs, const StoredValue& rhs)
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

        TRIBALSCRIPT_FORCEINLINE int toInteger() const
        {
            if (mReference)
            {
                return mReference->toInteger();
            }

            StoredValue* referenced;

            switch (mType)
            {
                case StoredValueType::Integer:
                    return mStorage.mInteger;
                case StoredValueType::Float:
                    return (int)mStorage.mFloat;
                case StoredValueType::String:
                    try
                    {
                        return std::stoi(mStorage.mStringPointer);
                    }
                    catch (std::invalid_argument exception)
                    {
                        return 0;
                    }
            }

            throw std::runtime_error("Unknown Conversion");
        }

        TRIBALSCRIPT_FORCEINLINE std::string toString() const
        {
            if (mReference)
            {
                return mReference->toString();
            }

            StoredValue* referenced;

            switch (mType)
            {
                case StoredValueType::Integer:
                    return std::to_string(mStorage.mInteger);
                case StoredValueType::Float:
                    return std::to_string(mStorage.mFloat);
                case StoredValueType::String:
                    return mStorage.mStringPointer;
            }

            throw std::runtime_error("Unknown Conversion");
        }

        /**
         *  @brief Converts the value in question to a native floating point type.
         *  @param scope The execution scope within which this conversion is occurring.
         *  @return A floating point representation of this value.
         */
        TRIBALSCRIPT_FORCEINLINE float toFloat() const
        {
            if (mReference)
            {
                return mReference->toFloat();
            }
            else if (mMemoryLocation)
            {
                switch (mType)
                {
                case StoredValueType::Float:
                    return *reinterpret_cast<float*>(mMemoryLocation);
                case StoredValueType::Integer:
                    return static_cast<float>(*reinterpret_cast<int*>(mMemoryLocation));
                }
            }

            StoredValue* referenced;

            switch (mType)
            {

            case StoredValueType::Integer:
                return (float)mStorage.mInteger;
            case StoredValueType::Float:
                return mStorage.mFloat;
            case StoredValueType::String:
                return std::stof(mStorage.mStringPointer);
            }
        }

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
        TRIBALSCRIPT_FORCEINLINE bool setValue(const StoredValue& newValue)
        {
            if (mReference)
            {
                return mReference->setValue(newValue);
            }
            else if (mMemoryLocation)
            {
                switch (mType)
                {
                case StoredValueType::Float:
                    *reinterpret_cast<float*>(mMemoryLocation) = newValue.toFloat();
                    return true;
                case StoredValueType::Integer:
                    *reinterpret_cast<int*>(mMemoryLocation) = newValue.toInteger();
                    return true;
                }
            }

            // Copy over stored data
            if (newValue.mReference)
            {
                mType = newValue.mReference->mType;
                mStorage = newValue.mReference->mStorage;
            }
            else
            {
                mType = newValue.mType;
                mStorage = newValue.mStorage;
            }

            return true;
        }

        void setValue(const float newValue);

        TRIBALSCRIPT_FORCEINLINE void setValue(const int newValue)
        {
            if (mReference)
            {
                return mReference->setValue(newValue);
            }
            else if (mMemoryLocation)
            {
                switch (mType)
                {
                case StoredValueType::Float:
                    *reinterpret_cast<float*>(mMemoryLocation) = newValue;
                    return;
                case StoredValueType::Integer:
                    *reinterpret_cast<int*>(mMemoryLocation) = static_cast<int>(newValue);
                    return;
                }
            }

            mType = StoredValueType::Integer;
            mStorage.mInteger = newValue;
        }

        std::string getRepresentation();

    private:
        StoredValueType mType;
        StoredValueUnion mStorage;

        void* mMemoryLocation;
        ConsoleObject* mConsoleObject;
        StoredValue* mReference;
    };
}
