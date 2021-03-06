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

#include <tribalscript/consoleobject.hpp>
#include <tribalscript/stringhelpers.hpp>
#include <tribalscript/interpreter.hpp>

namespace TribalScript
{
    ConsoleObject::ConsoleObject(Interpreter* interpreter) : mInterpreter(interpreter)
    {

    }

    ConsoleObject::~ConsoleObject()
    {
        // Deallocate fields
        for (auto iterator = mTaggedFields.begin(); iterator != mTaggedFields.end(); ++iterator)
        {
            delete iterator->second;
        }
    }

    StoredValue* ConsoleObject::getTaggedField(const std::string& name)
    {
        const std::string searchName = toLowerCase(name);
        auto search = mTaggedFields.find(searchName);

        if (search != mTaggedFields.end())
        {
            return search->second;
        }
        return nullptr;
    }

    StoredValue* ConsoleObject::getTaggedFieldOrAllocate(const std::string& name)
    {
        const std::string searchName = toLowerCase(name);
        auto search = mTaggedFields.find(searchName);

        if (search != mTaggedFields.end())
        {
            return search->second;
        }

        StoredValue* newValue = new StoredValue(0);
        mTaggedFields.insert(std::make_pair(name, newValue));
        return newValue;
    }

    void ConsoleObject::setTaggedField(const std::string& name, StoredValue value)
    {
        const std::string setName = toLowerCase(name);

        auto search = mTaggedFields.find(setName);
        if (search != mTaggedFields.end())
        {
            search->second->setValue(value);
        }
        else
        {
            mTaggedFields.insert(std::make_pair(setName, new StoredValue(value)));
        }
    }

    bool ConsoleObject::addChild(ConsoleObject* child)
    {
        return false;
    }

	bool ConsoleObject::destroy()
	{
		for (ConsoleObject* parent : mParents)
		{
			parent->removeChild(this);
		}
		return true;
	}

	bool ConsoleObject::removeChild(ConsoleObject* child)
	{
		for (auto iterator = mChildren.begin(); iterator != mChildren.end(); ++iterator)
		{
			if (*iterator == child)
			{
				mChildren.erase(iterator);
				return true;
			}
		}
		return false;
	}

    std::string ConsoleObject::getVirtualClassName()
    {
        return this->getClassName();
    }

	void ConsoleObject::associateWithParent(ConsoleObject* parent)
	{
		parent->mChildren.push_back(this);
		mParents.push_back(parent);
	}
}
