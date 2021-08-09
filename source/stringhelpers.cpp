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

#include <regex>
#include <string>
#include <iostream>
#include <algorithm>

#include <tribalscript/stringhelpers.hpp>

namespace TribalScript
{
    std::string toLowerCase(const std::string& in)
    {
        std::string result = in;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char character) { return std::tolower(character); });
        return result;
    }

    std::string expandEscapeSequences(const std::string& in)
    {
        std::regex colorRegex("\\\\c([0-9])");
        std::regex hexRegex("\\\\x([0-9a-f]{1,2})");

        std::string result = in;

        // Handle hex escapes
        std::smatch match;
        while (std::regex_search(result, match, hexRegex))
        {
            const unsigned long hexValue = std::stoul(match[1], nullptr, 16);
            std::string hexString(1, (const char)hexValue);

            const std::string beginning = result.substr(0, match.position());
            const std::string ending = result.substr(match.position() + match.length(), result.size());
            result = beginning + hexString + ending;
        }

        // Handle color escapes - in T2 these are just aliases for an unprintable
        while (std::regex_search(result, match, colorRegex))
        {
            const unsigned long integerValue = std::stoul(match[1], nullptr, 10);
            std::string colorString = "";
            switch (integerValue)
            {
                case 0:
                    colorString = "\x02";
                    break;
                case 1:
                    colorString = "\x03";
                    break;
                case 2:
                    colorString = "\x04";
                    break;
                case 3:
                    colorString = "\x05";
                    break;
                case 4:
                    colorString = "\x06";
                    break;
                case 5:
                    colorString = "\x07";
                    break;
                case 6:
                    colorString = "\x08";
                    break;
                case 7:
                    colorString = "\x0B";
                    break;
                case 8:
                    colorString = "\x0C";
                    break;
                case 9:
                    colorString = "\x0E";
                    break;
            }

            const std::string beginning = result.substr(0, match.position());
            const std::string ending = result.substr(match.position() + match.length(), result.size());
            result = beginning + colorString + ending;
        }
        return result;
    }
}
