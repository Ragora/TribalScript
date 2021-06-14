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

#include <memory>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <torquescript/nativefunction.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionstate.hpp>
#include <torquescript/storedvaluestack.hpp>
#include <torquescript/consoleobject.hpp>
#include <torquescript/fileobject.hpp>

namespace TorqueScript
{
    void EchoBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);

    void ExecBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);
    void ActivatePackageBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);

    void DeactivatePackageBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);

    void DeleteBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);

    void GetNameBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);

    void GetClassNameBuiltIn(ConsoleObject* thisObject, std::shared_ptr<ExecutionState> state, const std::size_t argumentCount);

    void registerCoreLibrary(Interpreter* interpreter);
}
