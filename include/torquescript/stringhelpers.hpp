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

#include <sstream>

#include <torquescript/storedvaluestack.hpp>

namespace TorqueScript
{
    std::string toLowerCase(const std::string& in);
    std::string expandEscapeSequences(const std::string& in);

    static std::string resolveArrayNameFromStack(StoredValueStack& stack, ExecutionState* state, const std::string& base, const std::size_t argumentCount)
    {
        std::vector<std::string> variableComponents;
        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            variableComponents.push_back(stack.popString(state));
        }

        std::ostringstream out;
        out << base;
        for (auto iterator = variableComponents.rbegin(); iterator != variableComponents.rend(); ++iterator)
        {
            if (iterator != variableComponents.rbegin())
            {
                out << "_";
            }
            out << *iterator;
        }

        return out.str();
    }

    static std::string resolveArrayName(const std::string& base)
    {
        return base;
    }

    template <typename... parameters>
    static std::string resolveArrayName(const std::string& base, const int value, parameters... params)
    {
        return TorqueScript::resolveArrayName(base + "_" + std::to_string(value), params...);
    }

    template <typename... parameters>
    static std::string resolveArrayName(const std::string& base, const float value, parameters... params)
    {
        return TorqueScript::resolveArrayName(base + "_" + std::to_string(value), params...);
    }

}
