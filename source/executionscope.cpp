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

#include <torquescript/executionscope.hpp>

namespace TorqueScript
{
    std::shared_ptr<StoredValue> ExecutionScope::getVariable(const std::string& name)
    {
        std::string lookup = toLowerCase(name);

        if (mLocalVariables.empty())
        {
            return nullptr;
        }

        // Here we search the scopes in reverse to find the first match for reference
        for (auto iterator = mLocalVariables.rbegin(); iterator != mLocalVariables.rend(); ++iterator)
        {
            std::map<std::string, std::shared_ptr<StoredValue>>& currentScope = *iterator;

            auto search = currentScope.find(lookup);
            if (search != currentScope.end())
            {
                return search->second;
            }
        }

        return nullptr;
    }

    void ExecutionScope::setVariable(const std::string& name, std::shared_ptr<StoredValue> variable)
    {
        std::string key = toLowerCase(name);

        // Initialize if necessary
        if (mLocalVariables.empty())
        {
            this->push();
        }

        std::map<std::string, std::shared_ptr<StoredValue>>& currentScope = mLocalVariables.back();
        currentScope[key] = variable;
    }

    void ExecutionScope::push()
    {
        mLocalVariables.push_back(std::map<std::string, std::shared_ptr<StoredValue>>());
    }

    void ExecutionScope::pop()
    {
        mLocalVariables.pop_back();
    }
}
