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

    antlrcpp::Any ASTVisitor::aggregateResult(antlrcpp::Any& aggregate, antlrcpp::Any& nextResult)
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
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitFunctionDeclarationNode(FunctionDeclarationNode* function)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : function->mBody)
        {
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitPackageDeclarationNode(PackageDeclarationNode* package)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : package->mFunctions)
        {
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitFunctionCallNode(FunctionCallNode* call)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : call->mParameters)
        {
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitSubFunctionCallNode(SubFunctionCallNode* call)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : call->mParameters)
        {
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitSubFieldNode(SubFieldNode* subfield)
    {
        antlrcpp::Any result = this->defaultResult();
        antlrcpp::Any childResult = subfield->mTarget->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitAddNode(AddNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitSubtractNode(SubtractNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitMultiplyNode(MultiplyNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitDivideNode(DivideNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitConcatNode(ConcatNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitEqualsNode(EqualsNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitAssignmentNode(AssignmentNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitLessThanNode(LessThanNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = expression->mLeft->accept(this);
        result = this->aggregateResult(result, childResult);
        childResult = expression->mRight->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitNegateNode(NegateNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        antlrcpp::Any childResult = expression->mInner->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitNotNode(NotNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        antlrcpp::Any childResult = expression->mInner->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitIncrementNode(IncrementNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        antlrcpp::Any childResult = expression->mInner->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitDecrementNode(DecrementNode* expression)
    {
        antlrcpp::Any result = this->defaultResult();
        antlrcpp::Any childResult = expression->mInner->accept(this);
        return this->aggregateResult(result, childResult);
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

        antlrcpp::Any childResult = array->mTarget->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* index : array->mIndices)
        {
            childResult = index->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitWhileNode(WhileNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        antlrcpp::Any childResult = node->mExpression->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* childNode : node->mBody)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitForNode(ForNode* node)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = node->mInitializer->accept(this);
        result = this->aggregateResult(result, childResult);

        childResult = node->mExpression->accept(this);
        result = this->aggregateResult(result, childResult);

        childResult = node->mAdvance->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* childNode : node->mBody)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitReturnNode(ReturnNode* node)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = node->mExpression->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitBreakNode(BreakNode* node)
    {
        return this->defaultResult();
    }

    antlrcpp::Any ASTVisitor::visitTernaryNode(TernaryNode* node)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = node->mExpression->accept(this);
        result = this->aggregateResult(result, childResult);

        childResult = node->mTrueValue->accept(this);
        result = this->aggregateResult(result, childResult);

        childResult = node->mFalseValue->accept(this);
        return this->aggregateResult(result, childResult);
    }

    antlrcpp::Any ASTVisitor::visitSwitchCaseNode(SwitchCaseNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* node : node->mCases)
        {
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        for (ASTNode* node : node->mBody)
        {
            antlrcpp::Any childResult = node->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitSwitchNode(SwitchNode* node)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = node->mExpression->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* childNode : node->mCases)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        for (ASTNode* childNode : node->mDefaultBody)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitElseIfNode(ElseIfNode* node)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = node->mExpression->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* childNode : node->mBody)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        return result;
    }

    antlrcpp::Any ASTVisitor::visitIfNode(IfNode* node)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = node->mExpression->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* childNode : node->mBody)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        for (ASTNode* childNode : node->mElseIfs)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        for (ASTNode* childNode : node->mElseBody)
        {
            childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitDatablockDeclarationNode(DatablockDeclarationNode* datablock)
    {
        antlrcpp::Any result = this->defaultResult();

        for (ASTNode* field : datablock->mFields)
        {
            antlrcpp::Any childResult = field->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }

    antlrcpp::Any ASTVisitor::visitFieldAssignNode(FieldAssignNode* node)
    {
        antlrcpp::Any result = this->defaultResult();
        for (ASTNode* childNode : node->mFieldExpressions)
        {
            antlrcpp::Any childResult = childNode->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        antlrcpp::Any childResult = node->mRight->accept(this);
        result = this->aggregateResult(result, childResult);
        return result;
    }

    antlrcpp::Any ASTVisitor::visitObjectDeclarationNode(ObjectDeclarationNode* object)
    {
        antlrcpp::Any result = this->defaultResult();

        antlrcpp::Any childResult = object->mName->accept(this);
        result = this->aggregateResult(result, childResult);

        childResult = object->mType->accept(this);
        result = this->aggregateResult(result, childResult);

        for (ASTNode* field : object->mFields)
        {
            childResult = field->accept(this);
            result = this->aggregateResult(result, childResult);
        }

        for (ObjectDeclarationNode* child : object->mChildren)
        {
            childResult = child->accept(this);
            result = this->aggregateResult(result, childResult);
        }
        return result;
    }
}
