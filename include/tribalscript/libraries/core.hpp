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

#include <tribalscript/nativefunction.hpp>
#include <tribalscript/executionscope.hpp>
#include <tribalscript/interpreter.hpp>
#include <tribalscript/executionstate.hpp>
#include <tribalscript/storedvaluestack.hpp>
#include <tribalscript/consoleobject.hpp>
#include <tribalscript/fileobject.hpp>

namespace TribalScript
{
    void EchoBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void ExecBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);
    void ActivatePackageBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void DeactivatePackageBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void DeleteBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void GetNameBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void GetIDBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void GetRealTimeBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void GetClassNameBuiltIn(ConsoleObject* thisObject, ExecutionState* state, std::vector<StoredValue>& parameters);

    void registerCoreLibrary(Interpreter* interpreter);
}
