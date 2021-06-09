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
    Compiler::Compiler(const InterpreterConfiguration& config) : mConfig(config)
    {

    }

    CodeBlock* Compiler::compileStream(std::istream& input, StringTable* stringTable)
    {
        ParserErrorListener parserErrorListener;

        antlr4::ANTLRInputStream antlrStream(input);
        TorqueLexer lexer(&antlrStream);
        lexer.removeErrorListeners();
        lexer.addErrorListener(&parserErrorListener);

        // Setup our parser
        antlr4::CommonTokenStream stream(&lexer);
        TorqueParser parser(&stream);
        parser.removeErrorListeners();

        parser.addErrorListener(&parserErrorListener);

        // Instantiate the program and go
        AST::ASTBuilder visitor(stringTable);
        AST::ProgramNode* tree = visitor.visitProgram(parser.program()).as<AST::ProgramNode*>();

        // Did we receive any errors?
        if (parserErrorListener.getErrors().empty())
        {
            // Used for generation of instructions
            mStringTable = stringTable;
            InstructionSequence instructions = this->visitProgramNode(tree).as<InstructionSequence>();
            delete tree;

            CodeBlock* result = new CodeBlock(instructions);
            return result;
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
        std::unique_ptr<FileHandleBase> handle = mConfig.mPlatform->getFileHandle(path);
        handle->openForRead();

        if (handle->isOpen())
        {
            // Determine file size
            handle->seek(0, std::ios_base::end);
            const std::size_t fileSize = handle->tell();
            handle->seek(0, std::ios_base::beg);

            // Load file content
            std::string fileContent(fileSize, ' ');
            handle->read(&fileContent[0], fileSize);

            return this->compileString(fileContent, stringTable);
        }

        return nullptr;
    }

    antlrcpp::Any Compiler::defaultResult()
    {
        return InstructionSequence();
    }

    antlrcpp::Any Compiler::aggregateResult(antlrcpp::Any& aggregate, antlrcpp::Any& nextResult)
    {
        InstructionSequence result = aggregate.as<InstructionSequence>();
        InstructionSequence next = nextResult.as<InstructionSequence>();

        result.insert(result.end(), next.begin(), next.end());
        return result;
    }

    /*
        Compiler Routines ====================
    */
    antlrcpp::Any Compiler::visitFunctionCallNode(AST::FunctionCallNode* call)
    {
        InstructionSequence result;

        for (AST::ASTNode* node : call->mParameters)
        {
            InstructionSequence parameterCode = node->accept(this).as<InstructionSequence>();
            result.insert(result.end(), parameterCode.begin(), parameterCode.end());
        }
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::CallFunctionInstruction(call->mNameSpace, call->mName, call->mParameters.size())));
        return result;
    }

    antlrcpp::Any Compiler::visitPackageDeclarationNode(AST::PackageDeclarationNode* package)
    {
        mCurrentPackage = package->mName;

        antlrcpp::Any result = ASTVisitor::visitPackageDeclarationNode(package);

        mCurrentPackage = PACKAGE_EMPTY;
        return result;
    }

    antlrcpp::Any Compiler::visitFunctionDeclarationNode(AST::FunctionDeclarationNode* function)
    {
        InstructionSequence functionBody;
        for (AST::ASTNode* node : function->mBody)
        {
            InstructionSequence nodeInstructions = node->accept(this).as<InstructionSequence>();
            functionBody.insert(functionBody.end(), nodeInstructions.begin(), nodeInstructions.end());
        }
        functionBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushIntegerInstruction(0))); // Add an empty return if we hit end of control but nothing returned

        std::vector<std::string> parameterNames = function->mParameterNames;
        if (!mConfig.mCaseSensitive)
        {
            for (unsigned int iteration = 0; iteration < parameterNames.size(); ++iteration)
            {
                parameterNames[iteration] = toLowerCase(parameterNames[iteration]);
            }
        }

        InstructionSequence result;
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::FunctionDeclarationInstruction(mCurrentPackage, function->mNameSpace, function->mName, parameterNames, functionBody)));
        return result;
    }

    antlrcpp::Any Compiler::visitSubFunctionCallNode(AST::SubFunctionCallNode* call)
    {
        InstructionSequence result;

        InstructionSequence targetCode = call->mTarget->accept(this).as<InstructionSequence>();
        result.insert(result.end(), targetCode.begin(), targetCode.end());

        for (AST::ASTNode* node : call->mParameters)
        {
            InstructionSequence parameterCode = node->accept(this).as<InstructionSequence>();
            result.insert(result.begin(), parameterCode.begin(), parameterCode.end());
        }

        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::CallBoundFunctionInstruction(call->mName, call->mParameters.size())));
        return result;
    }

    antlrcpp::Any Compiler::visitAddNode(AST::AddNode* expression)
    {
        InstructionSequence result;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        result.insert(result.end(), lhsCode.begin(), lhsCode.end());
        result.insert(result.end(), rhsCode.begin(), rhsCode.end());
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::AddInstruction()));

        return result;
    }

    antlrcpp::Any Compiler::visitIntegerNode(AST::IntegerNode* value)
    {
        InstructionSequence result;
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushIntegerInstruction(value->mValue)));
        return result;
    }

    antlrcpp::Any Compiler::visitFloatNode(AST::FloatNode* value)
    {
        InstructionSequence result;

        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushFloatInstruction(value->mValue)));
        return result;
    }

    antlrcpp::Any Compiler::visitStringNode(AST::StringNode* value)
    {
        InstructionSequence result;

        const std::size_t stringID = mStringTable->getOrAssign(value->mValue);
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushStringInstruction(stringID)));
        return result;
    }

    antlrcpp::Any Compiler::visitTaggedStringNode(AST::TaggedStringNode* value)
    {
        InstructionSequence result;

        const std::size_t stringID = mStringTable->getOrAssign(value->mValue);
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushIntegerInstruction(stringID)));
        return result;
    }

    antlrcpp::Any Compiler::visitLocalVariableNode(AST::LocalVariableNode* value)
    {
        InstructionSequence out;

        // NOTE: For now we collapse the name into a single string for lookup
        std::string lookupName = value->getName();

        const std::size_t stringID = mStringTable->getOrAssign(mConfig.mCaseSensitive ? lookupName : toLowerCase(lookupName));
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushLocalReferenceInstruction(stringID)));
        return out;
    }

    antlrcpp::Any Compiler::visitGlobalVariableNode(AST::GlobalVariableNode* value)
    {
        InstructionSequence out;

        // NOTE: For now we collapse the name into a single string for lookup
        std::string lookupName = value->getName();

        const std::size_t stringID = mStringTable->getOrAssign(mConfig.mCaseSensitive ? lookupName : toLowerCase(lookupName));
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushGlobalReferenceInstruction(stringID)));
        return out;
    }

    antlrcpp::Any Compiler::visitAssignmentNode(AST::AssignmentNode* expression)
    {
        InstructionSequence result;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        result.insert(result.end(), lhsCode.begin(), lhsCode.end());
        result.insert(result.end(), rhsCode.begin(), rhsCode.end());
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::AssignmentInstruction()));

        return result;
    }

    antlrcpp::Any Compiler::visitLessThanNode(AST::LessThanNode* expression)
    {
        InstructionSequence result;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        result.insert(result.end(), lhsCode.begin(), lhsCode.end());
        result.insert(result.end(), rhsCode.begin(), rhsCode.end());
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::LessThanInstruction()));

        return result;
    }

    antlrcpp::Any Compiler::visitNegateNode(AST::NegateNode* expression)
    {
        InstructionSequence result;

        InstructionSequence innerCode = expression->mInner->accept(this).as<InstructionSequence>();

        result.insert(result.end(), innerCode.begin(), innerCode.end());
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::NegateInstruction()));

        return result;
    }

    antlrcpp::Any Compiler::visitIncrementNode(AST::IncrementNode* expression)
    {
        InstructionSequence result;
        InstructionSequence innerCode = expression->mInner->accept(this).as<InstructionSequence>();

        result.insert(result.end(), innerCode.begin(), innerCode.end());
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushIntegerInstruction(1)));
        result.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::AddAssignmentInstruction()));

        return result;
    }

    antlrcpp::Any Compiler::visitWhileNode(AST::WhileNode* node)
    {
        InstructionSequence out;

        InstructionSequence bodyCode;
        InstructionSequence expressionCode = node->mExpression->accept(this).as<InstructionSequence>();
        for (AST::ASTNode* bodyNode : node->mBody)
        {
            InstructionSequence childCode = bodyNode->accept(this).as<InstructionSequence>();
            bodyCode.insert(bodyCode.end(), childCode.begin(), childCode.end());
        }

        // Expression should jump over body if false (+2 added for the NOP and jump below)
        expressionCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpFalseInstruction(bodyCode.size() + 2)));

        // Body should jump back to the expression to reevaluate
        const int jumpTarget = -(bodyCode.size() + expressionCode.size());
        bodyCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpInstruction(jumpTarget)));

        // Add a NOP for a jump target
        bodyCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::NOPInstruction()));

        // Add loop trackers
        bodyCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PopLoopInstruction()));
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushLoopInstruction(expressionCode.size() + bodyCode.size())));

        out.insert(out.end(), expressionCode.begin(), expressionCode.end());
        out.insert(out.end(), bodyCode.begin(), bodyCode.end());


        return out;
    }

    antlrcpp::Any Compiler::visitForNode(AST::ForNode* node)
    {
        InstructionSequence out;
        InstructionSequence initializerCode = node->mInitializer->accept(this).as<InstructionSequence>();
        InstructionSequence expressionCode = node->mExpression->accept(this).as<InstructionSequence>();
        InstructionSequence advanceCode = node->mAdvance->accept(this).as<InstructionSequence>();

        InstructionSequence forBody;
        for (AST::ASTNode* bodyNode : node->mBody)
        {
            InstructionSequence childInstructions = bodyNode->accept(this).as<InstructionSequence>();
            forBody.insert(forBody.end(), childInstructions.begin(), childInstructions.end());
        }

        // At the end of the loop, run advance
        forBody.insert(forBody.end(), advanceCode.begin(), advanceCode.end());

        // Pop the result of our initializer so it doesn't corrupt the stack
        initializerCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PopInstruction()));

        // Our body should return to the expression
        const unsigned int jumpTarget = expressionCode.size() + forBody.size();
        forBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpInstruction(-jumpTarget)));
        forBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::NOPInstruction()));

        // Check if our expression is false
        expressionCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpFalseInstruction(forBody.size())));

        forBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PopLoopInstruction()));
        initializerCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushLoopInstruction(expressionCode.size() + forBody.size())));

        // Output final code
        out.insert(out.end(), initializerCode.begin(), initializerCode.end());
        out.insert(out.end(), expressionCode.begin(), expressionCode.end());
        out.insert(out.end(), forBody.begin(), forBody.end());

        return out;
    }

    antlrcpp::Any Compiler::visitBreakNode(AST::BreakNode* node)
    {
        InstructionSequence out;
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::BreakInstruction()));
        return out;
    }

    antlrcpp::Any Compiler::visitReturnNode(AST::ReturnNode* node)
    {
        InstructionSequence out;
        if (node->mExpression)
        {
            out = node->mExpression->accept(this).as<InstructionSequence>();
        }
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::ReturnInstruction()));
        return out;
    }

    antlrcpp::Any Compiler::visitTernaryNode(AST::TernaryNode* node)
    {
        InstructionSequence out;

        InstructionSequence expressionCode = node->mExpression->accept(this).as<InstructionSequence>();
        InstructionSequence trueValueCode = node->mTrueValue->accept(this).as<InstructionSequence>();
        InstructionSequence falseValueCode = node->mFalseValue->accept(this).as<InstructionSequence>();

        // We add a NOP to the false expressions for a target to jump to
        falseValueCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::NOPInstruction()));

        // In the true expression we need to jump over the false expression
        trueValueCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpInstruction(falseValueCode.size())));

        // Jump to the false expression if our expression is false
        expressionCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpFalseInstruction(falseValueCode.size() + 1)));

        out.insert(out.end(), expressionCode.begin(), expressionCode.end());
        out.insert(out.end(), trueValueCode.begin(), trueValueCode.end());
        out.insert(out.end(), falseValueCode.begin(), falseValueCode.end());

        return out;
    }

    antlrcpp::Any Compiler::visitSwitchNode(AST::SwitchNode* node)
    {
        InstructionSequence out;

        InstructionSequence expressionCode = node->mExpression->accept(this).as<InstructionSequence>();

        // NOTE: We intentionally process in reverse order due to needing to know how long existing code is to jump over
        // Add a NOP to jump to
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::NOPInstruction()));

        InstructionSequence defaultInstructions;
        for (AST::ASTNode* defaultNode : node->mDefaultBody)
        {
            InstructionSequence childInstructions = defaultNode->accept(this).as<InstructionSequence>();
            defaultInstructions.insert(defaultInstructions.end(), childInstructions.begin(), childInstructions.end());
        }
        out.insert(out.begin(), defaultInstructions.begin(), defaultInstructions.end());

        // Process all cases
        for (AST::SwitchCaseNode* caseNode : node->mCases)
        {
            InstructionSequence caseBody;
            for (AST::ASTNode* node : caseNode->mBody)
            {
                InstructionSequence childInstructions = node->accept(this).as<InstructionSequence>();
                caseBody.insert(caseBody.end(), childInstructions.begin(), childInstructions.end());
            }
            // If we enter this body we should skip over the rest of the instructions
            caseBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpInstruction(out.size())));

            // Generate a sequence of checks until something comes out to be true
            InstructionSequence caseExpressions;
            for (auto iterator = caseNode->mCases.begin(); iterator != caseNode->mCases.end(); ++iterator)
            {
                AST::ASTNode* currentCaseExpression = *iterator;
                InstructionSequence caseExpressionCode = currentCaseExpression->accept(this).as<InstructionSequence>();

                // Place our expression to check against and then check if equal
                caseExpressionCode.insert(caseExpressionCode.end(), expressionCode.begin(), expressionCode.end());
                caseExpressionCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::EqualsInstruction()));

                if (iterator != caseNode->mCases.begin())
                {
                    caseExpressionCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpTrueInstruction(caseExpressions.size() + 1)));
                }
                else
                {
                    caseExpressionCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpFalseInstruction(caseBody.size() + 1)));
                }

                caseExpressions.insert(caseExpressions.begin(), caseExpressionCode.begin(), caseExpressionCode.end());
            }

            out.insert(out.begin(), caseBody.begin(), caseBody.end());
            out.insert(out.begin(), caseExpressions.begin(), caseExpressions.end());
        }

        return out;
    }

    antlrcpp::Any Compiler::visitIfNode(AST::IfNode* node)
    {
        InstructionSequence out;

        // Generate else code
        InstructionSequence elseCode;
        for (AST::ASTNode* bodyNode : node->mElseBody)
        {
            InstructionSequence childInstructions = bodyNode->accept(this).as<InstructionSequence>();
            elseCode.insert(elseCode.end(), childInstructions.begin(), childInstructions.end());
        }
        elseCode.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::NOPInstruction())); // Add a NOP for jump targets
        out.insert(out.end(), elseCode.begin(), elseCode.end());

        // Generate all else if's
        for (AST::ElseIfNode* elseIf : node->mElseIfs)
        {
            InstructionSequence elseIfBody;

            for (AST::ASTNode* node : elseIf->mBody)
            {
                InstructionSequence childInstructions = node->accept(this).as<InstructionSequence>();
                elseIfBody.insert(elseIfBody.end(), childInstructions.begin(), childInstructions.end());
            }

            InstructionSequence elseIfExpression = elseIf->mExpression->accept(this).as<InstructionSequence>();

            // The expression must jump over our body if false
            elseIfExpression.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpFalseInstruction(elseIfBody.size() + 2)));

            // The body, when done, must jump over the remaining code
            elseIfBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpInstruction(out.size())));

            out.insert(out.begin(), elseIfBody.begin(), elseIfBody.end());
            out.insert(out.begin(), elseIfExpression.begin(), elseIfExpression.end());
        }

        // Generate primary if condition
        InstructionSequence ifExpression = node->mExpression->accept(this).as<InstructionSequence>();

        InstructionSequence ifBody;
        for (AST::ASTNode* bodyNode : node->mBody)
        {
            InstructionSequence childInstructions = bodyNode->accept(this).as<InstructionSequence>();
            ifBody.insert(ifBody.end(), childInstructions.begin(), childInstructions.end());
        }

        // The expression must jump over our body if false
        ifExpression.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpFalseInstruction(ifBody.size() + 2)));

        // The body, when done, must jump over the remaining code
        ifBody.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::JumpInstruction(out.size())));

        out.insert(out.begin(), ifBody.begin(), ifBody.end());
        out.insert(out.begin(), ifExpression.begin(), ifExpression.end());
        return out;
    }

    antlrcpp::Any Compiler::visitArrayNode(AST::ArrayNode* array)
    {
        InstructionSequence out;

        AST::LocalVariableNode* localVariable = dynamic_cast<AST::LocalVariableNode*>(array->mTarget);
        AST::GlobalVariableNode* globalVariable = dynamic_cast<AST::GlobalVariableNode*>(array->mTarget);

        assert(localVariable || globalVariable);
        std::string variableName = localVariable ? localVariable->getName() : globalVariable->getName();

        // Ask all indices to generate their code
        for (AST::ASTNode* node : array->mIndices)
        {
            InstructionSequence childInstructions = node->accept(this).as<InstructionSequence>();
            out.insert(out.end(), childInstructions.begin(), childInstructions.end());
        }

        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::AccessArrayInstruction(variableName, array->mIndices.size(), globalVariable != nullptr)));
        return out;
    }

    antlrcpp::Any Compiler::visitEqualsNode(AST::EqualsNode* expression)
    {
        InstructionSequence out;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        out.insert(out.end(), lhsCode.begin(), lhsCode.end());
        out.insert(out.end(), rhsCode.begin(), rhsCode.end());
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::EqualsInstruction()));

        return out;
    }

    antlrcpp::Any Compiler::visitConcatNode(AST::ConcatNode* expression)
    {
        InstructionSequence out;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        out.insert(out.end(), lhsCode.begin(), lhsCode.end());
        out.insert(out.end(), rhsCode.begin(), rhsCode.end());
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::ConcatInstruction()));

        return out;
    }

    antlrcpp::Any Compiler::visitDivideNode(AST::DivideNode* expression)
    {
        InstructionSequence out;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        out.insert(out.end(), lhsCode.begin(), lhsCode.end());
        out.insert(out.end(), rhsCode.begin(), rhsCode.end());
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::DivideInstruction()));

        return out;
    }

    antlrcpp::Any Compiler::visitMultiplyNode(AST::MultiplyNode* expression)
    {
        InstructionSequence out;

        InstructionSequence lhsCode = expression->mLeft->accept(this).as<InstructionSequence>();
        InstructionSequence rhsCode = expression->mRight->accept(this).as<InstructionSequence>();

        out.insert(out.end(), lhsCode.begin(), lhsCode.end());
        out.insert(out.end(), rhsCode.begin(), rhsCode.end());
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::MultiplyInstruction()));

        return out;
    }

    antlrcpp::Any Compiler::visitDatablockDeclarationNode(AST::DatablockDeclarationNode* datablock)
    {
        throw std::runtime_error("Datablocks not Implemented Yet");
    }

    antlrcpp::Any Compiler::visitFieldAssignNode(AST::FieldAssignNode* node)
    {
        InstructionSequence out;

        // Push base
        const std::size_t stringID = mStringTable->getOrAssign(node->mFieldBaseName);
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushStringInstruction(stringID)));

        // Push all array components
        for (AST::ASTNode* childNode : node->mFieldExpressions)
        {
            InstructionSequence childCode = childNode->accept(this).as<InstructionSequence>();
            out.insert(out.end(), childCode.begin(), childCode.end());
        }

        // Push the rvalue
        InstructionSequence rvalueCode = node->mRight->accept(this).as<InstructionSequence>();
        out.insert(out.end(), rvalueCode.begin(), rvalueCode.end());

        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushObjectFieldInstruction(node->mFieldExpressions.size())));
        return out;
    }

    antlrcpp::Any Compiler::visitObjectDeclarationNode(AST::ObjectDeclarationNode* object)
    {
        InstructionSequence out;

        // The stack should look something like:
        // ...
        // ObjectTypeName
        // ObjectName
        InstructionSequence typeNameCode = object->mType->accept(this).as<InstructionSequence>();
        out.insert(out.begin(), typeNameCode.begin(), typeNameCode.end());

        if (object->mName)
        {
            InstructionSequence nameCode = object->mName->accept(this).as<InstructionSequence>();
            out.insert(out.end(), nameCode.begin(), nameCode.end());
        }
        else
        {
            const std::size_t stringID = mStringTable->getOrAssign("");
            out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushStringInstruction(stringID)));
        }

        // Push Object
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PushObjectInstantiationInstruction()));

        // ... gen fields
        for (AST::ASTNode* field : object->mFields)
        {
            InstructionSequence fieldCode = field->accept(this).as<InstructionSequence>();
            out.insert(out.end(), fieldCode.begin(), fieldCode.end());
        }

        // ... gen children
        for (AST::ObjectDeclarationNode* child : object->mChildren)
        {
            InstructionSequence childCode = child->accept(this).as<InstructionSequence>();
            out.insert(out.end(), childCode.begin(), childCode.end());
        }

        // Pop object
        out.push_back(std::shared_ptr<Instructions::Instruction>(new Instructions::PopObjectInstantiationInstruction()));

            /*
            *                 ASTNode* mName;
                ASTNode* mType;

                std::vector<ObjectDeclarationNode*> mChildren;
                std::vector<ASTNode*> mFields;
            */

        return out;
    }
}
