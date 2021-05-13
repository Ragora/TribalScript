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

#include <string.h>

#include <torquescript/bitstream.hpp>

namespace TorqueScript
{
    BitStream::BitStream(void* initializer, const size_t initializerLength, const size_t resizeLength) : mMemoryBlock((unsigned char*)initializer),
    mTotalSize(initializerLength), mPointer(0), mOwnsMemoryBlock(false), mResizeLength(resizeLength)
    {
    }

    BitStream::BitStream(const size_t sizeInBytes, const void* initializer, size_t initializerLength, const size_t resizeLength) :
    mMemoryBlock(new unsigned char[sizeInBytes]), mPointer(0), mTotalSize(sizeInBytes), mOwnsMemoryBlock(true), mResizeLength(resizeLength)
    {
        memset(mMemoryBlock, 0x00, sizeInBytes);

        if (initializer && !initializerLength)
        {
            initializerLength = sizeInBytes;
        }

        // Are we initializing from some existing memory block?
        if (initializer && initializerLength)
        {
            // Only copy what we can actually store.
            initializerLength = initializerLength >= sizeInBytes ? sizeInBytes - 1 : initializerLength;
            memcpy(mMemoryBlock, initializer, initializerLength);

            // Make sure we the pointer is set to the next available space
            mPointer = sizeInBytes;
        }
    }

    //! Standard destructor.
    BitStream::~BitStream(void)
    {
        if (mOwnsMemoryBlock)
        {
            free(mMemoryBlock);
        }
    }

    void BitStream::writeString(const char* string, const size_t length)
    {
        // assert(mPointer <= mTotalSize);

        // FIXME: The check below may cause a SIGSEGV if we're out on the heap and the input string (and length) comes from an untrusted source
        // FIXME: If mResizeLength < string length, this will cause erroneous writes

        // Is it properly NULL terminated?
        if (string[length] != 0x00)
        {
            throw std::runtime_error("Attempted to write bad string (null terminator isn't where it should be)!");
        }

        const size_t stringLength = strlen(string);
        const size_t totalLength = stringLength + sizeof(unsigned int) + 1; // Account for the NULL as well

        if (stringLength != length)
        {
            throw std::runtime_error("Attempted to write a bad string (Lengths do not match)!");
        }

        // Will the string fit?
        if (mTotalSize - mPointer < totalLength && mResizeLength == 0)
        {
            throw std::runtime_error("Cannot fit string into buffer!");
        }
        else if (mTotalSize - mPointer < totalLength)
        {
            this->resize(mTotalSize + mResizeLength);
        }

        // Write off the string length so we can properly unpack later
        this->write<unsigned int>(static_cast<unsigned int>(stringLength));

        if (mPointer >= mTotalSize || mTotalSize - mPointer < totalLength)
        {
            throw std::overflow_error("Stack Overflow");
        }

        memcpy(&mMemoryBlock[mPointer], string, stringLength + 1);
        mPointer += stringLength + 1; // NULL byte again
    }

    void BitStream::writeString(const std::string& string)
    {
        this->writeString(string.data(), string.length());
    }

    const char* BitStream::popString(void)
    {
        // Grab the top string
        // FIXME: Use the length read out of the buffer instead of recalculating the string size here
        const char* result = this->topString();
        mPointer += strlen(result) + sizeof(unsigned int) + 1;

        return result;
    }

    const char* BitStream::topString(void)
    {
        // Read the string length off
        const unsigned int stringLength = this->top<unsigned int>();

        // First off, is there enough memory in the buffer?
        if (stringLength > mTotalSize - mPointer)
        {
            throw std::underflow_error("Stack Underflow in String Read");
        }

        // Ensure that the string is properly terminated
        const unsigned int nullIndex = mPointer + stringLength + sizeof(unsigned int);
        if (mMemoryBlock[nullIndex] != 0x00)
        {
            throw std::logic_error("Attempted to unpack an improperly terminated string");
        }

        // Return the result
        return reinterpret_cast<const char*>(&mMemoryBlock[mPointer + sizeof(unsigned int)]);
    }

    size_t BitStream::getPointer(void)
    {
        return mPointer;
    }

    void* BitStream::getBlock(void)
    {
        return mMemoryBlock;
    }

    void BitStream::setPointer(const size_t pointer)
    {
        if (pointer < 0 || pointer >= mTotalSize)
        {
            throw std::out_of_range("Attempted to index out of bounds in BitStream");
        }

        mPointer = pointer;
    }

    void BitStream::resize(const size_t newSize)
    {
        // TODO: Implement as an exception
        // CONSOLE_ASSERTF(newSize > mTotalSize, "newSize=%u,mTotalSize=%u", newSize, mTotalSize);

        if (mOwnsMemoryBlock)
        {
            mMemoryBlock = reinterpret_cast<unsigned char*>(realloc(mMemoryBlock, newSize));
        }
        else
        {
            unsigned char* newBlock = reinterpret_cast<unsigned char*>(malloc(newSize));

            // Only memset the new bytes
            memset(&newBlock[mPointer + 1], 0x00, newSize - (mPointer + 1));

            // Copy any data we're actually using out of the block
            memcpy(newBlock, mMemoryBlock, mPointer);

            // Update our stored block information
            mMemoryBlock = newBlock;

            // We definitely own this block now
            mOwnsMemoryBlock = true;
        }

        mTotalSize = newSize;
    }

    bool BitStream::isFull(void) const
    {
        return mTotalSize == mPointer;
    }

    bool BitStream::isEmpty(void) const
    {
        return mPointer == 0;
    }

    size_t BitStream::getSize(void) const
    {
        return mTotalSize;
    }
}
