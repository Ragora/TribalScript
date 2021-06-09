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
#include <iostream>
#include <cstdio>

#include <torquescript/standardfilehandle.hpp>

namespace TorqueScript
{
    StandardFileHandle::StandardFileHandle(const std::string& path) : FileHandleBase(path)
    {

    }

    bool StandardFileHandle::isOpen()
    {
        return mFileHandle.is_open();
    }

    void StandardFileHandle::seek(const std::streampos& position)
    {
        mFileHandle.seekg(position);
        mFileHandle.seekp(position);
    }

    void StandardFileHandle::seek(const std::streampos& offset, std::ios_base::seekdir way)
    {
        mFileHandle.seekg(offset, way);
        mFileHandle.seekp(offset, way);
    }

    void StandardFileHandle::write(const char* buffer, const std::size_t size)
    {
        mFileHandle.write(buffer, size);
    }

    void StandardFileHandle::read(char* out, const std::size_t size)
    {
        mFileHandle.read(out, size);
    }

    void StandardFileHandle::close()
    {
        mFileHandle.close();
    }

    bool StandardFileHandle::isEOF()
    {
        return mFileHandle.eof();
    }

    std::streampos StandardFileHandle::tell()
    {
        std::streampos writePosition = mFileHandle.tellg();
        std::streampos readPosition = mFileHandle.tellp();

        assert(writePosition == readPosition);

        return readPosition;
    }

    void StandardFileHandle::openForWrite()
    {
        mFileHandle = std::fstream(mPath, std::fstream::out);
    }

    void StandardFileHandle::openForRead()
    {
        mFileHandle = std::fstream(mPath, std::fstream::in);
    }

    void StandardFileHandle::openForReadAndWrite()
    {
        mFileHandle = std::fstream(mPath, std::fstream::in | std::fstream::out);
    }

    bool StandardFileHandle::exists()
    {
        FILE* handle = std::fopen(mPath.c_str(), "r");
        if (handle)
        {
            std::fclose(handle);
            return true;
        }
        return false;
    }

    bool StandardFileHandle::deleteFile()
    {
        return std::remove(mPath.c_str()) == 0;
    }
}
