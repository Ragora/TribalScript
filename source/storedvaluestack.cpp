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

#include <torquescript/storedvaluestack.hpp>

namespace TorqueScript
{
    int StoredValueStack::popInteger(ExecutionScope* scope)
    {
        if (this->empty())
        {
            return 0;
        }
        std::shared_ptr<StoredValue> currentValue = this->back();
        int result = currentValue->toInteger(scope);
        this->pop_back();
        return result;
    }

    std::string StoredValueStack::popString(ExecutionScope* scope)
    {
        if (this->empty())
        {
            return "";
        }
        std::shared_ptr<StoredValue> currentValue = this->back();
        std::string result = currentValue->toString(scope);
        this->pop_back();
        return result;
    }

    float StoredValueStack::popFloat(ExecutionScope* scope)
    {
        if (this->empty())
        {
            return 0.0f;
        }
        std::shared_ptr<StoredValue> currentValue = this->back();
        float result = currentValue->toFloat(scope);
        this->pop_back();
        return result;
    }
}