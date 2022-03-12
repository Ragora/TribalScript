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

#include <tribalscript/storedvalue.hpp>
#include <tribalscript/interpreter.hpp>
#include <tribalscript/consoleobject.hpp>
#include <tribalscript/executionstate.hpp>

namespace TribalScript
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
            ConsoleObject* idLookup = state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(state->mInterpreter, rawValue.toInteger());
            if (idLookup)
            {
                return idLookup;
            }
        }

        const std::string lookupName = rawValue.toString();
        return state->mInterpreter->mConfig.mConsoleObjectRegistry->getConsoleObject(state->mInterpreter, lookupName);
    }

    bool StoredValue::isInteger()
    {
        if (mReference)
        {
            return mReference->isInteger();
        }

        return mType == StoredValueType::Integer;
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
            }
        }

        mType = StoredValueType::Float;
        mStorage.mFloat = newValue;
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
    }
}
