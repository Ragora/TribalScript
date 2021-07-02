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
    bool StoredValue::toBoolean(ExecutionState* state) const
    {
        if (mReference)
        {
            return mReference->toInteger(state);
        }

        return this->toInteger(state) != 0;
    }

    ConsoleObject* StoredValue::toConsoleObject(ExecutionState* state)
    {
        if (mReference)
        {
            return mReference->toConsoleObject(state);
        }

        StoredValue rawValue = this->getReferencedValueCopy(state);

        // Search by ID first
        if (rawValue.isInteger(state))
        {
            ConsoleObject* idLookup = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(rawValue.toInteger(state));
            if (idLookup)
            {
                return idLookup;
            }
        }

        const std::string lookupName = rawValue.toString(state);
        return state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(lookupName);
    }

    bool StoredValue::isInteger(ExecutionState* state)
    {
        if (mReference)
        {
            return mReference->isInteger(state);
        }

        return mType == StoredValueType::Integer;
    }

    // In Torque, if we end up trying to set a value of ie. a float it does nothing
    bool StoredValue::setValue(const StoredValue& newValue, ExecutionState* state)
    {
        if (mReference)
        {
            return mReference->setValue(newValue, state);
        }
    
        std::string variableName;

        // Copy over stored data
        mType = newValue.mType;
        mStorage = newValue.mStorage;

        return true;
    }

    int StoredValue::toInteger(ExecutionState* state) const
    {
        if (mReference)
        {
            return mReference->toInteger(state);
        }

        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::Integer:
                return mStorage.mInteger;
            case StoredValueType::Float:
                return (int)mStorage.mFloat;
            case StoredValueType::String:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                try
                {
                    return std::stoi(stringValue);
                }
                catch (std::invalid_argument exception)
                {
                    return 0;
                }
        }

        throw std::runtime_error("Unknown Conversion");
    }

    std::string StoredValue::toString(ExecutionState* state)
    {
        if (mReference)
        {
            return mReference->toString(state);
        }

        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::Integer:
                return std::to_string(mStorage.mInteger);
            case StoredValueType::Float:
                return std::to_string(mStorage.mFloat);
            case StoredValueType::String:
                return state->mInterpreter->mStringTable.getString(mStorage.mStringID);
        }

        throw std::runtime_error("Unknown Conversion");
    }

    StoredValue StoredValue::getReferencedValueCopy(ExecutionState* state) const
    {
        if (mReference)
        {
            return mReference->getReferencedValueCopy(state);
        }

        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::Integer:
                return StoredValue(mStorage.mInteger);
            case StoredValueType::Float:
                return StoredValue(mStorage.mFloat);
            case StoredValueType::String:
                return StoredValue(mStorage.mStringID);
        }

        throw std::runtime_error("Unknown Conversion");
    }

    float StoredValue::toFloat(ExecutionState* state) const
    {
        if (mReference)
        {
            return mReference->toFloat(state);
        }

        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::Integer:
                return (float)mStorage.mInteger;
            case StoredValueType::MemoryReference:
                assert(mMemoryLocation);

                switch (mMemoryReferenceType)
                {
                    case MemoryReferenceType::FloatMemory:
                        return *(float*)mMemoryLocation;
                    default:
                        throw std::runtime_error("Unknown Memory Reference Type in toFloat");
                }
            case StoredValueType::Float:
                return mStorage.mFloat;
            case StoredValueType::String:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                try
                {
                    return std::stof(stringValue);
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

        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::Integer:
                return std::to_string(mStorage.mInteger);
            case StoredValueType::Float:
                return std::to_string(mStorage.mFloat);
            case StoredValueType::String:
                return std::to_string(mStorage.mStringID);
        }

        throw std::runtime_error("Unknown Conversion");
    }
}
