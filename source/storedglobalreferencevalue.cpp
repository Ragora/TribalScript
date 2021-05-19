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

#include <torquescript/storedglobalreferencevalue.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/interpreter.hpp>

namespace TorqueScript
{
    StoredGlobalReferenceValue::StoredGlobalReferenceValue(const std::string& name) : mName(name)
    {

    }

    float StoredGlobalReferenceValue::toFloat(std::shared_ptr<ExecutionState> state)
    {
        std::shared_ptr<StoredValue> loaded = state->mInterpreter->getGlobal(mName);
        if (loaded)
        {
            return loaded->toFloat(state);
        }
        return 0.0f; // In Torque, if we're loading as a float but the variable does not exist we treat it as 0
    }

    int StoredGlobalReferenceValue::toInteger(std::shared_ptr<ExecutionState> state)
    {
        std::shared_ptr<StoredValue> loaded = state->mInterpreter->getGlobal(mName);
        if (loaded)
        {
            return loaded->toInteger(state);
        }
        return 0; // In Torque, if we're loading as a float but the variable does not exist we treat it as 0
    }

    std::string StoredGlobalReferenceValue::toString(std::shared_ptr<ExecutionState> state)
    {
        std::shared_ptr<StoredValue> loaded = state->mInterpreter->getGlobal(mName);

        if (loaded)
        {
            return loaded->toString(state);
        }
        return ""; // In Torque, if we're loading a string but the variable does not exist we treat it as ""
    }

    bool StoredGlobalReferenceValue::setValue(std::shared_ptr<StoredValue> value, std::shared_ptr<ExecutionState> state)
    {
        state->mInterpreter->setGlobal(mName, value);
        return true;
    }
}
