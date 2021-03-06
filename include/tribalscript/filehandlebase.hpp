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
#include <iostream>

namespace TribalScript
{
    class FileHandleBase
    {
        public:
            FileHandleBase(const std::string& path);
            virtual ~FileHandleBase();

            virtual bool isOpen() = 0;

            virtual void seek(const std::streampos& position) = 0;
            virtual void seek(const std::streampos& offset, std::ios_base::seekdir way) = 0;

            virtual void read(char* out, const std::size_t size) = 0;
            virtual void close();
            virtual bool isEOF() = 0;
            virtual void write(const char* buffer, const std::size_t size) = 0;
            virtual std::string readLine() = 0;

            virtual std::streampos tell() = 0;

            virtual void openForWrite() = 0;
            virtual void openForRead() = 0;
            virtual void openForReadAndWrite() = 0;

            virtual bool exists() = 0;
            virtual bool deleteFile() = 0;

            //! The file path this handle refers to.
            const std::string mPath;
    };
}
