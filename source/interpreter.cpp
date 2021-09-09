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

#include <cassert>

#include <tribalscript/interpreter.hpp>
#include <tribalscript/compiler.hpp>
#include <tribalscript/executionscope.hpp>
#include <tribalscript/stringhelpers.hpp>
#include <tribalscript/executionstate.hpp>

namespace TribalScript
{
    Interpreter::Interpreter() : Interpreter(InterpreterConfiguration())
    {

    }

    Interpreter::Interpreter(const InterpreterConfiguration& config) : mConfig(config)
    {
        mCompiler = new Compiler(mConfig);

        // "" is the default top-level
        this->addFunctionRegistry(PACKAGE_EMPTY);
        this->activateFunctionRegistry(PACKAGE_EMPTY);
    }

    Interpreter::~Interpreter()
    {
        assert(mCompiler);
        delete mCompiler;
    }

    void Interpreter::evaluate(const std::string& input, ExecutionState* state)
    {
        assert(mCompiler);

        // Spawn a compiler and generate a codeblock
        CodeBlock* compiled = this->compile(input);

        // FIXME: Some kind of error was encountered. Ask the compiler?
        if (!compiled)
        {
            return;
        }

        if (state)
        {
            compiled->execute(state);
        }
        else
        {
            ExecutionState localState = ExecutionState(this);
            compiled->execute(&localState);
        }
    }

    void Interpreter::execute(const std::string& path, ExecutionState* state)
    {
        CodeBlock* compiled = mCompiler->compileFile(path, &mStringTable);

        // FIXME: Some kind of error was encountered. Ask the compiler?
        if (!compiled)
        {
            return;
        }

        if (state)
        {
            compiled->execute(state);
        }
        else
        {
            ExecutionState localState = ExecutionState(this);
            compiled->execute(&localState);
        }
    }

    CodeBlock* Interpreter::compile(const std::string& input)
    {
        return mCompiler->compileString(input, &mStringTable);
    }

    StoredValue* Interpreter::getGlobal(const std::string& name)
    {
        const StringTableEntry stringID = mStringTable.getOrAssign(mConfig.mCaseSensitive ? name : toLowerCase(name));
        auto search = mGlobalVariables.find(stringID);
        if (search != mGlobalVariables.end())
        {
            return search->second;
        }
        return nullptr;
    }

    StoredValue* Interpreter::getGlobal(const StringTableEntry name)
    {
        auto search = mGlobalVariables.find(name);
        if (search != mGlobalVariables.end())
        {
            return search->second;
        }
        return nullptr;
    }

    StoredValue* Interpreter::getGlobalOrAllocate(const StringTableEntry name)
    {
        auto search = mGlobalVariables.find(name);
        if (search != mGlobalVariables.end())
        {
            return search->second;
        }

        StoredValue* newValue = new StoredValue(0);
        mGlobalVariables.insert(std::make_pair(name, newValue));
        return newValue;
    }

    void Interpreter::addFunction(std::shared_ptr<Function> function)
    {
        // Make sure the registry exists - if it already does this does nothing
        std::string package = function->getDeclaredPackage();
        this->addFunctionRegistry(package);
        FunctionRegistry* registry = this->findFunctionRegistry(package);

        const std::string storedName = toLowerCase(function->getDeclaredName());
        const std::string storedNameSpace = toLowerCase(function->getDeclaredNameSpace());
        registry->mFunctions[storedNameSpace][storedName] = function;
    }

    std::shared_ptr<Function> Interpreter::getFunction(const std::string& space, const std::string& name)
    {
        // Search registries back to front
        const std::string searchedName = toLowerCase(name);
        const std::string searchedNameSpace = toLowerCase(space);

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
        const std::string searchedPackage = toLowerCase(function->getDeclaredPackage());
        const std::string searchedNameSpace = toLowerCase(function->getDeclaredNameSpace());
        const std::string searchedFunction = toLowerCase(function->getDeclaredName());

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
        const StringTableEntry key = mStringTable.getOrAssign(mConfig.mCaseSensitive ? name : toLowerCase(name));

        auto search = mGlobalVariables.find(key);
        if (search != mGlobalVariables.end())
        {
            search->second->setValue(value);
            return;
        }

        mGlobalVariables.emplace(std::make_pair(key, new StoredValue(value)));
    }

    void Interpreter::setGlobal(const StringTableEntry name, StoredValue value)
    {
        auto search = mGlobalVariables.find(name);
        if (search != mGlobalVariables.end())
        {
            search->second->setValue(value);
            return;
        }

        mGlobalVariables.emplace(std::make_pair(name, new StoredValue(value)));
    }

    FunctionRegistry* Interpreter::findFunctionRegistry(const std::string& packageName)
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
        assert(packageName != PACKAGE_EMPTY);

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

        mFunctionRegistries.emplace_back(packageName);
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

    void Interpreter::relinkNamespaces()
    {
        for (auto&& entry : mConsoleObjectDescriptors)
        {
            // Reset the hierarchy of this namespace
            entry.second->mHierarchy = relinkNamespace(entry.second->mName);
            entry.second->mHierarchy.insert(entry.second->mHierarchy.begin(), entry.second->mName);
        }
    }

    ConsoleObjectDescriptor* Interpreter::lookupDescriptor(const std::string& objectTypeName)
    {
        const std::string chosenTypeName = mConfig.mCaseSensitive ? objectTypeName : toLowerCase(objectTypeName);

        auto lookup = mConsoleObjectDescriptors.find(chosenTypeName);

        if (lookup != mConsoleObjectDescriptors.end())
        {
            return lookup->second;
        }
        return nullptr;
    }

    std::unordered_map<std::string, ConsoleObjectDescriptor*>& Interpreter::getConsoleObjectDescriptors()
    {
        return mConsoleObjectDescriptors;
    }

    std::vector<std::string> Interpreter::relinkNamespace(const std::string& space)
    {
        auto search = mConsoleObjectDescriptors.find(space);
        if (search == mConsoleObjectDescriptors.end())
        {
            std::ostringstream out;
            out << "Fatal error in relink namespaces: " << space;
            throw std::runtime_error(out.str());
        }

        ConsoleObjectDescriptor* currentDescriptor = search->second;

        std::vector<std::string> result;
        result.push_back(currentDescriptor->mParentName);

        // ConsoleObject won't have an entry
        // FIXME: Is there a more appropriate way to handle this?
        if (toLowerCase(currentDescriptor->mParentName) == "consoleobject")
        {
            return result;
        }

        std::vector<std::string> children = relinkNamespace(currentDescriptor->mParentName);
        result.insert(result.end(), children.begin(), children.end());
        return result;
    }

    ConsoleObject* Interpreter::initializeConsoleObjectTree(ObjectInstantiationDescriptor& descriptor)
    {
        // Lookup console object descriptor
        ConsoleObjectDescriptor* objectDescriptor = this->lookupDescriptor(descriptor.mTypeName);
        if (!objectDescriptor)
        {
            std::ostringstream errorStream;
            errorStream << "Cannot instantiate non-console object type '" << descriptor.mTypeName << "'!";

            mConfig.mPlatform->logError(errorStream.str());

            // Forget about child initialization at this point
            return nullptr;
        }

        ConsoleObject* initialized = objectDescriptor->mInitializePointer(this, descriptor);

        // Register to interpreter
        mConfig.mConsoleObjectRegistry->addConsoleObject(this, initialized);
        mConfig.mConsoleObjectRegistry->setConsoleObject(this, descriptor.mName, initialized);

        // Handle child init
        for (ObjectInstantiationDescriptor& childDescriptor : descriptor.mChildren)
        {
            ConsoleObject* childObject = this->initializeConsoleObjectTree(childDescriptor);

            // Add to parent
            if (childObject)
            {
                initialized->addChild(childObject);
            }
        }

        return initialized;
    }
}
