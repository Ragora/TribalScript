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

#include <memory>
#include <iostream>
#include <stdexcept>

#include <torquescript/nativefunction.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/consoleobject.hpp>
#include <torquescript/fileobject.hpp>

namespace TorqueScript
{
    static void OpenForWriteBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        // Open the handle
        assert(thisObject);
        std::shared_ptr<FileObject> fileObject = std::dynamic_pointer_cast<FileObject>(thisObject);
        assert(fileObject);

        if (fileObject->openForWrite(path))
        {
            stack.push_back(StoredValue(0));
            return;
        }

        stack.push_back(StoredValue(-1));
    }

    static void WriteBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string written = stack.popString(state);

        // Open the handle
        assert(thisObject);
        std::shared_ptr<FileObject> fileObject = std::dynamic_pointer_cast<FileObject>(thisObject);
        assert(fileObject);

        fileObject->write(written);
        stack.push_back(StoredValue(0));
    }

    static void CloseBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        std::shared_ptr<FileObject> fileObject = std::dynamic_pointer_cast<FileObject>(thisObject);
        assert(fileObject);

        fileObject->close();
        stack.push_back(StoredValue(0));
    }

    static void IsFileBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        std::unique_ptr<FileHandleBase> handle = state->mInterpreter->mConfig.mPlatform->getFileHandle(path);
        stack.push_back(StoredValue(handle->exists() ? 1 : 0));
    }

    static void DeleteFileBuiltIn(std::shared_ptr<ConsoleObject> thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        std::unique_ptr<FileHandleBase> handle = state->mInterpreter->mConfig.mPlatform->getFileHandle(path);
        stack.push_back(StoredValue(handle->deleteFile()));
    }

    static void registerFileObjectLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(OpenForWriteBuiltIn, PACKAGE_EMPTY, "FileObject", "openForWrite")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(WriteBuiltIn, PACKAGE_EMPTY, "FileObject", "write")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(CloseBuiltIn, PACKAGE_EMPTY, "FileObject", "close")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(IsFileBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "isFile")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeleteFileBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "deleteFile")));

        interpreter->registerConsoleObjectType<FileObject>();
    }
}
