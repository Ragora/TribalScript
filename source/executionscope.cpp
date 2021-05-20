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

        if (mExecutionScopeData.empty())
        {
            return nullptr;
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

        auto search = currentScope.mLocalVariables.find(lookup);
        if (search != currentScope.mLocalVariables.end())
        {
            return search->second;
        }

        return nullptr;
    }

    void ExecutionScope::setVariable(const std::string& name, std::shared_ptr<StoredValue> variable)
    {
        std::string key = toLowerCase(name);

        // Initialize if necessary
        if (mExecutionScopeData.empty())
        {
            this->pushFrame();
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        currentScope.mLocalVariables[key] = variable;
    }

    void ExecutionScope::pushFrame()
    {
        mExecutionScopeData.push_back(ExecutionScopeData());
    }

    void ExecutionScope::popFrame()
    {
        mExecutionScopeData.pop_back();
    }

    void ExecutionScope::pushLoop(const unsigned int pointer, const unsigned int depth)
    {
        // Initialize if necessary
        if (mExecutionScopeData.empty())
        {
            this->pushFrame();
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        currentScope.mLoopDescriptors.push_back(LoopDescriptor(pointer, depth));
    }

    LoopDescriptor ExecutionScope::popLoop()
    {
        LoopDescriptor result = this->currentLoopDescriptor();

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        currentScope.mLoopDescriptors.pop_back();
        return result;
    }

    LoopDescriptor ExecutionScope::currentLoopDescriptor()
    {
        assert(!mExecutionScopeData.empty());

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

        assert(!currentScope.mLoopDescriptors.empty());

        return currentScope.mLoopDescriptors.back();
    }
}
