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

    void Compiler::pushInstructionFrame()
    {
        mInstructionStack.push_back(std::vector<std::shared_ptr<Instruction>>());
    }

    void Compiler::popInstructionFrame()
    {
        mInstructionStack.pop_back();
    }

    std::vector<std::shared_ptr<Instruction>>& Compiler::getCurrentInstructionFrame()
    {
        if (mInstructionStack.empty())
        {
            this->pushInstructionFrame();
        }
        return mInstructionStack.back();
    }


    // Compiler routines =====================================================

    void Compiler::enterFunctiondeclaration(TorqueParser::FunctiondeclarationContext* context)
    {
        this->pushInstructionFrame();
    }

    void Compiler::exitFunctiondeclaration(TorqueParser::FunctiondeclarationContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        std::shared_ptr<Function> newFunction = std::shared_ptr<Function>(new Function(context->LABELNAMESPACESINGLE()->getText()));
        newFunction->addInstructions(currentFrame);
        this->popInstructionFrame();

        mCurrentCodeBlock->addFunction(newFunction);
    }

    void Compiler::enterArithmetic(TorqueParser::ArithmeticContext* context)
    {

    }

    void Compiler::exitArithmetic(TorqueParser::ArithmeticContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->PLUS())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new AddInstruction()));
        }
        else if (context->MULT())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new MultiplyInstruction()));
        }
        else
        {
            throw std::runtime_error("Encountered unhandled arithmetic type!");
        }
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

        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        currentFrame.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(calledFunctionName)));
        currentFrame.push_back(std::shared_ptr<Instruction>(new CallFunctionInstruction()));
    }

    void Compiler::enterValue(TorqueParser::ValueContext* context)
    {

    }

    void Compiler::exitValue(TorqueParser::ValueContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->FLOAT())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushFloatInstruction(std::stof(context->getText()))));
        }
        else if (context->STRING())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
            const std::string rawString = context->getText();
            const std::string stringContent = rawString.substr(1, rawString.size() - 2);
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(stringContent)));
        }
        else if (context->INT())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoi(context->getText()))));
        }
        else if (context->LOCALVARIABLE())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the % prefix
            const std::string rawString = context->getText();
            const std::string variableName = rawString.substr(1, rawString.size());
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushLocalReferenceInstruction(variableName)));
        }
        else if (context->GLOBALVARIABLE())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the % prefix
            const std::string rawString = context->getText();
            const std::string variableName = rawString.substr(1, rawString.size());
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushGlobalReferenceInstruction(variableName)));
        }
        else if (context->TRUE())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(1)));
        }
        else if (context->FALSE())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(0)));
        }
        else
        {
            throw std::runtime_error("Encountered unhandled value type!");
        }
    }

    void Compiler::enterConcatenation(TorqueParser::ConcatenationContext* context)
    {

    }

    void Compiler::exitConcatenation(TorqueParser::ConcatenationContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->CONCAT())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new ConcatInstruction()));
        }
        else
        {
            throw std::runtime_error("Encountered unhandled concat op type!");
        }
    }


    void Compiler::enterUnary(TorqueParser::UnaryContext* context)
    {

    }

    void Compiler::exitUnary(TorqueParser::UnaryContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->MINUS())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new NegateInstruction()));
        }
        else
        {
            throw std::runtime_error("Encountered unhandled unary op type!");
        }
    }

    void Compiler::enterAssignment(TorqueParser::AssignmentContext* context)
    {

    }

    void Compiler::exitAssignment(TorqueParser::AssignmentContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        std::cout << context->getText() << std::endl;
        if (context->ASSIGN())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new AssignmentInstruction()));
        }
        else
        {
            throw std::runtime_error("Encountered unhandled unary op type!");
        }
    }

    void Compiler::enterBitwise(TorqueParser::BitwiseContext* context)
    {

    }

    void Compiler::exitBitwise(TorqueParser::BitwiseContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->BITWISEAND())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new BitwiseAndInstruction()));
        }
        else
        {
            throw std::runtime_error("Encountered unknown bitwise type!");
        }
    }

    void Compiler::enterProgram(TorqueParser::ProgramContext* context)
    {
        this->pushInstructionFrame();
    }

    void Compiler::exitProgram(TorqueParser::ProgramContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        mCurrentCodeBlock->addInstructions(currentFrame);
        this->popInstructionFrame();
    }

    void Compiler::enterStatement(TorqueParser::StatementContext* context)
    {

    }

    void Compiler::exitStatement(TorqueParser::StatementContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        currentFrame.push_back(std::shared_ptr<Instruction>(new PopInstruction()));
    }
}
