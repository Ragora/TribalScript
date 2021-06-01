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

#include <torquescript/astvisitor.hpp>
#include <torquescript/ast.hpp>

namespace TorqueScript
{
    antlrcpp::Any ASTVisitor::defaultResult()
    {
        return nullptr;
    }

    antlrcpp::Any ASTVisitor::aggregateResult(antlrcpp::Any aggregate, antlrcpp::Any nextResult)
    {
        return nextResult;
    }

    /*
        Visitor Routines =========================
    */

    antlrcpp::Any ASTVisitor::visitProgramNode(ProgramNode* program)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : program->mNodes)
        {
            result = this->aggregateResult(result, node->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitFunctionDeclarationNode(FunctionDeclarationNode* function)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : function->mBody)
        {
            result = this->aggregateResult(result, node->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitPackageDeclarationNode(PackageDeclarationNode* package)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : package->mFunctions)
        {
            result = this->aggregateResult(result, node->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitFunctionCallNode(FunctionCallNode* call)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : call->mParameters)
        {
            result = this->aggregateResult(result, node->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitSubFunctionCallNode(SubFunctionCallNode* call)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : call->mParameters)
        {
            result = this->aggregateResult(result, node->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitSubFieldNode(SubFieldNode* subfield)
    {
        antlrcpp::Any result = this->defaultResult();
        return this->aggregateResult(result, subfield->mTarget->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitAddNode(AddNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitSubtractNode(SubtractNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitMultiplyNode(MultiplyNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitDivideNode(DivideNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitConcatNode(ConcatNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitEqualsNode(EqualsNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitAssignmentNode(AssignmentNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitLessThanNode(LessThanNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, expression->mLeft->accept(this));
        return this->aggregateResult(result, expression->mRight->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitNegateNode(NegateNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        return this->aggregateResult(result, expression->mInner->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitNotNode(NotNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        return this->aggregateResult(result, expression->mInner->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitIncrementNode(IncrementNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        return this->aggregateResult(result, expression->mInner->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitDecrementNode(DecrementNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        return this->aggregateResult(result, expression->mInner->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitIntegerNode(IntegerNode* value)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitFloatNode(FloatNode* value)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitStringNode(StringNode* value)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitTaggedStringNode(TaggedStringNode* value)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitLocalVariableNode(LocalVariableNode* value)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitGlobalVariableNode(GlobalVariableNode* value)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitArrayNode(ArrayNode* array)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, array->mTarget->accept(this));

        for (ASTNode* index : array->mIndices)
        {
            result = this->aggregateResult(result, index->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitWhileNode(WhileNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, node->mExpression->accept(this));

        for (ASTNode* childNode : node->mBody)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitForNode(ForNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, node->mInitializer->accept(this));
        result = this->aggregateResult(result, node->mExpression->accept(this));
        result = this->aggregateResult(result, node->mAdvance->accept(this));

        for (ASTNode* childNode : node->mBody)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitReturnNode(ReturnNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        return this->aggregateResult(result, node->mExpression->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitBreakNode(BreakNode* node)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitTernaryNode(TernaryNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, node->mExpression->accept(this));
        result = this->aggregateResult(result, node->mTrueValue->accept(this));
        return this->aggregateResult(result, node->mFalseValue->accept(this));
    }

    antlrcpp::Any ASTVisitor::visitSwitchCaseNode(SwitchCaseNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : node->mCases)
        {
            result = this->aggregateResult(result, node->accept(this));
        }

        for (ASTNode* node : node->mBody)
        {
            result = this->aggregateResult(result, node->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitSwitchNode(SwitchNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, node->mExpression->accept(this));

        for (ASTNode* childNode : node->mCases)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }

        for (ASTNode* childNode : node->mDefaultBody)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitElseIfNode(ElseIfNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, node->mExpression->accept(this));

        for (ASTNode* childNode : node->mBody)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }

        return result;
    }

    antlrcpp::Any ASTVisitor::visitIfNode(IfNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        result = this->aggregateResult(result, node->mExpression->accept(this));

        for (ASTNode* childNode : node->mBody)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }

        for (ASTNode* childNode : node->mElseIfs)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }

        for (ASTNode* childNode : node->mElseBody)
        {
            result = this->aggregateResult(result, childNode->accept(this));
        }
        return result;
    }
}
