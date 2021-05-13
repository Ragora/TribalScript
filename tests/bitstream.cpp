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

#include "gtest/gtest.h"

#include <torquescript/bitstream.hpp>

const static float sFloatList[] =
{
    3.14159,
    1337.125,
};
const static unsigned int sFloatCount = sizeof(sFloatList) / sizeof(float);

inline void PackFloats(TorqueScript::BitStream& out)
{
    for (unsigned int iteration = 0; iteration < sFloatCount; iteration++)
    {
        out.write(sFloatList[iteration]);
    }
}

TEST(BitStream, Floats)
{
    unsigned int expectedStreamSize = sFloatCount * sizeof(float);
    TorqueScript::BitStream floatStream(expectedStreamSize);

    PackFloats(floatStream);
    floatStream.setPointer(0);

    // Check if our BitStream size is correct
    //EXPECT_EQ(expectedStreamSize, floatStream.getSize());

    // Now make sure we can unpack the data correctly
    for (unsigned int iteration = 0; iteration < sFloatCount; iteration++)
    {
        EXPECT_EQ(sFloatList[iteration], floatStream.pop<float>());
    }
}

TEST(BitStream, MemoryBlock)
{
    unsigned int expectedStreamSize = sizeof(sFloatList);
    TorqueScript::BitStream floatStream(expectedStreamSize);
    PackFloats(floatStream);

    // We wil have a memory block to use from the float stream
    TorqueScript::BitStream blockStream(floatStream.getBlock(), floatStream.getPointer());

    for (unsigned int iteration = 0; iteration < sFloatCount; iteration++)
    {
        EXPECT_EQ(sFloatList[iteration], blockStream.pop<float>());
    }
}

int main()
{
    return RUN_ALL_TESTS();
}
