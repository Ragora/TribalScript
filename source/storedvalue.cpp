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

#include <torquescript/storedvalue.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/consoleobject.hpp>
#include <torquescript/executionstate.hpp>

namespace TorqueScript
{
    bool StoredValue::toBoolean() const
    {
        if (mReference)
        {
            return mReference->toInteger();
        }

        return this->toInteger() != 0;
    }

    ConsoleObject* StoredValue::toConsoleObject(ExecutionState* state)
    {
        if (mReference)
        {
            return mReference->toConsoleObject(state);
        }

        StoredValue rawValue = this->getReferencedValueCopy();

        // Search by ID first
        if (rawValue.isInteger())
        {
            ConsoleObject* idLookup = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(rawValue.toInteger());
            if (idLookup)
            {
                return idLookup;
            }
        }

        const std::string lookupName = rawValue.toString();
        return state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(lookupName);
    }

    bool StoredValue::isInteger()
    {
        if (mReference)
        {
            return mReference->isInteger();
        }

        return mType == StoredValueType::Integer;
    }

    bool StoredValue::setValue(const StoredValue& newValue)
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
            default:
                throw std::runtime_error("Unknown Memory Type");
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

    void StoredValue::setValue(const float newValue)
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
            default:
                throw std::runtime_error("Unknown Memory Type");
            }
        }

        mType = StoredValueType::Float;
        mStorage.mFloat = newValue;
    }

    int StoredValue::toInteger() const
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

    std::string StoredValue::toString()
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

    StoredValue StoredValue::getReferencedValueCopy() const
    {
        if (mReference)
        {
            return mReference->getReferencedValueCopy();
        }

        switch (mType)
        {
        case StoredValueType::Integer:
            return StoredValue(mStorage.mInteger);
        case StoredValueType::Float:
            return StoredValue(mStorage.mFloat);
        case StoredValueType::String:
            return StoredValue(mStorage.mStringPointer);
        }

        throw std::runtime_error("Unknown Conversion");
    }

    float StoredValue::toFloat() const
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
            default:
                throw std::runtime_error("Unknown Memory Type");
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
            try
            {
                return std::stof(mStorage.mStringPointer);
            }
            catch (std::invalid_argument exception)
            {
                return 0;
            }
        }

        throw std::runtime_error("Unknown Conversion");
    }

    std::string StoredValue::getRepresentation()
    {
        if (mReference)
        {
            return mReference->getRepresentation();
        }

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
}
