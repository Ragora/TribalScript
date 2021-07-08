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
    ExecutionScope::ExecutionScope(const InterpreterConfiguration& config, StringTable* table) : mConfig(config), mStringTable(table)
    {
        this->pushFrame(nullptr);
    }

    StoredValue* ExecutionScope::getVariable(const StringTableEntry name)
    {
        if (mExecutionScopeData.empty())
        {
            return nullptr;
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

        auto search = currentScope.mLocalVariables.find(name);
        if (search != currentScope.mLocalVariables.end())
        {
            return search->second;
        }

        return nullptr;
    }

    StoredValue* ExecutionScope::getVariableOrAllocate(const StringTableEntry name)
    {
        if (mExecutionScopeData.empty())
        {
            return nullptr;
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

        auto search = currentScope.mLocalVariables.find(name);
        if (search != currentScope.mLocalVariables.end())
        {
            return search->second;
        }

        StoredValue* newValue = new StoredValue(0);
        currentScope.mLocalVariables.insert(std::make_pair(name, newValue));
        return newValue;
    }

    StoredValue* ExecutionScope::getVariable(const std::string& name)
    {
        const StringTableEntry lookup = mStringTable->getOrAssign(mConfig.mCaseSensitive ? name : toLowerCase(name));

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

    void ExecutionScope::setVariable(const StringTableEntry name, const StoredValue& variable)
    {
        // Initialize if necessary
        if (mExecutionScopeData.empty())
        {
            this->pushFrame(nullptr);
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

        auto search = currentScope.mLocalVariables.find(name);
        if (search != currentScope.mLocalVariables.end())
        {
            search->second->setValue(variable);
            return;
        }

        currentScope.mLocalVariables.insert(std::make_pair(name, new StoredValue(variable)));
    }

    void ExecutionScope::setVariable(const std::string& name, const StoredValue& variable)
    {
        const StringTableEntry key = mStringTable->getOrAssign(mConfig.mCaseSensitive ? name : toLowerCase(name));

        // Initialize if necessary
        if (mExecutionScopeData.empty())
        {
            this->pushFrame(nullptr);
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        auto search = currentScope.mLocalVariables.find(key);
        if (search != currentScope.mLocalVariables.end())
        {
            search->second->setValue(variable);
            return;
        }
        currentScope.mLocalVariables.insert(std::make_pair(key, new StoredValue(variable)));
    }

    void ExecutionScope::pushFrame(Function* function)
    {
        mExecutionScopeData.push_back(ExecutionScopeData(function));
    }

    void ExecutionScope::popFrame()
    {
        mExecutionScopeData.pop_back();
    }

    void ExecutionScope::pushLoop(const AddressType pointer, const std::size_t depth)
    {
        // Initialize if necessary
        if (mExecutionScopeData.empty())
        {
            this->pushFrame(nullptr);
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

    bool ExecutionScope::isLoopStackEmpty()
    {
        if (mExecutionScopeData.empty())
        {
            return true;
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        return currentScope.mLoopDescriptors.empty();
    }

    std::size_t ExecutionScope::getFrameDepth()
    {
        return mExecutionScopeData.size();
    }

    Function* ExecutionScope::getCurrentFunction()
    {
        if (mExecutionScopeData.empty())
        {
            return nullptr;
        }

        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        return currentScope.mCurrentFunction;
    }

    StoredValueStack& ExecutionScope::getStack()
    {
        ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();
        return currentScope.mStack;
    }

    StoredValueStack& ExecutionScope::getReturnStack()
    {
        ExecutionScopeData& currentScope = *(mExecutionScopeData.rbegin() + 1);
        return currentScope.mStack;
    }

    bool ExecutionScope::isAwaitingParentInstantiation()
    {
        ExecutionScopeData& currentScope = *(mExecutionScopeData.rbegin());
        return currentScope.mObjectInstantiations.size() != 0;
    }

    void ExecutionScope::pushObjectInstantiation(const std::string& typeName, const std::string& name)
    {
        ExecutionScopeData& currentScope = *(mExecutionScopeData.rbegin());
        currentScope.mObjectInstantiations.push_back(ObjectInstantiationDescriptor(typeName, name));
    }

    ObjectInstantiationDescriptor ExecutionScope::popObjectInstantiation()
    {
        ExecutionScopeData& currentScope = *(mExecutionScopeData.rbegin());
        ObjectInstantiationDescriptor result = currentScope.mObjectInstantiations.back();
        currentScope.mObjectInstantiations.pop_back();
        return result;
    }

    ObjectInstantiationDescriptor& ExecutionScope::currentObjectInstantiation()
    {
        ExecutionScopeData& currentScope = *(mExecutionScopeData.rbegin());
        return currentScope.mObjectInstantiations.back();
    }
}
