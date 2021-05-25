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

#include <sstream>

#include <torquescript/storedfloatvalue.hpp>

namespace TorqueScript
{
    StoredFloatValue::StoredFloatValue(float value) : mValue(value)
    {

    }

    float StoredFloatValue::toFloat(std::shared_ptr<ExecutionState> state)
    {
        return mValue;
    }

    int StoredFloatValue::toInteger(std::shared_ptr<ExecutionState> state)
    {
        return (int)mValue;
    }

    std::string StoredFloatValue::toString(std::shared_ptr<ExecutionState> state)
    {
        return std::to_string(mValue);
    }

    std::shared_ptr<StoredValue> StoredFloatValue::getReferencedValueCopy(std::shared_ptr<ExecutionState> state)
    {
        return std::shared_ptr<StoredValue>(new StoredFloatValue(mValue));
    }

    std::string StoredFloatValue::getRepresentation()
    {
        std::ostringstream result;
        result << "StoredFloatValue: " << mValue;
        return result.str();
    }
}
