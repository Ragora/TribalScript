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
#include <vector>
#include <memory>

#include <torquescript/function.hpp>
#include <torquescript/simobject.hpp>
#include <torquescript/storedvalue.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/storedvaluestack.hpp>

#define NAMESPACE_EMPTY ""
#define PACKAGE_EMPTY ""

namespace TorqueScript
{
    //! Forward declaration to deal with circular dependencies.
    class Compiler;
    class CodeBlock;
    class ExecutionState;

    class SimObjectRegistry
    {
        public:
            SimObjectRegistry();

            void setSimObject(const std::string& name, std::shared_ptr<SimObject> value);
            std::shared_ptr<SimObject> getSimObject(const std::string& name);
            std::shared_ptr<SimObject> getSimObject(const unsigned int id);

            std::string getSimObjectName(SimObject* target);
            unsigned int getSimObjectID(SimObject* target);

            unsigned int addSimObject(std::shared_ptr<SimObject> value);

            void removeSimObject(const std::string& name);
            void removeSimObject(std::shared_ptr<SimObject> target);

        private:
            unsigned int mNextObjectID;

            //! A mapping of object IDs to their sim objects
            std::map<unsigned int, std::shared_ptr<SimObject>> mSimObjectsByID;

            //! A mapping of object names to their sim objects
            std::map<std::string, std::shared_ptr<SimObject>> mSimObjectsByName;
    };
}
