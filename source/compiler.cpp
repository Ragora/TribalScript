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
#include <torquescript/astbuilder.hpp>
#include <torquescript/instructionsequence.hpp>
#include <torquescript/parsererrorlistener.hpp>

namespace TorqueScript
{
    CodeBlock* Compiler::compileStream(std::istream& input, StringTable* stringTable)
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
        ASTBuilder visitor(stringTable);
        ProgramNode* tree = visitor.visitProgram(parser.program()).as<ProgramNode*>();

        // Did we receive any errors?
        if (parserErrorListener.getErrors().empty())
        {

            // At this point, we should have a valid tree to generate code from
            delete tree;
            return nullptr;
            /*
            InstructionSequence instructions;
            for (ASTNode* node : tree)
            {
                InstructionSequence childInstructions = node->compile(stringTable);
                instructions.insert(instructions.end(), childInstructions.begin(), childInstructions.end());

                delete node;
            }
            */
            //CodeBlock* result = new CodeBlock(instructions);
            //return result;
        }

        delete tree;

        for (const std::string& message : parserErrorListener.getErrors())
        {
            std::cerr << message << std::endl;
        }

        return nullptr;
    }

    CodeBlock* Compiler::compileString(const std::string& input, StringTable* stringTable)
    {
        std::stringstream stream;
        stream << input;
        return this->compileStream(stream, stringTable);
    }

    CodeBlock* Compiler::compileFile(const std::string& path, StringTable* stringTable)
    {
        std::ifstream fileStream;
        fileStream.open(path);

        return this->compileStream(fileStream, stringTable);
    }
}
