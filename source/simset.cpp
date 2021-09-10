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

#include <tribalscript/simset.hpp>
#include <tribalscript/interpreter.hpp>

namespace TribalScript
{
    IMPLEMENT_CONSOLE_OBJECT(SimSet, ConsoleObject)

    SimSet::SimSet(Interpreter* interpreter) : ConsoleObject(interpreter)
    {

    }

	bool SimSet::addChild(ConsoleObject* child)
	{
		child->associateWithParent(this);
		return true;
	}

	std::size_t SimSet::getCount()
	{
		return mChildren.size();
	}

	ConsoleObject* SimSet::getObject(std::size_t index)
	{
		return mChildren[index];
	}

    ConsoleObject* SimSet::instantiateFromDescriptor(Interpreter* interpreter, ObjectInstantiationDescriptor& descriptor)
    {
        return new SimSet(interpreter);
    }

    void SimSet::initializeMemberFields(ConsoleObjectDescriptor* descriptor)
    {

    }
}
