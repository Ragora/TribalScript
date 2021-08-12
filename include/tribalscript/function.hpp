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

#include <string>
#include <vector>
#include <memory>

#include <tribalscript/instructionsequence.hpp>

namespace TribalScript
{
    //! Forward declaration to avoid circular dependencies.
    class Interpreter;
    class Instruction;
    class CodeBlock;
    class ExecutionScope;
    class StoredValue;
    class StoredValueStack;
    class ExecutionState;
    class ConsoleObject;

    /**
     *  @brief A function is callable subroutine from anywhere in the language, defined by a script. A NativeFunction is a specialization
     *  of this that allows native C++ programming to be called from within the interpreter.
     */
    class Function
    {
        public:
            Function(const std::string& package, const std::string& space, const std::string& name);
            Function(const std::string& package, const std::string& space, const std::string& name, const std::vector<std::string>& parameterNames);

            void addInstructions(const InstructionSequence& instructions);

            /**
             *  @brief Default implementation will execute virtual instructions but can be overridden to implement native
             *  functions.
             */
            virtual void execute(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount);

            /**
             *  @brief Retrieves the declared name of this function.
             *  @return The name that this function was declared with.
             */
            const std::string& getDeclaredName();

            /**
             *  @brief Retrieves the declared namespace of this function.
             *  @return The namespace that this function was declared with.
             */
            const std::string& getDeclaredNameSpace();

            /**
             *  @brief Retrieves the declared package of this function.
             *  @return The package that this function was declared in.
             */
            const std::string& getDeclaredPackage();

        private:
            //! The package of the function.
            std::string mPackage;

            //! The namespace of the function.
            std::string mNameSpace;

            //! The name of the function.
            std::string mName;

            //! All instructions associated with this function.
            InstructionSequence mInstructions;

            std::vector<std::string> mParameterNames;
    };
}
