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

#include <map>
#include <string>
#include <vector>

#include <torquescript/instructions.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/function.hpp>
#include <torquescript/storedvariable.hpp>

namespace TorqueScript
{
    /**
     *  @brief A CodeBlock defines a piece of executable code generated from a single input (Ie. a file).
     *  This includes global executable code and subroutines, datablocks, etc.
     */
    class CodeBlock
    {
        public:
            /**
             *  @brief Registers a new function within this codeblock.
             *  @param function The function to be added.
             */
            void addFunction(Function* function);

            /**
             *  @brief Registers the instruction sequence to the codeblock to be executed
             *  immediately.
             *  @param instructions The instructions to execute immediately.
             */
            void addInstructions(const std::vector<Instruction*> instructions);

            /**
             *  @brief Executes all instructions contained in mInstructions within the provided context.
             */
            void execute(Interpreter* interpreter, ExecutionScope* scope, std::vector<StoredVariable*>& stack);

            /**
             *  @brief Produces a disassembly of the global CodeBlock code.
             */
            std::vector<std::string> disassemble();

            std::vector<Function*> getFunctions();

        private:
            //! All functions registered in this codeblock.
            std::vector<Function*> mFunctions;

            //! All instructions that were generated global to the block - ie. should be executed immediately
            std::vector<Instruction*> mInstructions;
    };
}