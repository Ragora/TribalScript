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
#include <vector>
#include <memory>

#include <torquescript/function.hpp>
#include <torquescript/simobject.hpp>
#include <torquescript/storedvalue.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/storedvaluestack.hpp>

namespace TorqueScript
{
    //! Forward declaration to deal with circular dependencies.
    class Compiler;
    class CodeBlock;

    /**
     *  @brief The interpreter class represents a high level instance of the TorqueScript interpreter.
     *  It is where execution control flow begins.
     */
    class Interpreter
    {
        public:
            Interpreter();
            ~Interpreter();

            void setGlobal(const std::string& name, std::shared_ptr<StoredValue> value);
            std::shared_ptr<StoredValue> getGlobal(const std::string& name);

            void setSimObject(const std::string& name, std::shared_ptr<SimObject> value);
            std::shared_ptr<SimObject> getSimObject(const std::string& name);

            CodeBlock* compile(const std::string& input);
            void evaluate(const std::string& input, StoredValueStack& stack);

            /**
             *  @brief Registers a new function to the interpreter. Ownership is transferred to the interpreter at this
             *  point.
             */
            void addFunction(std::shared_ptr<Function> function);

            std::shared_ptr<Function> getFunction(const std::string& name);

        private:
            //! Keep a ready instance of the compiler on hand as it is reusable.
            Compiler* mCompiler;

            //! A mapping of function names to the function object.
            std::map<std::string, std::shared_ptr<Function>> mFunctions;

            //! A mapping of global variable names to their stored value instance.
            std::map<std::string, std::shared_ptr<StoredValue>> mGlobalVariables;

            //! A mapping of object names to their sim objects
            std::map<std::string, std::shared_ptr<SimObject>> mSimObjects;
    };
}
