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

#include <tribalscript/instructionsequence.hpp>
#include <tribalscript/executionstate.hpp>
#include <tribalscript/instructions.hpp>

namespace TribalScript
{
    void InstructionSequence::execute(ExecutionState* state)
    {
        AddressType instructionIndex = 0;

        const std::size_t instructionCount = this->size();
        while (instructionIndex < instructionCount && instructionIndex >= 0)
        {
            Instructions::Instruction* nextInstruction = this->at(instructionIndex);

            state->mInstructionPointer = instructionIndex;
            const AddressOffsetType advance = nextInstruction->execute(state);
            if (advance == 0)
            {
                break;
            }
            instructionIndex += advance;
        }
    }
}
