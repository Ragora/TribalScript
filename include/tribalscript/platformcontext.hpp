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

#include <tribalscript/filehandlebase.hpp>

namespace TribalScript
{
    class PlatformContext
    {
        public:
            /**
             *  @brief Asks the interpreter to handle a simple echo message.
             *  @param message The message to echo.
             */
            virtual void logEcho(const std::string& message);

            /**
             *  @brief Asks the interpreter to handle an error log message.
             *  @param message The message to output as an error.
             */
            virtual void logError(const std::string& message);

            /**
             *  @brief Asks the interpreter to handle a warning log message.
             *  @param message The message to output as a warning.
             */
            virtual void logWarning(const std::string& message);

            /**
             *  @brief Asks the interpreter to handle a debug log message.
             *  @param message The message to output as a debug message.
             */
            virtual void logDebug(const std::string& message);

            /**
             *  @brief Retrieves an abstract file handle to the file at the specified path. This
             *  allows for platform-dependent implementations of file I/O such as with Unreal Engine
             *  or PhysicsFS.
             *  @return A pointer to the file handle. Must be deleted by the call site when done.
             */
            virtual std::unique_ptr<FileHandleBase> getFileHandle(const std::string& path);
    };
}
