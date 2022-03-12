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

#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>

#include <tribalscript/storedvalue.hpp>

namespace TribalScript
{
    namespace Instructions
    {
        enum InstructionType
        {
            PushFloat,
            PushInteger,
            PushLocalReference,
            PushString,
            PushGlobalReference,
            AddAssignment,
            Assignment,
            Concat,
            Negate,
            Not,
            CallFunction, // 10
            LogicalAnd,
            LogicalOr,
            Add,
            Minus,
            Modulus,
            LessThan,
            GreaterThan,
            GreaterThanOrEqual,
            Equals,
            NotEquals, // 20
            StringEquals,
            StringNotEquals,
            BitwiseAnd,
            BitwiseOr,
            Multiply,
            Divide,
            Pop,
            Jump,
            JumpTrue,
            JumpFalse, // 30
            NOP,
            RegisterFunction, // By ID in the table
            Subreference,
            Return,
            Break,
            Continue,
            AccessArray,
            CallBoundFunction,
            PushObjectInstantiation,
            PushObjectField,
            PopObjectField,
            PopObjectInstantiation
        };

        /**
         *  @brief Base instruction class. All instructions in the interpreter should dervive
         *  from this class and implement all virtual members.
         */
        class Instruction
        {
            public:
                InstructionType mInstruction;

                StoredValue mOperands[4];

                Instruction(const InstructionType instruction) : mInstruction(instruction)
                {

                }

                Instruction(const InstructionType instruction, const StoredValue& operandOne) : mInstruction(instruction)
                {
                    mOperands[0] = operandOne;
                }

                Instruction(const InstructionType instruction, const StoredValue& operandOne, const StoredValue& operandTwo) : mInstruction(instruction)
                {
                    mOperands[0] = operandOne;
                    mOperands[1] = operandTwo;
                }

                Instruction(const InstructionType instruction, const StoredValue& operandOne, const StoredValue& operandTwo, const StoredValue& operandThree) : mInstruction(instruction)
                {
                    mOperands[0] = operandOne;
                    mOperands[1] = operandTwo;
                    mOperands[2] = operandThree;
                }

                Instruction(const InstructionType instruction, const StoredValue& operandOne, const StoredValue& operandTwo, const StoredValue& operandThree, const StoredValue& operandFour) : mInstruction(instruction)
                {
                    mOperands[0] = operandOne;
                    mOperands[1] = operandTwo;
                    mOperands[2] = operandThree;
                    mOperands[3] = operandFour;
                }
        };
    }
}
