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

#include <torquescript/codeblock.hpp>
#include <torquescript/instructions.hpp>
#include <torquescript/stringhelpers.hpp>

namespace TorqueScript
{
    void CodeBlock::addFunction(Function* function)
    {
        mFunctions.push_back(function);
    }

    void CodeBlock::addInstructions(const std::vector<Instruction*> instructions)
    {
        for (auto iterator = instructions.begin(); iterator != instructions.end(); ++iterator)
        {
            mInstructions.push_back(*iterator);
        }
    }

    void CodeBlock::execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack)
    {
        for (auto iterator = mInstructions.begin(); iterator != mInstructions.end(); ++iterator)
        {
            Instruction* instruction = *iterator;
            instruction->execute(interpreter, scope, stack);
        }
    }

    std::vector<std::string> CodeBlock::disassemble()
    {
        std::vector<std::string> result;

        for (auto iterator = mInstructions.begin(); iterator != mInstructions.end(); ++iterator)
        {
            Instruction* instruction = *iterator;
            result.push_back(instruction->disassemble());
        }
        return result;
    }

    std::vector<Function*> CodeBlock::getFunctions()
    {
        return mFunctions;
    }
}