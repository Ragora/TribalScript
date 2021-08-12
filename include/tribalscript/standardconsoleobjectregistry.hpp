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

#include <tribalscript/consoleobjectregistrybase.hpp>
#include <tribalscript/function.hpp>
#include <tribalscript/consoleobject.hpp>
#include <tribalscript/storedvalue.hpp>
#include <tribalscript/stringhelpers.hpp>
#include <tribalscript/storedvaluestack.hpp>

namespace TribalScript
{
    //! Forward declaration to deal with circular dependencies.
    class Compiler;
    class CodeBlock;
    class ExecutionState;

    class StandardConsoleObjectRegistry : public ConsoleObjectRegistryBase
    {
        public:
            StandardConsoleObjectRegistry();

            void setConsoleObject(const std::string& name, ConsoleObject* value) override;
            ConsoleObject* getConsoleObject(const std::string& name) override;
            ConsoleObject* getConsoleObject(const unsigned int id) override;

            std::string getConsoleObjectName(ConsoleObject* target) override;
            unsigned int getConsoleObjectID(ConsoleObject* target) override;

            unsigned int addConsoleObject(ConsoleObject* value) override;

            void removeConsoleObject(const std::string& name) override;
            void removeConsoleObject(ConsoleObject* target) override;

        private:
            unsigned int mNextObjectID;

            //! A mapping of object IDs to their sim objects
            std::unordered_map<unsigned int, ConsoleObject*> mConsoleObjectsByID;

            //! A mapping of object names to their sim objects
            std::unordered_map<std::string, ConsoleObject*> mConsoleObjectsByName;
    };
}
