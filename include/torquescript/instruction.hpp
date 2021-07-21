#pragma once

namespace TorqueScript
{
    //! Typedef for the signed integer type to be used when calculating addresses.
    typedef long long int AddressType;
    typedef unsigned long long int AddressOffsetType;

    class ExecutionState;

    namespace Instructions
    {
        class Instruction;

        typedef AddressOffsetType (*OpcodeResolutionPointer)(ExecutionState* state, Instruction* instruction);

#define INSTRUCTION_PARAMETER_COUNT 4
        union InstructionParameters
        {
            int mInteger;
            float mFloat;
            AddressType mOffset;
            std::size_t mStringID;
        };

        class Instruction
        {
        public:
            Instruction(OpcodeResolutionPointer op) : mOp(op)
            {

            }

            Instruction(const Instruction& copied) : mOp(copied.mOp)
            {
                // , mParameters(copied.mParameters)
                memcpy(mParameters, copied.mParameters, sizeof(InstructionParameters) * INSTRUCTION_PARAMETER_COUNT);
            }

            OpcodeResolutionPointer mOp;

            InstructionParameters mParameters[INSTRUCTION_PARAMETER_COUNT];
        };

    }
}
