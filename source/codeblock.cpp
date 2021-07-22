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
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
{
    CodeBlock::CodeBlock(const InstructionSequence& instructions, const std::vector<std::shared_ptr<Function>>& functions)
    {
        mFunctions.insert(mFunctions.end(), functions.begin(), functions.end());
        mInstructions.insert(mInstructions.end(), instructions.begin(), instructions.end());
    }

    void CodeBlock::execute(ExecutionState* state)
    {
        mInstructions.execute(state);
    }

    const std::vector<std::shared_ptr<Function>>& CodeBlock::getFunctions()
    {
        return mFunctions;
    }

    std::vector<std::string> CodeBlock::disassemble()
    {
        std::vector<std::string> result;

        /*
        for (auto&& instruction : mInstructions)
        {
            std::ostringstream out;
            out << instruction->disassemble();

            if (instruction->mComment != "")
            {
                out << " // " << instruction->mComment;
            }
            result.push_back(out.str());
        }
        */
        return result;
    }
}
