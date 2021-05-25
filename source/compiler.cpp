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

#include <TorqueLexer.h>
#include <TorqueParser.h>

#include <torquescript/compiler.hpp>
#include <torquescript/compilervisitor.hpp>
#include <torquescript/instructionsequence.hpp>
#include <torquescript/parsererrorlistener.hpp>

namespace TorqueScript
{
    CodeBlock* Compiler::compileStream(std::istream& input)
    {
        antlr4::ANTLRInputStream antlrStream(input);
        TorqueLexer lexer(&antlrStream);

        // Setup our parser
        antlr4::CommonTokenStream stream(&lexer);
        TorqueParser parser(&stream);
        parser.removeErrorListeners();

        ParserErrorListener parserErrorListener;
        parser.addErrorListener(&parserErrorListener);

        // Instantiate the program and go
        CompilerVisitor visitor;

        InstructionSequence instructions = visitor.collapseInstructions(visitor.visitProgram(parser.program()).as<GeneratedInstructions>());

        // Did we receive any errors?
        if (parserErrorListener.getErrors().empty())
        {
            CodeBlock* result = new CodeBlock();
            result->addInstructions(instructions);

            return result;
        }

        for (const std::string& message : parserErrorListener.getErrors())
        {
            std::cerr << message << std::endl;
        }

        return nullptr;
    }

    CodeBlock* Compiler::compileString(const std::string& input)
    {
        std::stringstream stream;
        stream << input;
        return this->compileStream(stream);
    }

    CodeBlock* Compiler::compileFile(const std::string& path)
    {
        std::ifstream fileStream;
        fileStream.open(path);

        return this->compileStream(fileStream);
    }
}
