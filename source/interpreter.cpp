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

        // "" is the default top-level
        this->addFunctionRegistry(PACKAGE_EMPTY);
        this->activateFunctionRegistry(PACKAGE_EMPTY);
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
        CodeBlock* compiled = mCompiler->compileFile(path, &mStringTable);

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
        return mCompiler->compileString(input, &mStringTable);
    }

    StoredValue* Interpreter::getGlobal(const std::string& name)
    {
        const std::string key = toLowerCase(name);

        auto search = mGlobalVariables.find(key);
        if (search != mGlobalVariables.end())
        {
            return &search->second;
        }
        return nullptr;
    }

    void Interpreter::addFunction(std::shared_ptr<Function> function)
    {
        std::hash<std::string> stringHasher;

        // Make sure the registry exists - if it already does this does nothing
        std::string package = function->getPackage();
        this->addFunctionRegistry(package);
        FunctionRegistry* registry = this->findFunctionRegistry(package);

        const std::size_t storedName = stringHasher(toLowerCase(function->getName()));
        const std::size_t storedNameSpace = stringHasher(toLowerCase(function->getNameSpace()));
        registry->mFunctions[storedNameSpace][storedName] = function;
    }

    std::shared_ptr<Function> Interpreter::getFunction(const std::string& space, const std::string& name)
    {
        // Search registries back to front
        std::hash<std::string> stringHasher;
        const std::size_t searchedName = stringHasher(toLowerCase(name));
        const std::size_t searchedNameSpace = stringHasher(toLowerCase(space));

        for (auto iterator = mFunctionRegistries.rbegin(); iterator != mFunctionRegistries.rend(); ++iterator)
        {
            FunctionRegistry& registry = *iterator;

            if (registry.mActive)
            {
                auto namespaceSearch = registry.mFunctions.find(searchedNameSpace);
                if (namespaceSearch != registry.mFunctions.end())
                {
                    auto nameSearch = namespaceSearch->second.find(searchedName);
                    if (nameSearch != namespaceSearch->second.end())
                    {
                        return nameSearch->second;
                    }
                }
            }
        }

        return nullptr;
    }

    std::shared_ptr<Function> Interpreter::getFunctionParent(Function* function)
    {
        std::hash<std::string> stringHasher;

        const std::string searchedPackage = toLowerCase(function->getPackage());
        const std::size_t searchedNameSpace = stringHasher(toLowerCase(function->getNameSpace()));
        const std::size_t searchedFunction = stringHasher(toLowerCase(function->getName()));

        // Search registries back to front
        bool shouldSearchFunction = false;
        for (auto iterator = mFunctionRegistries.rbegin(); iterator != mFunctionRegistries.rend(); ++iterator)
        {
            FunctionRegistry& registry = *iterator;
            // Ignore inactive registries
            if (!registry.mActive)
            {
                continue;
            }
            else if (!shouldSearchFunction)
            {
                if (registry.mActive && registry.mPackageName == searchedPackage)
                {
                    shouldSearchFunction = true;
                }
                continue;
            }

            auto namespaceSearch = registry.mFunctions.find(searchedNameSpace);
            if (namespaceSearch != registry.mFunctions.end())
            {
                auto nameSearch = namespaceSearch->second.find(searchedFunction);
                if (nameSearch != namespaceSearch->second.end())
                {
                    return nameSearch->second;
                }
            }
        }

        return nullptr;
    }

    void Interpreter::setGlobal(const std::string& name, StoredValue value)
    {
        const std::string key = toLowerCase(name);
        mGlobalVariables[key] = value;
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

    FunctionRegistry* Interpreter::findFunctionRegistry(const std::string packageName)
    {
        std::string searchedName = toLowerCase(packageName);
        for (FunctionRegistry& registry : mFunctionRegistries)
        {
            if (registry.mPackageName == searchedName)
            {
                return &registry;
            }
        }
        return nullptr;
    }

    void Interpreter::removeFunctionRegistry(const std::string& packageName)
    {
        // We cannot remove root level
        assert(packageName != "");

        std::string removedName = toLowerCase(packageName);
        for (auto iterator = mFunctionRegistries.begin(); iterator != mFunctionRegistries.end(); ++iterator)
        {
            FunctionRegistry& registry = *iterator;

            if (registry.mPackageName == packageName)
            {
                mFunctionRegistries.erase(iterator);
                return;
            }
        }
    }

    void Interpreter::addFunctionRegistry(const std::string& packageName)
    {
        std::string addedName = toLowerCase(packageName);
        if (this->findFunctionRegistry(packageName))
        {
            return;
        }

        mFunctionRegistries.push_back(FunctionRegistry(packageName));
    }

    void Interpreter::activateFunctionRegistry(const std::string& packageName)
    {
        std::string activatedName = toLowerCase(packageName);

        // When we activate a package, it gets moved to the back to take precedence in the stack
        for (auto iterator = mFunctionRegistries.begin(); iterator != mFunctionRegistries.end(); ++iterator)
        {
            FunctionRegistry& registry = *iterator;

            if (registry.mPackageName == packageName)
            {
                if (!registry.mActive)
                {
                    registry.mActive = true;
                    std::rotate(iterator, iterator + 1, mFunctionRegistries.end());
                }

                return;
            }
        }
    }

    void Interpreter::deactivateFunctionRegistry(const std::string& packageName)
    {
        std::string deactivatedName = toLowerCase(packageName);

        FunctionRegistry* deactivated = this->findFunctionRegistry(deactivatedName);
        if (!deactivated)
        {
            return;
        }

        deactivated->mActive = false;
    }
}
