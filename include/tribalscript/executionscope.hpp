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

#include <tribalscript/storedvalue.hpp>
#include <tribalscript/stringhelpers.hpp>
#include <tribalscript/function.hpp>
#include <tribalscript/stringtable.hpp>
#include <tribalscript/codeblock.hpp>
#include <tribalscript/stringtable.hpp>
#include <tribalscript/interpreterconfiguration.hpp>
#include <tribalscript/storedvaluestack.hpp>
#include <tribalscript/consoleobject.hpp>

namespace TribalScript
{
    /**
     *  @brief Struct describing a tree of console object initializations.
     */
    struct ObjectInstantiationDescriptor
    {
        /**
         *  @brief Constructs a new ObjectInstantiationDescriptor.
         *  @param typeName The console object type to inititalize.
         *  @param name The name of the new object.
         */
        ObjectInstantiationDescriptor(const std::string typeName, const std::string& name) : mName(name), mTypeName(typeName)
        {

        }

        void copyFieldsToConsoleObject(ConsoleObject* target)
        {
            for (auto&& assignment : mFieldAssignments)
            {
                target->setTaggedField(assignment.first, assignment.second);
            }
        }

        //! The name to assign the new console object.
        std::string mName;

        //! The typename to instantiate the console object as.
        std::string mTypeName;

        //! All children of this object. These will not be initialized until the parent is initialized.
        std::vector<ObjectInstantiationDescriptor> mChildren;

        //! All resolved field names mapped to the values to set.
        std::map<std::string, StoredValue> mFieldAssignments;
    };

    struct ExecutionScopeData
    {
        explicit ExecutionScopeData(Function* function) : mCurrentFunction(function)
        {
            mStack.reserve(64);
            mRegisters.reserve(32);
        }

        Function* mCurrentFunction;

        //! The stack used for execution in this state.
        StoredValueStack mStack;

        //! Awaiting root-level object instantiations.
        std::vector<ObjectInstantiationDescriptor> mObjectInstantiations;

        std::vector<StoredValue> mRegisters;
    };

    /**
     *  @brief A specific scope of execution - this is used to delineate local variables
     *  primarily.
     */
    class ExecutionScope
    {
        public:
            ExecutionScope(const InterpreterConfiguration& config, StringTable* table);

            void pushFrame(Function* function);
            void popFrame();

            bool isAwaitingParentInstantiation();
            void pushObjectInstantiation(const std::string& typeName, const std::string& name);
            ObjectInstantiationDescriptor popObjectInstantiation();
            ObjectInstantiationDescriptor& currentObjectInstantiation();

            std::size_t getFrameDepth();

            Function* getCurrentFunction();
            StoredValueStack& getStack();
            StoredValueStack& getReturnStack();

            __forceinline StoredValue* getRegister(const std::size_t registerID)
            {
                ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

                if (registerID >= currentScope.mRegisters.size())
                {
                    return nullptr;
                }

                return &currentScope.mRegisters[registerID];
            }

            __forceinline StoredValue* getRegisterOrAllocate(const std::size_t registerID)
            {
                StoredValue* result = getRegister(registerID);
                ExecutionScopeData& currentScope = *mExecutionScopeData.rbegin();

                if (!result)
                {
                    // Allocate empty registers between where we are now and where we want to go
                    for (std::size_t iteration = currentScope.mRegisters.size(); iteration < registerID + 1; ++iteration)
                    {
                        currentScope.mRegisters.emplace_back(0);
                    }

                    assert(registerID < currentScope.mRegisters.size());
                    result = &currentScope.mRegisters[registerID];
                }

                return result;
            }

            void setRegister(const std::size_t registerID, const StoredValue& variable);

            //! The InterpreterConfiguration associated with this ExecutionScope.
            const InterpreterConfiguration mConfig;

        private:
            StringTable* mStringTable;

            //! A stack of mappings of local variable names to their stored value instance.
            std::vector<ExecutionScopeData> mExecutionScopeData;
    };
}
