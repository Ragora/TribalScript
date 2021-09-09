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
#include <memory>
#include <unordered_map>

#include <tribalscript/function.hpp>
#include <tribalscript/consoleobject.hpp>
#include <tribalscript/storedvalue.hpp>
#include <tribalscript/stringhelpers.hpp>
#include <tribalscript/storedvaluestack.hpp>

#define NAMESPACE_EMPTY ""
#define PACKAGE_EMPTY ""

namespace TribalScript
{
    //! Forward declaration to deal with circular dependencies.
    class Compiler;
    class CodeBlock;
    class ExecutionState;

    class ConsoleObjectRegistryBase
    {
        public:
            virtual void setConsoleObject(Interpreter* interpreter, const std::string& name, ConsoleObject* value) = 0;
            virtual ConsoleObject* getConsoleObject(Interpreter* interpreter, const std::string& name) = 0;
            virtual ConsoleObject* getConsoleObject(Interpreter* interpreter, const unsigned int id) = 0;

            virtual std::string getConsoleObjectName(Interpreter* interpreter, ConsoleObject* target) = 0;
            virtual unsigned int getConsoleObjectID(Interpreter* interpreter, ConsoleObject* target) = 0;

            virtual unsigned int addConsoleObject(Interpreter* interpreter, ConsoleObject* value) = 0;

            virtual void removeConsoleObject(Interpreter* interpreter, const std::string& name) = 0;
            virtual void removeConsoleObject(Interpreter* interpreter, ConsoleObject* target) = 0;
    };
}
