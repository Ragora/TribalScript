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

#include <torquescript/libraries/fileobject.hpp>

namespace TorqueScript
{
    void OpenForWriteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        if (fileObject->openForWrite(path))
        {
            stack.push_back(StoredValue(0));
            return;
        }

        stack.push_back(StoredValue(-1));
    }

    void OpenForReadBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        if (fileObject->openForRead(path))
        {
            stack.push_back(StoredValue(0));
            return;
        }

        stack.push_back(StoredValue(-1));
    }

    void WriteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string written = stack.popString(state);

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        fileObject->write(written);
        stack.push_back(StoredValue(0));
    }

    void CloseBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        fileObject->close();
        stack.push_back(StoredValue(0));
    }

    void IsEOFBuiltin(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        stack.push_back(StoredValue(fileObject->isEOF() ? 1 : 0));
    }

    void ReadLineBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();

        // Open the handle
        assert(thisObject);
        FileObject* fileObject = reinterpret_cast<FileObject*>(thisObject);
        assert(fileObject);

        const StringTableEntry stringID = state->mInterpreter->mStringTable.getOrAssign(fileObject->readLine());
        stack.push_back(StoredValue(stringID, StoredValueType::String));
    }

    void IsFileBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        std::unique_ptr<FileHandleBase> handle = state->mInterpreter->mConfig.mPlatform->getFileHandle(path);
        stack.push_back(StoredValue(handle->exists() ? 1 : 0));
    }

    void DeleteFileBuiltIn(ConsoleObject* thisObject, ExecutionState* state, const std::size_t argumentCount)
    {
        StoredValueStack& stack = state->mExecutionScope.getStack();
        std::string path = stack.popString(state);

        std::unique_ptr<FileHandleBase> handle = state->mInterpreter->mConfig.mPlatform->getFileHandle(path);
        stack.push_back(StoredValue(handle->deleteFile()));
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

        interpreter->registerConsoleObjectType<FileObject>();
    }
}
