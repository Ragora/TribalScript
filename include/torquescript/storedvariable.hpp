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

#include <iostream>
#include <assert.h>
#include <variant>
#include <string>

namespace TorqueScript
{
    /**
     *  @brief Storage class used to keep variable values in-memory of arbitrary data types.
     */
    class StoredVariable
    {
        public:
            typedef enum {
                FLOAT,
                INTEGER,
                STRING,
                LOCALREFERENCE,
                GLOBALREFERENCE
            } VariableType;

            explicit StoredVariable(const float value)
            {
                mValue = value;
                mVariableType = VariableType::FLOAT;
            }

            explicit StoredVariable(const int value)
            {
                mValue = value;
                mVariableType = VariableType::INTEGER;
            }

            explicit StoredVariable(const std::string& value, const VariableType valueType = VariableType::STRING)
            {
                // Ensure we're not trying to pass a string as an int or something
                // FIXME: Perhaps use a hierarchy of types here?
                assert(valueType == VariableType::STRING || valueType == VariableType::LOCALREFERENCE || valueType == VariableType::GLOBALREFERENCE);

                mValue = value;
                mVariableType = valueType;
            }

            VariableType getVariableType()
            {
                return mVariableType;
            }

            std::variant<float, int, std::string> getValue()
            {
                return mValue;
            }

            // Below is Torque-specific implementations of handling of individual cases
            float toFloat()
            {
                switch (mVariableType)
                {
                    case StoredVariable::VariableType::INTEGER:
                        return (float)std::get<int>(mValue);
                    case StoredVariable::VariableType::FLOAT:
                        return std::get<float>(mValue);
                    case StoredVariable::VariableType::STRING:
                        try
                        {
                            return std::stof(std::get<std::string>(mValue));
                        }
                        catch (std::invalid_argument exception)
                        {

                        }
                        break;
                    default:
                        throw new std::runtime_error("Unhandled conversion argument type!");
                }
            }

            std::string toString()
            {
                switch (mVariableType)
                {
                    case StoredVariable::VariableType::INTEGER:
                        return std::to_string(std::get<int>(mValue));
                    case StoredVariable::VariableType::LOCALREFERENCE:
                    case StoredVariable::VariableType::GLOBALREFERENCE:
                        return std::get<std::string>(mValue);
                    case StoredVariable::VariableType::FLOAT:
                        return std::to_string(std::get<float>(mValue));
                    case StoredVariable::VariableType::STRING:
                        return std::get<std::string>(mValue);
                    default:
                        throw new std::runtime_error("Unhandled conversion argument type!");
                }
            }

        private:
            //! The type of variable this is.
            VariableType mVariableType;

            //! The value stored.
            std::variant<float, int, std::string> mValue;
    };
}
