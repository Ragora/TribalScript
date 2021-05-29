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

#include <string>
#include <memory>
#include <unordered_map>

#include <torquescript/storedvalue.hpp>

namespace TorqueScript
{
    class Interpreter;
    class ExecutionScope;
    class Interpreter;

    /**
     *  @brief Base class for object instances recognized by the interpreter. These object instances
     *  may contain member fields addressable the interpreter as well as tagged fields which are arbitrarily
     *  mapped values.
     */
    class ConsoleObject
    {
        public:
            /**
             *  @brief Retrieves a tagged field by name from the object.
             *  @param The tagged field name to retrieve, which is case insensitive.
             *  @return A StoredValue pointer of the value currently in that tagged field
             *  slot.
             */
            StoredValue* getTaggedField(const std::string& name);

            /**
             *  @brief Sets a tagged field by name on the object.
             *  @param name The tagged field name to set, which is case insensitive.
             *  @param value The value to set.
             */
            void setTaggedField(const std::string& name, StoredValue value);

        protected:
            //! A mapping of tagged field names to their stored values.
            std::unordered_map<std::string, StoredValue> mTaggedFields;
    };
}
