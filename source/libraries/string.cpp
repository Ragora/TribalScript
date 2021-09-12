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

#include <tribalscript/libraries/string.hpp>
#include <tribalscript/stringhelpers.hpp>

namespace TribalScript
{
    /* Words */
    StoredValue GetWordBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        std::string result = getStringComponentsJoined(parameters[0].toString(), ' ', parameters[1].toInteger(), 1);
        return StoredValue(result.c_str());
    }

    StoredValue GetWordsBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        std::string result = getStringComponentsJoined(parameters[0].toString(), ' ', parameters[1].toInteger(), parameters[2].toInteger());
        return StoredValue(result.c_str());
    }

    StoredValue SetWordBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        // std::string setStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::vector<std::string>& newComponents)
        std::string result = setStringComponents(parameters[0].toString(), ' ', parameters[1].toInteger(), {
            parameters[2].toString()
        });
        return StoredValue(result.c_str());
    }

    StoredValue SetWordsBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        // std::string setStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::vector<std::string>& newComponents)
        std::vector<std::string> newWords;
        for (std::size_t iteration = 2; iteration < parameters.size(); ++iteration)
        {
            newWords.push_back(parameters[iteration].toString());
        }

        std::string result = setStringComponents(parameters[0].toString(), ' ', parameters[1].toInteger(), newWords);
        return StoredValue(result.c_str());
    }

    /* Fields */
    StoredValue GetFieldBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        std::string result = getStringComponentsJoined(parameters[0].toString(), '\t', parameters[1].toInteger(), 1);
        return StoredValue(result.c_str());
    }

    StoredValue GetFieldsBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        std::string result = getStringComponentsJoined(parameters[0].toString(), '\t', parameters[1].toInteger(), parameters[2].toInteger());
        return StoredValue(result.c_str());
    }

    StoredValue SetFieldBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        // std::string setStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::vector<std::string>& newComponents)
        std::string result = setStringComponents(parameters[0].toString(), '\t', parameters[1].toInteger(), {
            parameters[2].toString()
        });
        return StoredValue(result.c_str());
    }

    StoredValue SetFieldsBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters)
    {
        // std::string setStringComponents(const std::string& in, const unsigned char delineator, const std::size_t startComponent, const std::vector<std::string>& newComponents)
        std::vector<std::string> newWords;
        for (std::size_t iteration = 2; iteration < parameters.size(); ++iteration)
        {
            newWords.push_back(parameters[iteration].toString());
        }

        std::string result = setStringComponents(parameters[0].toString(), '\t', parameters[1].toInteger(), newWords);
        return StoredValue(result.c_str());
    }

    void registerStringLibrary(Interpreter* interpreter)
    {
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetWordBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getWord")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetFieldsBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getWords")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(SetWordBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "setWord")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(SetWordsBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "setWords")));

        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetFieldBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getField")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(GetWordsBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "getFields")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(SetFieldBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "setField")));
        interpreter->addFunction(std::shared_ptr<Function>(new NativeFunction(SetFieldsBuiltIn, PACKAGE_EMPTY, NAMESPACE_EMPTY, "setFields")));
    }
}
