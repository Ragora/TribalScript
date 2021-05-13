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

#include <map>
#include <string>
#include <variant>

/**
 *  @brief Storage class used to keep variable values in-memory of arbitrary data types.
 */
class StoredVariable
{
    typedef enum {
        FLOAT,
        INTEGER,
        STRING
    } VariableType;

    //! The type of variable this is.
    VariableType mVariableType;

    //! The value stored.
    std::variant<float, int, std::string> mValue;

    explicit StoredVariable(const float value)
    {
        mValue = value;
    }

    explicit StoredVariable(const int value)
    {
        mValue = value;
    }

    explicit StoredVariable(const std::string& value)
    {
        mValue = value;
    }
};

/**
 *  @brief A specific scope of execution - this is used to delineate local variables
 *  primarily.
 */
class ExecutionScope
{
    private:
        //! A mapping of local variable names to their stored value instance.
        std::map<std::string, StoredVariable> mLocalVariables;
};

/**
 *  @brief The interpreter class represents a high level instance of the TorqueScript interpreter.
 *  It is where execution control flow begins.
 */
class Interpreter
{
    private:
        //! A mapping of global variable names to their stored value instance.
        std::map<std::string, StoredVariable> mGlobalVariables;
};
