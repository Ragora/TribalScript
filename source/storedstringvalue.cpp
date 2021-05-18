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

#include <torquescript/storedstringvalue.hpp>

namespace TorqueScript
{
    StoredStringValue::StoredStringValue(const std::string& value) : mValue(value)
    {

    }

    float StoredStringValue::toFloat(ExecutionState* state)
    {
        try
        {
            return std::stof(mValue);
        }
        catch (std::invalid_argument exception)
        {
            return 0.0f; // In Torque if the value cannot be converted it is treated as zero
        }
    }

    std::string StoredStringValue::toString(ExecutionState* state)
    {
        return mValue;
    }

    int StoredStringValue::toInteger(ExecutionState* state)
    {
        try
        {
            return std::stoi(mValue);
        }
        catch (std::invalid_argument exception)
        {
            return 0; // In Torque if the value cannot be converted it is treated as zero
        }
    }
}
