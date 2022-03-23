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

#include <tribalscript/codeblock.hpp>
#include <tribalscript/instructions.hpp>
#include <tribalscript/stringhelpers.hpp>
#include <tribalscript/instructionsequence.hpp>

namespace TribalScript
{
    CodeBlock::CodeBlock(const InstructionSequence& instructions, const std::vector<std::shared_ptr<Function>>& functions) : mFunctions(functions)
    {
        mInstructions.insert(mInstructions.end(), instructions.begin(), instructions.end());
    }

    void CodeBlock::execute(ExecutionState* state)
    {
        mInstructions.execute(this, state);
    }

    std::shared_ptr<Function> CodeBlock::getFunction(const std::size_t functionNumber)
    {
        return mFunctions[functionNumber];
    }

    std::vector<std::string> CodeBlock::disassembleInstructions(const std::string& prefix, InstructionSequence instructions)
    {
        std::vector<std::string> result;

        for (auto&& instruction : instructions)
        {
            std::ostringstream currentDisassembly;

            int iteration;
            std::vector<std::string> subdisassembly;

            switch (instruction.mInstruction)
            {
                case Instructions::InstructionType::RegisterFunction:
                    currentDisassembly << prefix << "RegisterFunction " << instruction.mOperands[0].toInteger() << std::endl;

                    // Lookup the function and dissassemble it
                    subdisassembly = disassembleInstructions(prefix + "  ", getFunction(instruction.mOperands[0].toInteger())->getInstructions());
                    for (iteration = 0; iteration < subdisassembly.size(); ++iteration)
                    {
                        currentDisassembly << subdisassembly[iteration] << std::endl;
                    }

                    break;

                case Instructions::InstructionType::Add:
                    currentDisassembly << prefix << "Add";
                    break;

                case Instructions::InstructionType::Minus:
                    currentDisassembly << prefix << "Minus";
                    break;

                case Instructions::InstructionType::Equals:
                    currentDisassembly << prefix << "Equals";
                    break;

                case Instructions::InstructionType::NotEquals:
                    currentDisassembly << prefix << "NotEquals";
                    break;

                case Instructions::InstructionType::Not:
                    currentDisassembly << prefix << "Not";
                    break;

                case Instructions::InstructionType::StringEquals:
                    currentDisassembly << prefix << "StringEquals";
                    break;

                case Instructions::InstructionType::StringNotEquals:
                    currentDisassembly << prefix << "StringEquals";
                    break;

                case Instructions::InstructionType::LessThan:
                    currentDisassembly << prefix << "LessThan";
                    break;

                case Instructions::InstructionType::AddAssignment:
                    currentDisassembly << prefix << "AddAssignment";
                    break;

                case Instructions::InstructionType::PushInteger:
                    currentDisassembly << prefix << "PushInteger " << instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::PushString:
                    currentDisassembly << prefix << "PushString " << instruction.mOperands[0].toString();
                    break;

                case Instructions::InstructionType::Assignment:
                    currentDisassembly << prefix << "Assignment";
                    break;

                case Instructions::InstructionType::Negate:
                    currentDisassembly << prefix << "Negate";
                    break;

                case Instructions::InstructionType::NOP:
                    currentDisassembly << prefix << "NOP";
                    break;

                case Instructions::InstructionType::PushFloat:
                    currentDisassembly << prefix << "PushFloat " << instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::Jump:
                    currentDisassembly << prefix << "Jump " <<  instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::JumpFalse:
                    currentDisassembly << prefix << "JumpFalse " << instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::JumpTrue:
                    currentDisassembly << prefix << "JumpTrue " << instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::CallFunction:
                    currentDisassembly << prefix << "CallFunction " << instruction.mOperands[0].toString() << "::" << instruction.mOperands[1].toString() << " Argc " << instruction.mOperands[2].toInteger();
                    break;

                case Instructions::InstructionType::PushLocalReference:
                    currentDisassembly << prefix << "PushLocalReference " << instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::PushGlobalReference:
                    currentDisassembly << prefix << "PushGlobalReference " << instruction.mOperands[0].toInteger();
                    break;

                case Instructions::InstructionType::Return:
                    currentDisassembly << prefix << "Return";
                    break;

                case Instructions::InstructionType::Concat:
                    currentDisassembly << prefix << "Concat " << instruction.mOperands[0].toString();
                    break;

                default:
                    currentDisassembly << prefix << "Unknown OpCode " << instruction.mInstruction;
                    break;
            }
            result.push_back(currentDisassembly.str());
        }

        return result;
    }

    std::vector<std::string> CodeBlock::disassemble()
    {
        return disassembleInstructions("", mInstructions);
    }
}
