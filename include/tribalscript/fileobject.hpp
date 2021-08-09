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

#include <string>
#include <memory>
#include <unordered_map>

#include <tribalscript/consoleobject.hpp>
#include <tribalscript/filehandlebase.hpp>
#include <tribalscript/executionscope.hpp>

namespace TribalScript
{
    /**
     *  @brief A FileObject is a class that may be used to interact with the filesystem
     *  provided by the PlatformContext configured with the interpreter.
     */
    class FileObject : public ConsoleObject
    {
        DECLARE_CONSOLE_OBJECT_BODY()

        public:
            FileObject(Interpreter* interpreter);

            /**
             *  @brief Opens a file at the specified path in write-only mode.
             *  @param path The file to open.
             *  @return True if successful, false otherwise.
             */
            bool openForWrite(const std::string& path);

            bool openForRead(const std::string& path);

            bool isEOF();

            std::string readLine();

            /**
             *  @brief Writes a string to the file, if the FileObject has been
             *  opened with write mode enabled.
             *  @param written The string data to write to the file.
             */
            void write(const std::string& written);
            void close();

            static void initializeMemberFields(ConsoleObjectDescriptor* descriptor);

            static ConsoleObject* instantiateFromDescriptor(Interpreter* interpreter, ObjectInstantiationDescriptor& descriptor);

        private:
            std::unique_ptr<FileHandleBase> mHandle;
    };

    DECLARE_CONSOLE_OBJECT(FileObject, ConsoleObject)
}
