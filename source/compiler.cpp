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
#include <torquescript/instructionsequence.hpp>
#include <torquescript/parsererrorlistener.hpp>

namespace TorqueScript
{
    CodeBlock* Compiler::compileStream(std::istream& input)
    {
        mCurrentCodeBlock = new CodeBlock();

        antlr4::ANTLRInputStream antlrStream(input);
        TorqueLexer lexer(&antlrStream);

        // Setup our parser
        antlr4::CommonTokenStream stream(&lexer);
        TorqueParser parser(&stream);
        parser.removeErrorListeners();

        ParserErrorListener parserErrorListener;
        parser.addErrorListener(&parserErrorListener);

        // Instantiate the program and go
        antlr4::tree::ParseTree* tree = parser.program();
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(this, tree);

        // Did we receive any errors?
        if (parserErrorListener.getErrors().empty())
        {
            return mCurrentCodeBlock;
        }

        for (const std::string& message : parserErrorListener.getErrors())
        {
            std::cerr << message << std::endl;
        }

        delete mCurrentCodeBlock;
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

    void Compiler::pushInstructionFrame()
    {
        mInstructionStack.push_back(std::vector<std::shared_ptr<Instruction>>());
    }

    void Compiler::popInstructionFrame()
    {
        std::vector<std::shared_ptr<Instruction>> oldFrame = this->getCurrentInstructionFrame();
        mInstructionStack.pop_back();
        //std::vector<std::shared_ptr<Instruction>>& newFrame = this->getCurrentInstructionFrame();

        //newFrame.insert(newFrame.end(), oldFrame.begin(), oldFrame.end());
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
        std::vector<std::string> parameterNames;
        if (context->paramlist())
        {

            for (auto parameter : context->paramlist()->localvariable())
            {
                // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the % prefix
                const std::string rawString = parameter->getText();
                const std::string variableName = rawString.substr(1, rawString.size());

                parameterNames.push_back(variableName);
            }
        }

        const unsigned int statementCount = context->statement().size();

        InstructionSequence functionBody;
        for (unsigned int iteration = 0; iteration < statementCount; ++iteration)
        {
            std::vector<std::shared_ptr<Instruction>> bodyStatement = this->getCurrentInstructionFrame();
            functionBody.insert(functionBody.begin(), bodyStatement.begin(), bodyStatement.end());
            this->popInstructionFrame();
        }
        functionBody.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(0))); // Add an empty return if we hit end of control but nothing returned

        const std::string functionName = context->labelsingle()->getText();

        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();

        targetFrame.push_back(std::shared_ptr<Instruction>(new FunctionDeclarationInstruction(functionName, parameterNames, functionBody)));
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
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->LESS())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new LessThanInstruction()));
        }
        else
        {
            throw std::runtime_error("Unknown relational operator!");
        }
    }

    void Compiler::enterCall(TorqueParser::CallContext* context)
    {

    }

    void Compiler::exitCall(TorqueParser::CallContext* context)
    {
        const std::string calledFunctionName = context->labelsingle()->getText();
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        currentFrame.push_back(std::shared_ptr<Instruction>(new CallFunctionInstruction(calledFunctionName, context->expression().size())));
    }

    void Compiler::enterValue(TorqueParser::ValueContext* context)
    {

    }

    void Compiler::exitValue(TorqueParser::ValueContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        if (context->FLOAT())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushFloatInstruction(std::stof(context->FLOAT()->getText()))));
        }
        else if (context->STRING())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
            const std::string rawString = context->getText();
            const std::string stringContent = rawString.substr(1, rawString.size() - 2);
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(stringContent)));
        }
        else if (context->LABEL())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(context->getText())));
        }
        else if (context->INT())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoi(context->INT()->getText()))));
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
        else if (context->PLUSPLUS())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(1)));
            currentFrame.push_back(std::shared_ptr<Instruction>(new AddAssignmentInstruction()));
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

        if (context->ASSIGN())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new AssignmentInstruction()));
        }
        else if (context->ADDASSIGN())
        {
            currentFrame.push_back(std::shared_ptr<Instruction>(new AddAssignmentInstruction()));
        }
        else
        {
            throw std::runtime_error("Encountered unhandled assignment op type!");
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
        InstructionSequence generatedInstructions;
        while (!this->mInstructionStack.empty())
        {
            std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
            generatedInstructions.insert(generatedInstructions.begin(), currentFrame.begin(), currentFrame.end());
            this->popInstructionFrame();
        }
        mCurrentCodeBlock->addInstructions(generatedInstructions);
    }

    void Compiler::enterStatement(TorqueParser::StatementContext* context)
    {
        this->pushInstructionFrame();
    }

    void Compiler::exitStatement(TorqueParser::StatementContext* context)
    {

    }

    void Compiler::enterActionstatement(TorqueParser::ActionstatementContext* context)
    {

    }

    void Compiler::exitActionstatement(TorqueParser::ActionstatementContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        currentFrame.push_back(std::shared_ptr<Instruction>(new PopInstruction()));
    }

    void Compiler::enterWhilecontrol(TorqueParser::WhilecontrolContext* context)
    {

    }

    void Compiler::exitWhilecontrol(TorqueParser::WhilecontrolContext* context)
    {
        const unsigned int statementCount = context->statement().size();

        std::vector<std::shared_ptr<Instruction>> whileBody;
        for (unsigned int iteration = 0; iteration < statementCount; ++iteration)
        {
            std::vector<std::shared_ptr<Instruction>> bodyStatement = this->getCurrentInstructionFrame();
            whileBody.insert(whileBody.begin(), bodyStatement.begin(), bodyStatement.end());
            this->popInstructionFrame();
        }

        // Next frame should be expression
        std::vector<std::shared_ptr<Instruction>> whileExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();

        // Our expression should jump over our while body +2 (NOP and jump back) if false
        whileExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(whileBody.size() + 2)));

        // Our body should return to the expression
        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();

        const unsigned int jumpTarget = whileExpression.size() + whileBody.size();
        whileBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(-jumpTarget)));
        whileBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

        whileBody.push_back(std::shared_ptr<Instruction>(new PopLoopInstruction()));
        targetFrame.push_back(std::shared_ptr<Instruction>(new PushLoopInstruction(whileExpression.size() + whileBody.size())));

        // Set comments
        whileExpression[0]->mComment = "Begin While";
        whileBody[whileBody.size() - 1]->mComment = "End While";

        // Push generated instructions
        targetFrame.insert(targetFrame.end(), whileExpression.begin(), whileExpression.end());
        targetFrame.insert(targetFrame.end(), whileBody.begin(), whileBody.end());
    }

    void Compiler::enterControlexpression(TorqueParser::ControlexpressionContext* context)
    {
        this->pushInstructionFrame();
    }

    void Compiler::exitControlexpression(TorqueParser::ControlexpressionContext* context)
    {

    }

    void Compiler::enterTernary(TorqueParser::TernaryContext* context)
    {

    }

    void Compiler::exitTernary(TorqueParser::TernaryContext* context)
    {
        // A ternary is made up of three expressions
        std::vector<std::shared_ptr<Instruction>> falseExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();
        std::vector<std::shared_ptr<Instruction>> trueExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();

        std::vector<std::shared_ptr<Instruction>> statement = this->getCurrentInstructionFrame();
        this->popInstructionFrame();

        // We add a NOP to the false expressions for a target to jump to
        falseExpression.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

        // In the true expression we need to jump over the false expression
        trueExpression.push_back(std::shared_ptr<Instruction>(new JumpInstruction(falseExpression.size())));

        // Jump to the false expression if our expression is false
        statement.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(falseExpression.size() + 1)));

        // Push generated instructions back
        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();
        targetFrame.insert(targetFrame.end(), statement.begin(), statement.end());
        targetFrame.insert(targetFrame.end(), trueExpression.begin(), trueExpression.end());
        targetFrame.insert(targetFrame.end(), falseExpression.begin(), falseExpression.end());
    }

    void Compiler::enterForcontrol(TorqueParser::ForcontrolContext* context)
    {

    }

    void Compiler::exitForcontrol(TorqueParser::ForcontrolContext* context)
    {
        // A for control is made up of any number of statements and 3 expressions
        const unsigned int statementCount = context->statement().size();

        std::vector<std::shared_ptr<Instruction>> forBody;
        for (unsigned int iteration = 0; iteration < statementCount; ++iteration)
        {
            std::vector<std::shared_ptr<Instruction>> bodyStatement = this->getCurrentInstructionFrame();
            forBody.insert(forBody.begin(), bodyStatement.begin(), bodyStatement.end());
            this->popInstructionFrame();
        }

        // A for is made of 3 expressions
        std::vector<std::shared_ptr<Instruction>> advanceExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();
        std::vector<std::shared_ptr<Instruction>> conditionExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();

        std::vector<std::shared_ptr<Instruction>> initialExpression = this->getCurrentInstructionFrame();

        this->popInstructionFrame();

        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();

        // Push our initial expression out and pop its result
        initialExpression.push_back(std::shared_ptr<Instruction>(new PopInstruction()));
        targetFrame.insert(targetFrame.end(), initialExpression.begin(), initialExpression.end());

        // At the end of our loop, run the advance expression
        forBody.insert(forBody.end(), advanceExpression.begin(), advanceExpression.end());

        // Our body should return to the expression
        const unsigned int jumpTarget = conditionExpression.size() + forBody.size();
        forBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(-jumpTarget)));
        forBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

        // Check if our expression is false
        conditionExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(forBody.size())));

        forBody.push_back(std::shared_ptr<Instruction>(new PopLoopInstruction()));
        targetFrame.push_back(std::shared_ptr<Instruction>(new PushLoopInstruction(conditionExpression.size() + forBody.size())));

        // Set comments
        conditionExpression[0]->mComment = "Begin For";
        forBody[forBody.size() - 1]->mComment = "End For";

        // Output final code
        targetFrame.insert(targetFrame.end(), conditionExpression.begin(), conditionExpression.end());
        targetFrame.insert(targetFrame.end(), forBody.begin(), forBody.end());
    }

    void Compiler::enterSubreference(TorqueParser::SubreferenceContext* context)
    {

    }

    void Compiler::exitSubreference(TorqueParser::SubreferenceContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();

        targetFrame.push_back(std::shared_ptr<Instruction>(new SubReferenceInstruction(context->label()->getText())));
    }

    void Compiler::enterIfcontrol(TorqueParser::IfcontrolContext* context)
    {

    }

    void Compiler::exitIfcontrol(TorqueParser::IfcontrolContext* context)
    {
        // Load the else if present
        std::vector<std::shared_ptr<Instruction>> elseBody;
        if (context->elsecontrol())
        {
            unsigned int statementCount = context->elsecontrol()->statement().size();
            for (unsigned int iteration = 0; iteration < statementCount; ++iteration)
            {
                std::vector<std::shared_ptr<Instruction>> bodyStatement = this->getCurrentInstructionFrame();
                elseBody.insert(elseBody.begin(), bodyStatement.begin(), bodyStatement.end());
                this->popInstructionFrame();
            }
        }
        elseBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

        // Set comments
        elseBody[0]->mComment = "Begin Else";
        elseBody[elseBody.size() - 1]->mComment = "End Else";

        // Deal with else if's
        std::vector<std::shared_ptr<Instruction>> elseIfCode;
        unsigned int elseIfCount = context->elseifcontrol().size();
        for (unsigned int iteration = 0; iteration < elseIfCount; ++iteration)
        {
            unsigned int statementCount = context->elseifcontrol()[iteration]->statement().size();

            // Load all statements from else if
            std::vector<std::shared_ptr<Instruction>> elseIfBody;
            for (unsigned int elseIfIteration = 0; elseIfIteration < statementCount; ++elseIfIteration)
            {
                std::vector<std::shared_ptr<Instruction>> bodyStatement = this->getCurrentInstructionFrame();
                elseIfBody.insert(elseIfBody.begin(), bodyStatement.begin(), bodyStatement.end());
                this->popInstructionFrame();
            }

            // Load the expression
            std::vector<std::shared_ptr<Instruction>> elseIfExpression = this->getCurrentInstructionFrame();
            this->popInstructionFrame();

            // If the expression is false, jump to next statement or end
            elseIfExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(elseIfBody.size() + 2)));

            // The if body should skip over all instructions in the true branch
            elseIfBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(elseIfCode.size() + elseBody.size() + 1)));

            // Set comments
            elseIfExpression[0]->mComment = "Begin Else If";
            elseIfBody[elseIfBody.size() - 1]->mComment = "End Else If";

            elseIfCode.insert(elseIfCode.begin(), elseIfBody.begin(), elseIfBody.end());
            elseIfCode.insert(elseIfCode.begin(), elseIfExpression.begin(), elseIfExpression.end());
        }

        // Load the original if now
        std::vector<std::shared_ptr<Instruction>> ifBody;
        unsigned int statementCount = context->statement().size();

        for (unsigned int iteration = 0; iteration < statementCount; ++iteration)
        {
            std::vector<std::shared_ptr<Instruction>> bodyStatement = this->getCurrentInstructionFrame();
            ifBody.insert(ifBody.begin(), bodyStatement.begin(), bodyStatement.end());
            this->popInstructionFrame();
        }

        std::vector<std::shared_ptr<Instruction>> ifExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();

        // The if body should skip over all instructions in the true branch
        ifBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(elseIfCode.size() + elseBody.size() + 1)));

        // Make the expression jump over the body if our expression is false
        ifExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(ifBody.size() + 1)));

        // Set comments
        ifExpression[0]->mComment = "Begin If";
        ifBody[ifBody.size() - 1]->mComment = "End If";

        // Output code
        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();

        targetFrame.insert(targetFrame.end(), ifExpression.begin(), ifExpression.end());
        targetFrame.insert(targetFrame.end(), ifBody.begin(), ifBody.end());

        targetFrame.insert(targetFrame.end(), elseIfCode.begin(), elseIfCode.end());
        targetFrame.insert(targetFrame.end(), elseBody.begin(), elseBody.end());
    }

    void Compiler::enterReturncontrol(TorqueParser::ReturncontrolContext* context)
    {

    }

    void Compiler::exitReturncontrol(TorqueParser::ReturncontrolContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();
        targetFrame.push_back(std::shared_ptr<Instruction>(new ReturnInstruction()));
    }

    void Compiler::enterEquality(TorqueParser::EqualityContext* context)
    {

    }

    void Compiler::exitEquality(TorqueParser::EqualityContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& targetFrame = this->getCurrentInstructionFrame();

        if (context->EQUAL())
        {
            targetFrame.push_back(std::shared_ptr<Instruction>(new EqualsInstruction()));
        }
        else
        {
            throw std::runtime_error("Unhandled equality op!");
        }
    }

    void Compiler::enterArray(TorqueParser::ArrayContext* context)
    {

    }

    void Compiler::exitArray(TorqueParser::ArrayContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        std::string name = "";
        bool global = false;
        if (context->globalvariable())
        {
            global = true;
            name = context->globalvariable()->label()->getText();
        }
        else if (context->localvariable())
        {
            name = context->localvariable()->label()->getText();
        }
        else
        {
            throw std::runtime_error("Encountered unknown variable reference type in array!");
        }

        currentFrame.push_back(std::shared_ptr<Instruction>(new AccessArrayInstruction(name, context->expression().size(), global)));
    }

    void Compiler::enterDatablockdeclaration(TorqueParser::DatablockdeclarationContext* context)
    {
        throw std::runtime_error("Datablocks not Implemented Yet");
    }

    void Compiler::exitDatablockdeclaration(TorqueParser::DatablockdeclarationContext* context)
    {

    }

    void Compiler::enterPackagedeclaration(TorqueParser::PackagedeclarationContext* context)
    {
        throw std::runtime_error("Packages not Implemented Yet");
    }

    void Compiler::exitPackagedeclaration(TorqueParser::PackagedeclarationContext* context)
    {

    }

    void Compiler::enterSwitchcontrol(TorqueParser::SwitchcontrolContext* context)
    {

    }

    void Compiler::exitSwitchcontrol(TorqueParser::SwitchcontrolContext* context)
    {
        std::vector<std::shared_ptr<Instruction>> defaultCaseInstructions;
        if (context->defaultcase())
        {
            TorqueParser::DefaultcaseContext* defaultCase = context->defaultcase();

            // Load instructions for the default case
            std::vector<TorqueParser::StatementContext*> defaultStatements = defaultCase->statement();
            for (auto statement : defaultStatements)
            {
                std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
                defaultCaseInstructions.insert(defaultCaseInstructions.begin(), currentFrame.begin(), currentFrame.end());
                this->popInstructionFrame();
            }
        }

        // The switch structure is a little more complex - a set of expressions is associated with it so we need to track a list of expressions
        // and the body code
        struct SwitchCaseData
        {
            std::vector<std::shared_ptr<Instruction>> mCaseBody;
            std::vector<std::vector<std::shared_ptr<Instruction>>> mExpressions;
        };

        // Now enumerate all case statements
        std::vector<SwitchCaseData> caseData;
        std::vector<TorqueParser::SwitchcaseContext*> switchCases = context->switchcase();
        for (auto iterator = switchCases.rbegin(); iterator != switchCases.rend(); ++iterator)
        {
            caseData.push_back(SwitchCaseData());
            SwitchCaseData& currentCaseData = caseData.back();

            TorqueParser::SwitchcaseContext* caseContext = *iterator;

            // Load all statements from the case
            std::vector<TorqueParser::StatementContext*> caseStatements = caseContext->statement();
            for (auto statementIterator = caseStatements.begin(); statementIterator != caseStatements.end(); ++statementIterator)
            {
                std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
                currentCaseData.mCaseBody.insert(currentCaseData.mCaseBody.begin(), currentFrame.begin(), currentFrame.end());
                this->popInstructionFrame();
            }

            // Load all expressions
            std::vector<TorqueParser::ControlexpressionContext*> caseExpressions = caseContext->controlexpression();
            for (auto expressionIterator = caseExpressions.begin(); expressionIterator != caseExpressions.end(); ++expressionIterator)
            {
                currentCaseData.mExpressions.push_back(std::vector<std::shared_ptr<Instruction>>());
                std::vector<std::shared_ptr<Instruction>>& currentExpression = currentCaseData.mExpressions.back();

                std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
                currentExpression.insert(currentExpression.begin(), currentFrame.begin(), currentFrame.end());
                this->popInstructionFrame();
            }
        }

        // Finally load the expression to switch on
        std::vector<std::shared_ptr<Instruction>> switchExpression = this->getCurrentInstructionFrame();
        this->popInstructionFrame();

        // Once we know our expression, we need to cycle through our stored case data and generate handlers for each one
        for (SwitchCaseData& currentCaseData : caseData)
        {
            std::vector<std::shared_ptr<Instruction>> generatedConditions;

            // For each sub expression we push our expression
            const unsigned int expressionCount = currentCaseData.mExpressions.size();
            for (auto expressionIteration = currentCaseData.mExpressions.begin(); expressionIteration != currentCaseData.mExpressions.end(); ++expressionIteration)
            {
                std::vector<std::shared_ptr<Instruction>>& expression = *expressionIteration;

                // Check if the expression is true - if so, jump to body immediately unless we're the last check then we jump false over the body
                if (expressionIteration == currentCaseData.mExpressions.begin())
                {
                    generatedConditions.insert(generatedConditions.begin(), std::shared_ptr<Instruction>(new JumpFalseInstruction(currentCaseData.mCaseBody.size() + 2)));
                }
                else
                {
                    generatedConditions.insert(generatedConditions.begin(), std::shared_ptr<Instruction>(new JumpTrueInstruction(generatedConditions.size() + 1)));
                }

                generatedConditions.insert(generatedConditions.begin(), std::shared_ptr<Instruction>(new EqualsInstruction()));
                generatedConditions.insert(generatedConditions.begin(), switchExpression.begin(), switchExpression.end());
                generatedConditions.insert(generatedConditions.begin(), expression.begin(), expression.end());

                generatedConditions[0]->mComment = "Begin Case";
                generatedConditions[generatedConditions.size() - 1]->mComment = "End Case";
            }
            currentCaseData.mCaseBody.insert(currentCaseData.mCaseBody.begin(), generatedConditions.begin(), generatedConditions.end());
        }

        // Stick the default case entry at the end
        caseData.insert(caseData.begin(), SwitchCaseData());
        SwitchCaseData& defaultCaseData = caseData.front();
        defaultCaseData.mCaseBody.insert(defaultCaseData.mCaseBody.begin(), defaultCaseInstructions.begin(), defaultCaseInstructions.end());
        defaultCaseData.mCaseBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));
        defaultCaseData.mCaseBody[0]->mComment = "Begin Default Case";
        defaultCaseData.mCaseBody[defaultCaseData.mCaseBody.size() - 1]->mComment = "End Default Case";

        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        // Output code and insert jumps for true branches
        for (auto iterator = caseData.begin(); iterator != caseData.end(); ++iterator)
        {
            SwitchCaseData& currentCaseData = *iterator;

            // Generate a jump to end for the true case
            if (currentFrame.size() != 0)
            {
                currentCaseData.mCaseBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(currentFrame.size())));
            }
            else
            {
                currentCaseData.mCaseBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));
            }

            currentFrame.insert(currentFrame.begin(), currentCaseData.mCaseBody.begin(), currentCaseData.mCaseBody.end());
        }
    }

    void Compiler::enterNewobject(TorqueParser::NewobjectContext* context)
    {
        throw std::runtime_error("New Object Statements not Implemented Yet");
    }

    void Compiler::exitNewobject(TorqueParser::NewobjectContext* context)
    {

    }

    void Compiler::enterBreakcontrol(TorqueParser::BreakcontrolContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();
        currentFrame.push_back(std::shared_ptr<Instruction>(new BreakInstruction()));
    }

    void Compiler::exitBreakcontrol(TorqueParser::BreakcontrolContext* context)
    {

    }

    void Compiler::enterLogicalop(TorqueParser::LogicalopContext* context)
    {
        throw std::runtime_error("Logical Operators not Implemented Yet");
    }

    void Compiler::exitLogicalop(TorqueParser::LogicalopContext* context)
    {

    }

    void Compiler::enterLocalVariableValue(TorqueParser::LocalVariableValueContext* context)
    {

    }

    void Compiler::exitLocalVariableValue(TorqueParser::LocalVariableValueContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        const std::string variableName = context->localvariable()->label()->getText();
        currentFrame.push_back(std::shared_ptr<Instruction>(new PushLocalReferenceInstruction(variableName)));
    }

    void Compiler::enterGlobalVariableValue(TorqueParser::GlobalVariableValueContext* context)
    {

    }

    void Compiler::exitGlobalVariableValue(TorqueParser::GlobalVariableValueContext* context)
    {
        std::vector<std::shared_ptr<Instruction>>& currentFrame = this->getCurrentInstructionFrame();

        const std::string variableName = context->globalvariable()->label()->getText();
        currentFrame.push_back(std::shared_ptr<Instruction>(new PushGlobalReferenceInstruction(variableName)));
    }
}
