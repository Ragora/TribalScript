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
#include <vector>
#include <iostream>
#include <algorithm>

#include <tribalscript/stringhelpers.hpp>

namespace TribalScript
{
    std::vector<std::pair<std::size_t, std::size_t>> getDelineatorData(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::size_t count, std::size_t& realDelineatorCount)
    {
        std::vector<std::pair<std::size_t, std::size_t>> dataLocations;
        dataLocations.reserve(count);

        // Search for the delineators that we need
        std::size_t currentDataStart = 0;
        std::size_t currentDelineatorCount = 0;
        for (std::size_t iteration = 0; iteration < in.size(); ++iteration)
        {
            unsigned char character = in[iteration];

            // Increment delineator count
            if (currentDelineatorCount >= startComponent && (character == delineator || iteration == in.size() - 1))
            {
                const std::size_t dataEnd = iteration == in.size() - 1 ? iteration + 1 : iteration;

                if (currentDelineatorCount >= startComponent)
                {
                    dataLocations.push_back(std::make_pair(currentDataStart, dataEnd));
                }

                // Set next start
                currentDataStart = iteration + 1;
                ++currentDelineatorCount;

                if (currentDelineatorCount >= count + startComponent)
                {
                    break;
                }
            }
            else if (character == delineator)
            {
                currentDataStart = iteration + 1;
                ++currentDelineatorCount;
            }
        }

        realDelineatorCount = currentDelineatorCount;
        return dataLocations;
    }

    std::vector<std::string> getStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::size_t count)
    {
        std::vector<std::string> result;
        result.reserve(count);

        std::size_t realDelineatorCount;
        std::vector<std::pair<std::size_t, std::size_t>> dataLocations = getDelineatorData(in, delineator, startComponent, count, realDelineatorCount);

        // Process all pairs now
        for (auto&& dataPair : dataLocations)
        {
            const std::size_t dataStart = dataPair.first;
            const std::size_t dataEnd = dataPair.second;

            result.push_back(in.substr(dataStart, dataEnd - dataStart));
        }
        return result;
    }

    std::string getStringComponentsJoined(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::size_t count)
    {
        const std::vector<std::string> components = getStringComponents(in, delineator, startComponent, count);

        std::ostringstream result;
        for (std::size_t iteration = 0; iteration < components.size(); ++iteration)
        {
            if (iteration != components.size() - 1)
            {
                result << components[iteration] << delineator;
            }
            else
            {
                result << components[iteration];
            }
        }

        return result.str();
    }

    std::string setStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::vector<std::string>& newComponents)
    {
        std::size_t realDelineatorCount = 0;
        std::vector<std::pair<std::size_t, std::size_t>> delineatorData = getDelineatorData(in, delineator, startComponent, newComponents.size(), realDelineatorCount);

        // If the delineator data is not equal to our component count then some of the data needs appended (with the delineator in between)
        const std::size_t endIndex = startComponent + newComponents.size();
        std::size_t appendedCount = endIndex >= realDelineatorCount ? endIndex - realDelineatorCount : 0;

        std::string leftHalf;
        std::string rightHalf;
        std::ostringstream stream;

        if (delineatorData.size() != 0)
        {
            leftHalf = in.substr(0,delineatorData[0].first);
            rightHalf = in.substr(delineatorData[delineatorData.size() - 1].second, in.size() );
        }
        else
        {
            stream << in;
            appendedCount = newComponents.size();
        }

        // Set everything in between now ...
        stream << leftHalf;

        for (std::size_t iteration = 0; iteration < delineatorData.size(); ++iteration)
        {
            const std::pair<std::size_t, std::size_t>& dataPair = delineatorData[iteration];
            const std::string nextField = iteration < newComponents.size() ? newComponents[iteration] : in.substr(dataPair.first, dataPair.second - dataPair.first);

            if (iteration == 0)
            {
                stream << nextField;
            }
            else
            {
                stream << delineator << nextField;
            }
        }

        // Append empty delineators
        if (startComponent - appendedCount >= realDelineatorCount)
        {
            const std::size_t emptyDelineatorCount = (startComponent - appendedCount) - 1;
            for (std::size_t iteration = 0; iteration < emptyDelineatorCount; ++iteration)
            {
                stream << delineator;
            }
        }

        // Handle appended results now
        for (std::size_t iteration = newComponents.size() - appendedCount; iteration < newComponents.size(); ++iteration)
        {
            stream << delineator << newComponents[iteration];
        }

        stream << rightHalf;
        return stream.str();
    }

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
        std::regex otherSequences("\\\\([tn])");

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

        // Handle ie. tab and newline sequences
        while (std::regex_search(result, match, otherSequences))
        {
            const std::string beginning = result.substr(0, match.position());
            const std::string ending = result.substr(match.position() + match.length(), result.size());

            if (match[1] == "n")
            {
                result = beginning + '\n' + ending;
            }
            else if (match[1] == "t")
            {
                result = beginning + '\t' + ending;
            }
            else
            {
                throw std::invalid_argument("Unexpected escape type!");
            }
        }

        return result;
    }
}
