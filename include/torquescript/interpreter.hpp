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

#include <torquescript/interpreterconfiguration.hpp>
#include <torquescript/function.hpp>
#include <torquescript/consoleobject.hpp>
#include <torquescript/storedvalue.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/stringtable.hpp>
#include <torquescript/functionregistry.hpp>
#include <torquescript/storedvaluestack.hpp>

#define NAMESPACE_EMPTY ""
#define PACKAGE_EMPTY ""

namespace TorqueScript
{
    //! Forward declaration to deal with circular dependencies.
    class Compiler;
    class CodeBlock;
    class ExecutionState;

    /**
     *  @brief The interpreter class represents a high level instance of the TorqueScript interpreter.
     *  It is where execution control flow begins.
     */
    class Interpreter
    {
        public:
            /**
             *  @brief Constructs a new interpreter with the default configuration.
             *  @see TorqueScript::InterpreterConfiguration
             *  @example interpreterInit.cpp
             */
            Interpreter();

            /**
             *  @brief Constructs a new interpreter with the specified configuration.
             *  @param config The configuration to initialize a new interpreter with.
             */
            Interpreter(const InterpreterConfiguration& config);
            ~Interpreter();

            /// @name Global Variables
            ///
            /// These functions handle getting and setting global variables within
            /// the context of an interpreter.
            /// @{
            void setGlobal(const StringTableEntry name, StoredValue value);
            void setGlobal(const std::string& name, StoredValue value);

            /**
             *  @brief Retrieves a global variable by string.
             *  @param name A string containining the name of the variable
             *  to retrieve, excluding the $ prefix.
             *  @return The stored value at that global variable. If no such variable
             *  exists, nullptr is returned.
             */
            StoredValue* getGlobal(const std::string& name);

            /**
             *  @brief Retrieves a global variable by string ID.
             *  @param name The string ID representing the global variable
             *  name to lookup. This should be preferred over the lookup by name
             *  as this version will be faster.
             *  @return The stored value at that global variable. If no such variable
             *  exists, nullptr is returned.
             */
            StoredValue* getGlobal(const StringTableEntry name);

            /// @}

            /**
             *  @brief Ask the interpreter to compile the input string and return the resulting
             *  CodeBlock.
             *  @param input The string input containing the TorqueScript program.
             */
            CodeBlock* compile(const std::string& input);
            void evaluate(const std::string& input, ExecutionState* state = nullptr);
            void execute(const std::string& path, ExecutionState* state = nullptr);

            /// @name Functions
            ///
            /// These functions handle management of functions registered in the interpreter.
            /// @{

            /**
             *  @brief Registers a new function to the interpreter.
             *  @param function The function object to register to the interpreter.
             */
            void addFunction(std::shared_ptr<Function> function);
            std::shared_ptr<Function> getFunction(const std::string& space, const std::string& name);
            std::shared_ptr<Function> getFunctionParent(Function* function);

            FunctionRegistry* findFunctionRegistry(const std::string packageName);
            void addFunctionRegistry(const std::string& packageName);
            void activateFunctionRegistry(const std::string& packageName);
            void deactivateFunctionRegistry(const std::string& packageName);
            void removeFunctionRegistry(const std::string& packageName);
            /// @}

            //! The string table associated with this interpreter.
            StringTable mStringTable;

            //! The interpreter configuration.
            const InterpreterConfiguration mConfig;

            /**
             *  @brief Initializes the tree of objects described by the provided descriptor.
             *  @details The tree described is walked and all objects along the tree will be initialized,
             *  unless any parent is invalid (Ie. bad typename, invalid parameters, etc) at which point
             *  that entire branch of the tree is invalidated and *not* instantiated.
             *  @param descriptor The descriptor describing the root of the tree to initialize.
             *  @return The root level ConsoleObject that was instantiated if successful. Otherwise,
             *  nullptr is returned.
             */
            ConsoleObject* initializeConsoleObjectTree(ObjectInstantiationDescriptor& descriptor);

            template <typename classType>
            void registerConsoleObjectType()
            {
                // Ensure descriptors are initialized
                classType::initializeMemberFields(TypeInformation<classType>::Descriptor);
            }

        private:
            //! Keep a ready instance of the compiler on hand as it is reusable.
            Compiler* mCompiler;

            //! A mapping of function namespaces to a mapping of function names to the function object.
            std::vector<FunctionRegistry> mFunctionRegistries;

            //! A mapping of global variable names to their stored value instance.
            std::unordered_map<StringTableEntry, StoredValue> mGlobalVariables;
    };
}
