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

#include <torquescript/storedvalue.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/function.hpp>
#include <torquescript/stringtable.hpp>
#include <torquescript/codeblock.hpp>
#include <torquescript/interpreterconfiguration.hpp>
#include <torquescript/storedvaluestack.hpp>

namespace TorqueScript
{
    struct LoopDescriptor
    {
        LoopDescriptor(const unsigned int pointer, const unsigned int size) : mInstructionPointer(pointer), mLoopSize(size)
        {

        }

        unsigned int mInstructionPointer;
        unsigned int mLoopSize;
    };

    struct ExecutionScopeData
    {
        ExecutionScopeData(Function* function) : mCurrentFunction(function)
        {

        }

        Function* mCurrentFunction;

        //! The stack used for execution in this state.
        StoredValueStack mStack;

        std::vector<LoopDescriptor> mLoopDescriptors;
        std::map<std::size_t, StoredValue> mLocalVariables;
    };

    /**
     *  @brief A specific scope of execution - this is used to delineate local variables
     *  primarily.
     */
    class ExecutionScope
    {
        public:
            ExecutionScope(const InterpreterConfiguration& config);

            void pushFrame(Function* function);
            void popFrame();

            void pushLoop(const unsigned int pointer, const unsigned int depth);
            LoopDescriptor popLoop();
            LoopDescriptor currentLoopDescriptor();
            bool isLoopStackEmpty();
            unsigned int getFrameDepth();

            Function* getCurrentFunction();
            StoredValueStack& getStack();
            StoredValueStack& getReturnStack();

            StoredValue* getVariable(const std::string& name);
            StoredValue* getVariable(const std::size_t name);
            void setVariable(const std::string& name, StoredValue variable);
            void setVariable(const std::size_t name, StoredValue variable);

            const InterpreterConfiguration mConfig;

        private:
            //! A stack of mappings of local variable names to their stored value instance.
            std::vector<ExecutionScopeData> mExecutionScopeData;
    };
}
