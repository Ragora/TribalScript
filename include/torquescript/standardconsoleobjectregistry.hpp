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

#include <torquescript/consoleobjectregistrybase.hpp>
#include <torquescript/function.hpp>
#include <torquescript/consoleobject.hpp>
#include <torquescript/storedvalue.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/storedvaluestack.hpp>

namespace TorqueScript
{
    //! Forward declaration to deal with circular dependencies.
    class Compiler;
    class CodeBlock;
    class ExecutionState;

    class StandardConsoleObjectRegistry : public ConsoleObjectRegistryBase
    {
        public:
            StandardConsoleObjectRegistry();

            virtual void setConsoleObject(const std::string& name, ConsoleObject* value);
            virtual ConsoleObject* getConsoleObject(const std::string& name);
            virtual ConsoleObject* getConsoleObject(const unsigned int id);

            virtual std::string getConsoleObjectName(ConsoleObject* target);
            virtual unsigned int getConsoleObjectID(ConsoleObject* target);

            virtual unsigned int addConsoleObject(ConsoleObject* value);

            virtual void removeConsoleObject(const std::string& name);
            virtual void removeConsoleObject(ConsoleObject* target);

        private:
            unsigned int mNextObjectID;

            //! A mapping of object IDs to their sim objects
            std::unordered_map<unsigned int, ConsoleObject*> mConsoleObjectsByID;

            //! A mapping of object names to their sim objects
            std::unordered_map<std::string, ConsoleObject*> mConsoleObjectsByName;
    };
}
