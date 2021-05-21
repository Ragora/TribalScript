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
#include <torquescript/executionstate.hpp>

namespace TorqueScript
{
    Interpreter::Interpreter()
    {
        mCompiler = new Compiler();
        mMaxRecursionDepth = 1024;
    }

    Interpreter::~Interpreter()
    {
        assert(mCompiler);
        delete mCompiler;
    }

    void Interpreter::evaluate(const std::string& input, std::shared_ptr<ExecutionState> state)
    {
        assert(mCompiler);

        // Spawn a compiler and generate a codeblock
        CodeBlock* compiled = this->compile(input);

        // FIXME: Some kind of error was encountered. Ask the compiler?
        if (!compiled)
        {
            return;
        }

        state = state ? state : this->getExecutionState();
        compiled->execute(state);
    }

    void Interpreter::execute(const std::string& path, std::shared_ptr<ExecutionState> state)
    {
        CodeBlock* compiled = mCompiler->compileFile(path);

        // FIXME: Some kind of error was encountered. Ask the compiler?
        if (!compiled)
        {
            return;
        }

        state = state ? state : this->getExecutionState();
        compiled->execute(state);
    }

    std::shared_ptr<ExecutionState> Interpreter::getExecutionState()
    {
        return std::shared_ptr<ExecutionState>(new ExecutionState(this));
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

    void Interpreter::setSimObject(const std::string& name, std::shared_ptr<SimObject> value)
    {
        const std::string setName = toLowerCase(name);
        mSimObjects[setName] = value;
    }

    std::shared_ptr<SimObject> Interpreter::getSimObject(const std::string& name)
    {
        const std::string searchedName = toLowerCase(name);
        auto search = mSimObjects.find(searchedName);

        if (search != mSimObjects.end())
        {
            return search->second;
        }
        return nullptr;
    }

    void Interpreter::logEcho(const std::string& message)
    {
        std::cout << "Echo > " << message << std::endl;
    }

    void Interpreter::logError(const std::string& message)
    {
        std::cerr << "Error > " << message << std::endl;
    }

    void Interpreter::logWarning(const std::string& message)
    {
        std::cout << "Warning > " << message << std::endl;
    }
}
