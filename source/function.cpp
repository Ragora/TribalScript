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
    Function::Function(const std::string& name) : mName(name)
    {

    }

    Function::Function(const std::string& name, const std::vector<std::string>& parameterNames) : mName(name), mParameterNames(parameterNames)
    {

    }

    void Function::addInstructions(const std::vector<std::shared_ptr<Instruction>>& instructions)
    {
        for (auto iterator = instructions.begin(); iterator != instructions.end(); ++iterator)
        {
            mInstructions.push_back(*iterator);
        }
    }

    void Function::execute(std::shared_ptr<ExecutionState> state, const unsigned int argumentCount)
    {
        state->mExecutionScope.push();

        int instructionIndex = 0;

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
                state->mStack.pop_back();
            }
            adjustedArgumentCount -= removedParameters;
        }

        // Once we know what parameters we're providing for, set the values
        for (unsigned int iteration = 0; iteration < adjustedArgumentCount; ++iteration)
        {
            const std::string nextParameterName = mParameterNames[mParameterNames.size() - (iteration + emptyParameters + 1)];
            state->mExecutionScope.setVariable(nextParameterName, state->mStack.back());
            state->mStack.pop_back();
        }

        while (instructionIndex < mInstructions.size() && instructionIndex >= 0)
        {
            std::shared_ptr<Instruction> nextInstruction = mInstructions[instructionIndex];
            instructionIndex += nextInstruction->execute(state);
        }

        state->mExecutionScope.pop();
    }

    std::string Function::getName()
    {
        return mName;
    }
}
