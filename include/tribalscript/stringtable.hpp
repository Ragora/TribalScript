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

#include <cstddef>
#include <unordered_map>
#include <string>
#include <functional>

namespace TribalScript
{
    typedef std::size_t StringTableEntry;

    /**
     *  @brief A string table maintains a mapping of numeric identifiers to their
     *  raw string values. This is used to simplify code referencing strings
     *  by using a fixed width identifier.
     *  @details String identifiers are currently generated by taking a hash of the
     *  input string. The hash algorithm used is defined by std::hash in the STL.
     */
    class StringTable : public std::unordered_map<StringTableEntry, std::string>
    {
        public:
            StringTableEntry getOrAssign(const std::string& string);

            const std::string& getString(const StringTableEntry id);
    };
}
