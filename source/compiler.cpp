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

namespace TorqueScript
{
    CodeBlock* Compiler::compileStream(std::istream& input)
    {
        mCurrentCodeBlock = new CodeBlock();

        antlr4::ANTLRInputStream antlrStream(input);
        TorqueLexer lexer(&antlrStream);
        antlr4::CommonTokenStream stream(&lexer);
        TorqueParser parser(&stream);

        antlr4::tree::ParseTree* tree = parser.program();
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(this, tree);

        return mCurrentCodeBlock;
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

    // Compiler routines =====================================================

    void Compiler::enterFunctiondeclaration(TorqueParser::FunctiondeclarationContext* context)
    {
        // Functions are a global construct only
        assert(!mCurrentFunction);

        mCurrentFunction = new Function(context->LABELNAMESPACESINGLE()->getText());
        mCurrentCodeBlock->addFunction(mCurrentFunction);
    }

    void Compiler::exitFunctiondeclaration(TorqueParser::FunctiondeclarationContext* context)
    {
        mCurrentFunction = nullptr;
    }

    void Compiler::enterArithmetic(TorqueParser::ArithmeticContext* context)
    {

    }

    void Compiler::exitArithmetic(TorqueParser::ArithmeticContext* context)
    {

    }

    void Compiler::enterRelational(TorqueParser::RelationalContext* context)
    {

    }

    void Compiler::exitRelational(TorqueParser::RelationalContext* context)
    {

    }
}
