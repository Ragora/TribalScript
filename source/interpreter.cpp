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

#include <assert.h>

#include <torquescript/interpreter.hpp>
#include <torquescript/compiler.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/stringhelpers.hpp>

namespace TorqueScript
{
    Interpreter::Interpreter()
    {
        mCompiler = new Compiler();
    }

    Interpreter::~Interpreter()
    {
        assert(mCompiler);
        delete mCompiler;
    }

    void Interpreter::evaluate(const std::string& input, StoredValueStack& stack)
    {
        assert(mCompiler);

        // Spawn a compiler and generate a codeblock
        CodeBlock* compiled = this->compile(input);

        // FIXME: Some kind of error was encountered. Ask the compiler?
        if (!compiled)
        {
            return;
        }

        ExecutionScope scope;
        compiled->execute(this, &scope, stack);
    }

    CodeBlock* Interpreter::compile(const std::string& input)
    {
        return mCompiler->compileString(input);
    }

    std::shared_ptr<StoredValue> Interpreter::getGlobal(const std::string& name)
    {
        const std::string key = toLowerCase(name);

        auto search = mGlobalVariables.find(key);
        if (search != mGlobalVariables.end())
        {
            return search->second;
        }
        return nullptr;
    }

    void Interpreter::addFunction(std::shared_ptr<Function> function)
    {
        const std::string storedName = toLowerCase(function->getName());
        auto search = mFunctions.find(storedName);

        if (search != mFunctions.end())
        {
            mFunctions.erase(search);
        }

        mFunctions[storedName] = function;
    }

    std::shared_ptr<Function> Interpreter::getFunction(const std::string& name)
    {
        const std::string searchedName = toLowerCase(name);
        auto search = mFunctions.find(searchedName);

        if (search != mFunctions.end())
        {
            return search->second;
        }
        return nullptr;
    }

    void Interpreter::setGlobal(const std::string& name, std::shared_ptr<StoredValue> value)
    {
        const std::string key = toLowerCase(name);
        mGlobalVariables[key] = value;
    }
}
