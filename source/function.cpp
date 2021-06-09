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

#include <torquescript/function.hpp>
#include <torquescript/instructions.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/storedvaluestack.hpp>

namespace TorqueScript
{
    Function::Function(const std::string& package, const std::string& space, const std::string& name) : mPackage(toLowerCase(package)), mNameSpace(toLowerCase(space)), mName(toLowerCase(name))
    {

    }

    Function::Function(const std::string& package, const std::string& space, const std::string& name, const std::vector<std::string>& parameterNames) : mPackage(package), mNameSpace(space), mName(name), mParameterNames(parameterNames)
    {

    }

    void Function::addInstructions(const InstructionSequence& instructions)
    {
        for (auto iterator = instructions.begin(); iterator != instructions.end(); ++iterator)
        {
            mInstructions.push_back(*iterator);
        }
    }

    void Function::execute(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Calculate expected versus provided to determine what parameters should be left empty
        const unsigned int expectedParameterCount = mParameterNames.size();
        const unsigned int emptyParameters = expectedParameterCount > argumentCount ? expectedParameterCount - argumentCount : 0;

        // If we have too many parameters, just lop them off
        unsigned int adjustedArgumentCount = argumentCount;
        if (argumentCount > expectedParameterCount)
        {
            const unsigned int removedParameters = argumentCount - expectedParameterCount;

            for (unsigned int iteration = 0; iteration < removedParameters; ++iteration)
            {
                stack.pop_back();
            }
            adjustedArgumentCount -= removedParameters;
        }

        // Once we know what parameters we're providing for, set the values
        std::map<std::string, StoredValue> newLocals;
        for (unsigned int iteration = 0; iteration < adjustedArgumentCount; ++iteration)
        {
            const std::string nextParameterName = mParameterNames[mParameterNames.size() - (iteration + emptyParameters + 1)];

            auto search = newLocals.find(nextParameterName);
            if (search != newLocals.end())
            {
                search->second = stack.back().getReferencedValueCopy(state);
            }
            else
            {
                newLocals.insert(std::make_pair(nextParameterName, stack.back().getReferencedValueCopy(state)));
            }

            stack.pop_back();
        }

        // Once we've cleared the stack, check if we're at max recursion depth
        if (state->mInterpreter->mConfig.mMaxRecursionDepth > 0 && state->mExecutionScope.getFrameDepth() >= state->mInterpreter->mConfig.mMaxRecursionDepth)
        {
            state->mInterpreter->mConfig.mPlatform->logError("Reached maximum recursion depth! Pushing 0 and returning.");
            stack.push_back(StoredValue(0));
            return;
        }

        // Push scope once we're done dealing with locals and load in to current scope
        state->mExecutionScope.pushFrame(this);
        for (auto localIterator = newLocals.begin(); localIterator != newLocals.end(); ++localIterator)
        {
            auto currentLocal = *localIterator;
            state->mExecutionScope.setVariable(currentLocal.first, currentLocal.second);
        }

        mInstructions.execute(state);

        state->mExecutionScope.popFrame();
    }

    std::string Function::getName()
    {
        return mName;
    }

    std::string Function::getNameSpace()
    {
        return mNameSpace;
    }

    std::string Function::getPackage()
    {
        return mPackage;
    }
}
