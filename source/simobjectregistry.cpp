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

#include <torquescript/simobjectregistry.hpp>
#include <torquescript/compiler.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/stringhelpers.hpp>
#include <torquescript/executionstate.hpp>

namespace TorqueScript
{
    SimObjectRegistry::SimObjectRegistry() : mNextObjectID(0)
    {

    }

    void SimObjectRegistry::setSimObject(const std::string& name, std::shared_ptr<SimObject> value)
    {
        const std::string setName = toLowerCase(name);
        mSimObjectsByName[setName] = value;

        // Ensure an ID mapping exists
        this->addSimObject(value);
    }

    std::shared_ptr<SimObject> SimObjectRegistry::getSimObject(const std::string& name)
    {
        const std::string searchedName = toLowerCase(name);
        auto search = mSimObjectsByName.find(searchedName);

        if (search != mSimObjectsByName.end())
        {
            return search->second;
        }
        return nullptr;
    }

    std::shared_ptr<SimObject> SimObjectRegistry::getSimObject(const unsigned int id)
    {
        auto search = mSimObjectsByID.find(id);

        if (search != mSimObjectsByID.end())
        {
            return search->second;
        }

        return nullptr;
    }

    void SimObjectRegistry::removeSimObject(const std::string& name)
    {
        const std::string removedName = toLowerCase(name);

        auto search = mSimObjectsByName.find(removedName);
        if (search != mSimObjectsByName.end())
        {
            mSimObjectsByName.erase(search);
        }
    }

    void SimObjectRegistry::removeSimObject(std::shared_ptr<SimObject> target)
    {
        // Remove from name mapping & ID mapping
        for (auto iterator = mSimObjectsByName.begin(); iterator != mSimObjectsByName.end(); ++iterator)
        {
            if (iterator->second == target)
            {
                mSimObjectsByName.erase(iterator);
                break;
            }
        }

        for (auto iterator = mSimObjectsByID.begin(); iterator != mSimObjectsByID.end(); ++iterator)
        {
            if (iterator->second == target)
            {
                mSimObjectsByID.erase(iterator);
                break;
            }
        }
    }

    unsigned int SimObjectRegistry::addSimObject(std::shared_ptr<SimObject> value)
    {
        // Check if it exists already in our ID mapping
        for (auto iterator = mSimObjectsByID.begin(); iterator != mSimObjectsByID.end(); ++iterator)
        {
            if (iterator->second == value)
            {
                return iterator->first;
            }
        }

        const unsigned int result = mNextObjectID++;

        mSimObjectsByID[result] = value;
        return result;
    }

    std::string SimObjectRegistry::getSimObjectName(SimObject* target)
    {
        // Search name set
        for (auto iterator = mSimObjectsByName.begin(); iterator != mSimObjectsByName.end(); ++iterator)
        {
            if (iterator->second.get() == target)
            {
                return iterator->first;
            }
        }
        return "";
    }

    unsigned int SimObjectRegistry::getSimObjectID(SimObject* target)
    {
        for (auto iterator = mSimObjectsByID.begin(); iterator != mSimObjectsByID.end(); ++iterator)
        {
            if (iterator->second.get() == target)
            {
                return iterator->first;
            }
        }
        return 0;
    }
}
