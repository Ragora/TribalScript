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

#include <tribalscript/scriptobject.hpp>
#include <tribalscript/interpreter.hpp>

namespace TribalScript
{
    IMPLEMENT_CONSOLE_OBJECT(ScriptObject, ConsoleObject)

    ScriptObject::ScriptObject(Interpreter* interpreter, const std::string& className) : mClassName(className), ConsoleObject(interpreter)
    {
        if (className != "")
        {
            interpreter->registerConsoleObjectDescriptor(className, "ScriptObject", ScriptObject::instantiateFromDescriptor);
        }
    }

    std::string ScriptObject::getVirtualClassName()
    {
        return mClassName == "" ? "ScriptObject" : mClassName;
    }

    ConsoleObject* ScriptObject::instantiateFromDescriptor(Interpreter* interpreter, ObjectInstantiationDescriptor& descriptor)
    {
        std::string className = "";
        auto classNameSearch = descriptor.mFieldAssignments.find("class");
        if (classNameSearch != descriptor.mFieldAssignments.end())
        {
            className = classNameSearch->second.toString();
        }
        return new ScriptObject(interpreter, className);
    }

    void ScriptObject::initializeMemberFields(ConsoleObjectDescriptor* descriptor)
    {

    }
}
