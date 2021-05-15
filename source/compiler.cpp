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

    void Compiler::pushInstructions(const std::vector<std::shared_ptr<Instruction>>& instructions)
    {
        if (mCurrentFunction)
        {
            mCurrentFunction->addInstructions(instructions);
            return;
        }

        mCurrentCodeBlock->addInstructions(instructions);
    }

    // Compiler routines =====================================================

    void Compiler::enterFunctiondeclaration(TorqueParser::FunctiondeclarationContext* context)
    {
        // Functions are a global construct only
        assert(!mCurrentFunction);

        mCurrentFunction = std::shared_ptr<Function>(new Function(context->LABELNAMESPACESINGLE()->getText()));
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
        std::vector<std::shared_ptr<Instruction>> generatedCode;

        if (context->PLUS())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new AddInstruction()));
        }
        else if (context->MULT())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new MultiplyInstruction()));
        }
        else
        {
            throw new std::runtime_error("Encountered unhandled arithmetic type!");
        }

        this->pushInstructions(generatedCode);
    }

    void Compiler::enterRelational(TorqueParser::RelationalContext* context)
    {

    }

    void Compiler::exitRelational(TorqueParser::RelationalContext* context)
    {

    }

    void Compiler::enterCall(TorqueParser::CallContext* context)
    {

    }

    void Compiler::exitCall(TorqueParser::CallContext* context)
    {
        const std::string calledFunctionName = context->LABELNAMESPACESINGLE()->getText();

        std::vector<std::shared_ptr<Instruction>> generatedCode;
        generatedCode.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(calledFunctionName)));
        generatedCode.push_back(std::shared_ptr<Instruction>(new CallFunctionInstruction()));

        this->pushInstructions(generatedCode);
    }

    void Compiler::enterValue(TorqueParser::ValueContext* context)
    {

    }

    void Compiler::exitValue(TorqueParser::ValueContext* context)
    {
        std::vector<std::shared_ptr<Instruction>> generatedCode;

        if (context->FLOAT())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new PushFloatInstruction(std::stof(context->getText()))));
        }
        else if (context->STRING())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
            const std::string rawString = context->getText();
            const std::string stringContent = rawString.substr(1, rawString.size() - 2);
            generatedCode.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(stringContent)));
        }
        else if (context->INT())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoi(context->getText()))));
        }
        else if (context->LOCALVARIABLE())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the % prefix
            const std::string rawString = context->getText();
            const std::string variableName = rawString.substr(1, rawString.size());
            generatedCode.push_back(std::shared_ptr<Instruction>(new PushLocalReferenceInstruction(variableName)));
        }
        else if (context->GLOBALVARIABLE())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the % prefix
            const std::string rawString = context->getText();
            const std::string variableName = rawString.substr(1, rawString.size());
            generatedCode.push_back(std::shared_ptr<Instruction>(new PushGlobalReferenceInstruction(variableName)));
        }
        else
        {
            throw new std::runtime_error("Encountered unhandled value type!");
        }

        this->pushInstructions(generatedCode);
    }

    void Compiler::enterConcatenation(TorqueParser::ConcatenationContext* context)
    {

    }

    void Compiler::exitConcatenation(TorqueParser::ConcatenationContext* context)
    {
        std::vector<std::shared_ptr<Instruction>> generatedCode;

        if (context->CONCAT())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new ConcatInstruction()));
        }
        else
        {
            throw new std::runtime_error("Encountered unhandled concat op type!");
        }

        this->pushInstructions(generatedCode);
    }


    void Compiler::enterUnary(TorqueParser::UnaryContext* context)
    {

    }

    void Compiler::exitUnary(TorqueParser::UnaryContext* context)
    {
        std::vector<std::shared_ptr<Instruction>> generatedCode;

        if (context->MINUS())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new NegateInstruction()));
        }
        else
        {
            throw new std::runtime_error("Encountered unhandled unary op type!");
        }

        this->pushInstructions(generatedCode);
    }

    void Compiler::enterAssignment(TorqueParser::AssignmentContext* context)
    {

    }

    void Compiler::exitAssignment(TorqueParser::AssignmentContext* context)
    {
        std::vector<std::shared_ptr<Instruction>> generatedCode;

        std::cout << context->getText() << std::endl;
        if (context->ASSIGN())
        {
            generatedCode.push_back(std::shared_ptr<Instruction>(new AssignmentInstruction()));
        }
        else
        {
            throw new std::runtime_error("Encountered unhandled unary op type!");
        }

        this->pushInstructions(generatedCode);
    }
}
