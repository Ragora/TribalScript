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

#include <tribalscript/libraries/fileobject.hpp>

namespace TribalScript
{
    StoredValue OpenForWriteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = parameters.back().toString();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        if (fileObject->openForWrite(path))
        {
            return StoredValue(0);
        }

        return StoredValue(-1);
    }

    StoredValue OpenForReadBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = parameters.back().toString();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        if (fileObject->openForRead(path))
        {
            return StoredValue(0);
        }

        return StoredValue(-1);
    }

    StoredValue WriteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string written = parameters.back().toString();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        fileObject->write(written);
        return StoredValue(0);
    }

    StoredValue CloseBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        fileObject->close();
        return StoredValue(0);
    }

    StoredValue IsEOFBuiltin(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        return StoredValue(fileObject->isEOF() ? 1 : 0);
    }

    StoredValue ReadLineBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        const std::string stringData = fileObject->readLine();
        return StoredValue(stringData.c_str());
    }

    StoredValue IsFileBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = parameters.back().toString();

        std::unique_ptr<FileHandleBase> handle = state->mInterpreter->mConfig.mPlatform->getFileHandle(path);
        return StoredValue(handle->exists() ? 1 : 0);
    }

    StoredValue DeleteFileBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = parameters.back().toString();

        std::unique_ptr<FileHandleBase> handle = state->mInterpreter->mConfig.mPlatform->getFileHandle(path);
        return StoredValue(handle->deleteFile());
    }

    void registerFileObjectLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(OpenForWriteBuiltIn, PACKAGE_EMPTY, "FileObject", "openForWrite")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(OpenForReadBuiltIn, PACKAGE_EMPTY, "FileObject", "openForRead")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(WriteBuiltIn, PACKAGE_EMPTY, "FileObject", "write")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(CloseBuiltIn, PACKAGE_EMPTY, "FileObject", "close")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(IsEOFBuiltin, PACKAGE_EMPTY, "FileObject", "isEOF")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(ReadLineBuiltIn, PACKAGE_EMPTY, "FileObject", "readLine")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(IsFileBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "isFile")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(DeleteFileBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "deleteFile")));

        INTERPRETER_REGISTER_CONSOLEOBJECT_TYPE(interpreter, FileObject, ConsoleObject);
        //interpreter->registerConsoleObjectType<FileObject>();
    }
}
