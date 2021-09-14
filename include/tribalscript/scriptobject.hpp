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
#include <memory>
#include <unordered_map>

#include <tribalscript/consoleobject.hpp>
#include <tribalscript/executionscope.hpp>

namespace TribalScript
{
    /**
     *  @brief A FileObject is a class that may be used to interact with the filesystem
     *  provided by the PlatformContext configured with the interpreter.
     */
    class ScriptObject : public ConsoleObject
    {
        DECLARE_CONSOLE_OBJECT_BODY()

        public:
            explicit ScriptObject(Interpreter* interpreter, const std::string& className);

            std::string getVirtualClassName() override;

            static void initializeMemberFields(ConsoleObjectDescriptor* descriptor);

            static ConsoleObject* instantiateFromDescriptor(Interpreter* interpreter, ObjectInstantiationDescriptor& descriptor);

        protected:
            std::string mClassName;
    };
}

DECLARE_CONSOLE_OBJECT(ScriptObject, ConsoleObject)
