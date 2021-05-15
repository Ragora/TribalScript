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

#include <torquescript/builtins.hpp>
#include <torquescript/interpreter.hpp>
#include <torquescript/executionscope.hpp>
#include <torquescript/codeblock.hpp>

int main(int argc, char* argv[])
{
    TorqueScript::Interpreter interpreter;
    TorqueScript::registerBuiltIns(&interpreter);

    std::cout << "Type TorqueScript Program, use EOF (CTRL+D on Unix, CTRL+Z on Windows) to End Input" << std::endl << std::endl;

    std::istreambuf_iterator<char> begin(std::cin), end;
    std::string evaluated(begin, end);

    std::vector<std::shared_ptr<TorqueScript::StoredVariable>> stack;
    TorqueScript::CodeBlock* compiled = interpreter.compile(evaluated);

    // Load all functions from the codeblock
    std::vector functions = compiled->getFunctions();
    for (auto&& function : functions)
    {
        interpreter.addFunction(function);
    }

    TorqueScript::ExecutionScope scope;
    compiled->execute(&interpreter, &scope, stack);

    // Produce a disassembly for debugging
    std::cout << std::endl << "Disassembly: " << std::endl;
    std::vector<std::string> disassembly = compiled->disassemble();

    for (auto iterator = disassembly.begin(); iterator != disassembly.end(); ++iterator)
    {
        std::cout << *iterator << std::endl;
    }

    delete compiled;
    return 0;
}
