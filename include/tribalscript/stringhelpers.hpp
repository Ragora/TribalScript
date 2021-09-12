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

#include <sstream>

#include <tribalscript/storedvaluestack.hpp>

namespace TribalScript
{
    std::string toLowerCase(const std::string& in);
    std::string expandEscapeSequences(const std::string& in);
    std::vector<std::pair<std::size_t, std::size_t>> getDelineatorData(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::size_t count);
    std::vector<std::string> getStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::size_t count);
    std::string getStringComponentsJoined(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::size_t count);

    std::string setStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::vector<std::string>& newComponents);
    static std::string resolveArrayNameFromStack(StoredValueStack& stack, ExecutionState* state, const std::string& base, const std::size_t argumentCount)
    {
        // Load array components
        std::vector<std::string> arrayComponents;

        // First determine the number of bytes to allocate
        std::size_t totalByteCount = base.size() + 1;
        for (unsigned int iteration = 0; iteration < argumentCount; ++iteration)
        {
            const std::string loadedString = stack.popString(state);
            totalByteCount += loadedString.size() + 1;
            arrayComponents.push_back(loadedString);
        }
        arrayComponents.push_back(base);

        // Allocate workspace
        ++totalByteCount; // Account for NULL terminator

        char* allocatedString = new char[totalByteCount];
        char* currentStringPointer = allocatedString;

        for (auto iterator = arrayComponents.rbegin(); iterator != arrayComponents.rend(); ++iterator)
        {
            const std::string& currentString = *iterator;
            const std::size_t currentStringLength = currentString.size();

            // Copy in from source and add seperator if necessary
            std::memcpy(currentStringPointer, currentString.c_str(), currentStringLength);
            currentStringPointer += currentStringLength;

            if (iterator != arrayComponents.rend() - 1)
            {
                *currentStringPointer = '_';
                ++currentStringPointer;
            }
        }

        // Set terminator
        *currentStringPointer = 0x00;

        auto stringLength = static_cast<std::size_t>(currentStringPointer - allocatedString);
        std::string result(allocatedString, stringLength);
        delete[] allocatedString;

        return result;
    }

    static std::string resolveArrayName(const std::string& base)
    {
        return base;
    }

    template <typename... parameters>
    static std::string resolveArrayName(const std::string& base, const int value, parameters... params)
    {
        return TribalScript::resolveArrayName(base + "_" + std::to_string(value), params...);
    }

    template <typename... parameters>
    static std::string resolveArrayName(const std::string& base, const float value, parameters... params)
    {
        return TribalScript::resolveArrayName(base + "_" + std::to_string(value), params...);
    }

}
