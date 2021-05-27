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

#include <torquescript/consoleobjectregistry.hpp>
#include <torquescript/compiler.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/executionstate.hpp>

namespace TorqueScript
{
    ConsoleObjectRegistry::ConsoleObjectRegistry() : mNextObjectID(0)
    {

    }

    void ConsoleObjectRegistry::setConsoleObject(const std::string& name, std::shared_ptr<ConsoleObject> value)
    {
        const std::string setName = toLowerCase(name);
        mConsoleObjectsByName[setName] = value;

        // Ensure an ID mapping exists
        this->addConsoleObject(value);
    }

    std::shared_ptr<ConsoleObject> ConsoleObjectRegistry::getConsoleObject(const std::string& name)
    {
        const std::string searchedName = toLowerCase(name);
        auto search = mConsoleObjectsByName.find(searchedName);

        if (search != mConsoleObjectsByName.end())
        {
            return search->second;
        }
        return nullptr;
    }

    std::shared_ptr<ConsoleObject> ConsoleObjectRegistry::getConsoleObject(const unsigned int id)
    {
        auto search = mConsoleObjectsByID.find(id);

        if (search != mConsoleObjectsByID.end())
        {
            return search->second;
        }

        return nullptr;
    }

    void ConsoleObjectRegistry::removeConsoleObject(const std::string& name)
    {
        const std::string removedName = toLowerCase(name);

        auto search = mConsoleObjectsByName.find(removedName);
        if (search != mConsoleObjectsByName.end())
        {
            mConsoleObjectsByName.erase(search);
        }
    }

    void ConsoleObjectRegistry::removeConsoleObject(std::shared_ptr<ConsoleObject> target)
    {
        // Remove from name mapping & ID mapping
        for (auto iterator = mConsoleObjectsByName.begin(); iterator != mConsoleObjectsByName.end(); ++iterator)
        {
            if (iterator->second == target)
            {
                mConsoleObjectsByName.erase(iterator);
                break;
            }
        }

        for (auto iterator = mConsoleObjectsByID.begin(); iterator != mConsoleObjectsByID.end(); ++iterator)
        {
            if (iterator->second == target)
            {
                mConsoleObjectsByID.erase(iterator);
                break;
            }
        }
    }

    unsigned int ConsoleObjectRegistry::addConsoleObject(std::shared_ptr<ConsoleObject> value)
    {
        // Check if it exists already in our ID mapping
        for (auto iterator = mConsoleObjectsByID.begin(); iterator != mConsoleObjectsByID.end(); ++iterator)
        {
            if (iterator->second == value)
            {
                return iterator->first;
            }
        }

        const unsigned int result = mNextObjectID++;

        mConsoleObjectsByID[result] = value;
        return result;
    }

    std::string ConsoleObjectRegistry::getConsoleObjectName(std::shared_ptr<ConsoleObject> target)
    {
        // Search name set
        for (auto iterator = mConsoleObjectsByName.begin(); iterator != mConsoleObjectsByName.end(); ++iterator)
        {
            if (iterator->second == target)
            {
                return iterator->first;
            }
        }
        return "";
    }

    unsigned int ConsoleObjectRegistry::getConsoleObjectID(std::shared_ptr<ConsoleObject> target)
    {
        for (auto iterator = mConsoleObjectsByID.begin(); iterator != mConsoleObjectsByID.end(); ++iterator)
        {
            if (iterator->second == target)
            {
                return iterator->first;
            }
        }
        return 0;
    }
}
