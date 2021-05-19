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

#include <torquescript/storedfieldreferencevalue.hpp>
#include <torquescript/simobject.hpp>

namespace TorqueScript
{
    StoredFieldReferenceValue::StoredFieldReferenceValue(std::shared_ptr<SimObject> object, const std::string& name) : mSimObject(object), mName(name)
    {

    }

    float StoredFieldReferenceValue::toFloat(std::shared_ptr<ExecutionState> state)
    {
        assert(mSimObject);

        std::shared_ptr<StoredValue> referenced = mSimObject->getField(mName);
        if (referenced)
        {
            return referenced->toFloat(state);
        }
        return 0.0f;
    }

    std::string StoredFieldReferenceValue::toString(std::shared_ptr<ExecutionState> state)
    {
        assert(mSimObject);

        std::shared_ptr<StoredValue> referenced = mSimObject->getField(mName);
        if (referenced)
        {
            return referenced->toString(state);
        }
        return "";
    }

    int StoredFieldReferenceValue::toInteger(std::shared_ptr<ExecutionState> state)
    {
        assert(mSimObject);

        std::shared_ptr<StoredValue> referenced = mSimObject->getField(mName);
        if (referenced)
        {
            return referenced->toInteger(state);
        }
        return 0;
    }

    bool StoredFieldReferenceValue::setValue(std::shared_ptr<StoredValue> newValue, std::shared_ptr<ExecutionState> state)
    {
        assert(mSimObject);

        mSimObject->setField(mName, newValue->getReferencedValueCopy(state));
        return true;
    }

    std::shared_ptr<StoredValue> StoredFieldReferenceValue::getReferencedValueCopy(std::shared_ptr<ExecutionState> state)
    {
        assert(mSimObject);

        std::shared_ptr<StoredValue> referenced = mSimObject->getField(mName);
        assert(referenced);

        return referenced->getReferencedValueCopy(state);
    }
}
