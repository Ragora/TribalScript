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
    bool StoredValue::toBoolean(std::shared_ptr<ExecutionState> state)
    {
        return this->toInteger(state) != 0;
    }

    std::shared_ptr<ConsoleObject> StoredValue::toConsoleObject(std::shared_ptr<ExecutionState> state)
    {
        // Search by ID first
        if (this->isInteger(state))
        {
            std::shared_ptr<ConsoleObject> idLookup = state->mInterpreter->mConsoleObjectRegistry.getConsoleObject(this->toInteger(state));
            if (idLookup)
            {
                return idLookup;
            }
        }

        const std::string lookupName = this->toString(state);
        return state->mInterpreter->mConsoleObjectRegistry.getConsoleObject(lookupName);
    }

    bool StoredValue::isInteger(std::shared_ptr<ExecutionState> state)
    {
        return mType == StoredValueType::Integer;
    }

    // In Torque, if we end up trying to set a value of ie. a float it does nothing
    bool StoredValue::setValue(StoredValue newValue, std::shared_ptr<ExecutionState> state)
    {
        std::string variableName;

        switch (mType)
        {
            case StoredValueType::LocalReference:
                variableName = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                state->mExecutionScope.setVariable(variableName, newValue.getReferencedValueCopy(state));
                return true;
            case StoredValueType::GlobalReference:
                variableName = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                state->mInterpreter->setGlobal(variableName, newValue.getReferencedValueCopy(state));
                return true;
            case StoredValueType::SubfieldReference:
                assert(mConsoleObject);

                variableName = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                mConsoleObject->setTaggedField(variableName, newValue.getReferencedValueCopy(state));
                return true;
        }
        return false;
    }

    int StoredValue::toInteger(std::shared_ptr<ExecutionState> state)
    {
        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::LocalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mExecutionScope.getVariable(stringValue);
                if (referenced)
                {
                    return referenced->toInteger(state);
                }
                return 0;
            case StoredValueType::GlobalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mInterpreter->getGlobal(stringValue);
                if (referenced)
                {
                    return referenced->toInteger(state);
                }
                return 0;
            case StoredValueType::SubfieldReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                assert(mConsoleObject);

                referenced = mConsoleObject->getTaggedField(stringValue);
                if (referenced)
                {
                    return referenced->toInteger(state);
                }
                return 0;
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

    std::string StoredValue::toString(std::shared_ptr<ExecutionState> state)
    {
        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::LocalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mExecutionScope.getVariable(stringValue);
                if (referenced)
                {
                    return referenced->toString(state);
                }
                return "";
            case StoredValueType::GlobalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mInterpreter->getGlobal(stringValue);
                if (referenced)
                {
                    return referenced->toString(state);
                }
                return "";
            case StoredValueType::SubfieldReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                assert(mConsoleObject);

                referenced = mConsoleObject->getTaggedField(stringValue);
                if (referenced)
                {
                    return referenced->toString(state);
                }
                return "";
            case StoredValueType::Integer:
                return std::to_string(mStorage.mInteger);
            case StoredValueType::Float:
                return std::to_string(mStorage.mFloat);
            case StoredValueType::String:
                return state->mInterpreter->mStringTable.getString(mStorage.mStringID);
        }

        throw std::runtime_error("Unknown Conversion");
    }

    StoredValue StoredValue::getReferencedValueCopy(std::shared_ptr<ExecutionState> state)
    {
        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::LocalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);

                referenced = state->mExecutionScope.getVariable(stringValue);
                if (referenced)
                {
                    return referenced->getReferencedValueCopy(state);
                }
				break;
            case StoredValueType::GlobalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mInterpreter->getGlobal(stringValue);
                if (referenced)
                {
                    return referenced->getReferencedValueCopy(state);
                }
				break;
            case StoredValueType::SubfieldReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = mConsoleObject->getTaggedField(stringValue);
                if (referenced)
                {
                    return referenced->getReferencedValueCopy(state);
                }
                break;
            case StoredValueType::Integer:
                return StoredValue(mStorage.mInteger);
            case StoredValueType::Float:
                return StoredValue(mStorage.mFloat);
            case StoredValueType::String:
                return StoredValue(mStorage.mStringID, StoredValueType::String);
        }

        throw std::runtime_error("Unknown Conversion");
    }

    float StoredValue::toFloat(std::shared_ptr<ExecutionState> state)
    {
        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::LocalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mExecutionScope.getVariable(stringValue);
                if (referenced)
                {
                    return referenced->toFloat(state);
                }
                return 0;
            case StoredValueType::GlobalReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                referenced = state->mInterpreter->getGlobal(stringValue);
                if (referenced)
                {
                    return referenced->toFloat(state);
                }
                return 0;
            case StoredValueType::SubfieldReference:
                stringValue = state->mInterpreter->mStringTable.getString(mStorage.mStringID);
                assert(mConsoleObject);

                referenced = mConsoleObject->getTaggedField(stringValue);
                if (referenced)
                {
                    return referenced->toFloat(state);
                }
                return 0;
            case StoredValueType::Integer:
                return (float)mStorage.mInteger;
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
        StoredValue* referenced;
        std::string stringValue;

        switch (mType)
        {
            case StoredValueType::LocalReference:
                return "LocalReference";
            case StoredValueType::GlobalReference:
                return "GlobalReference";
            case StoredValueType::SubfieldReference:
                return "SubfieldReference";
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
