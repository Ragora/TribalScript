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

#include <torquescript/stringtable.hpp>

namespace TorqueScript
{
    StringTable::StringTable()
    {
        mEntryCount = 0;
    }

    unsigned int StringTable::getOrAssign(const std::string& string)
    {
        const unsigned int existingID = this->getID(string);
        if (existingID != 0)
        {
            return existingID;
        }

        const unsigned int newID = ++mEntryCount;
        this->emplace(std::make_pair(newID, string));
        return newID;
    }

    unsigned int StringTable::getID(const std::string& string)
    {
        for (auto search : *this)
        {
            if (search.second == string)
            {
                return search.first;
            }
        }
        return 0;
    }

    const std::string& StringTable::getString(const unsigned int id)
    {
        return this->at(id);
    }
}
