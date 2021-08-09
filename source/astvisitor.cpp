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

#include <torquescript/astvisitor.hpp>
#include <torquescript/ast.hpp>

namespace TorqueScript
{
    namespace AST
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

        antlrcpp::Any ASTVisitor::visitSubreferenceNode(AST::SubreferenceNode* reference)
        {
            antlrcpp::Any result = this->defaultResult();

            // Don't visit left as it is what triggered into us
            antlrcpp::Any childResult = reference->mTarget->accept(this);
            result = this->aggregateResult(result, childResult);

            // Right may be unassigned on rightmost
            if (reference->mRight)
            {
                antlrcpp::Any childResult = reference->mRight->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            return result;
        }

        antlrcpp::Any ASTVisitor::visitProgramNode(AST::ProgramNode* program)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* node : program->mNodes)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitFunctionDeclarationNode(AST::FunctionDeclarationNode* function)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* node : function->mBody)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitPackageDeclarationNode(AST::PackageDeclarationNode* package)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* node : package->mFunctions)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitFunctionCallNode(AST::FunctionCallNode* call)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* node : call->mParameters)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitSubFunctionCallNode(AST::SubFunctionCallNode* call)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* node : call->mParameters)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitSubFieldNode(AST::SubFieldNode* subfield)
        {
            antlrcpp::Any result = this->defaultResult();
            for (ASTNode* index : subfield->mIndices)
            {
                antlrcpp::Any indexResult = index->accept(this);
                result = this->aggregateResult(result, indexResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitLogicalOrNode(AST::LogicalOrNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitLogicalAndNode(AST::LogicalAndNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitAddNode(AST::AddNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitMinusNode(AST::MinusNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitModulusNode(AST::ModulusNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitSubtractNode(AST::SubtractNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitMultiplyNode(AST::MultiplyNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitDivideNode(AST::DivideNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitConcatNode(AST::ConcatNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitEqualsNode(AST::EqualsNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitNotEqualsNode(AST::NotEqualsNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitStringEqualsNode(AST::StringEqualsNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitStringNotEqualNode(AST::StringNotEqualNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitAssignmentNode(AST::AssignmentNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitLessThanNode(AST::LessThanNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitGreaterThanNode(AST::GreaterThanNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = expression->mLeft->accept(this);
            result = this->aggregateResult(result, childResult);
            childResult = expression->mRight->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitNegateNode(AST::NegateNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();
            antlrcpp::Any childResult = expression->mInner->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitNotNode(AST::NotNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();
            antlrcpp::Any childResult = expression->mInner->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitIncrementNode(AST::IncrementNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();
            antlrcpp::Any childResult = expression->mInner->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitDecrementNode(AST::DecrementNode* expression)
        {
            antlrcpp::Any result = this->defaultResult();
            antlrcpp::Any childResult = expression->mInner->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitIntegerNode(AST::IntegerNode* value)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitFloatNode(AST::FloatNode* value)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitStringNode(AST::StringNode* value)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitTaggedStringNode(AST::TaggedStringNode* value)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitLocalVariableNode(AST::LocalVariableNode* value)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitGlobalVariableNode(AST::GlobalVariableNode* value)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitArrayNode(AST::ArrayNode* array)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = array->mTarget->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* index : array->mIndices)
            {
                childResult = index->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitWhileNode(AST::WhileNode* node)
        {
            antlrcpp::Any result = this->defaultResult();
            antlrcpp::Any childResult = node->mExpression->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* childNode : node->mBody)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitForNode(AST::ForNode* node)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = node->mInitializer->accept(this);
            result = this->aggregateResult(result, childResult);

            childResult = node->mExpression->accept(this);
            result = this->aggregateResult(result, childResult);

            childResult = node->mAdvance->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* childNode : node->mBody)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitReturnNode(AST::ReturnNode* node)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = node->mExpression->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitBreakNode(AST::BreakNode* node)
        {
            return this->defaultResult();
        }

        antlrcpp::Any ASTVisitor::visitTernaryNode(AST::TernaryNode* node)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = node->mExpression->accept(this);
            result = this->aggregateResult(result, childResult);

            childResult = node->mTrueValue->accept(this);
            result = this->aggregateResult(result, childResult);

            childResult = node->mFalseValue->accept(this);
            return this->aggregateResult(result, childResult);
        }

        antlrcpp::Any ASTVisitor::visitSwitchCaseNode(AST::SwitchCaseNode* node)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* node : node->mCases)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            for (AST::ASTNode* node : node->mBody)
            {
                antlrcpp::Any childResult = node->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitSwitchNode(AST::SwitchNode* node)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = node->mExpression->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* childNode : node->mCases)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            for (AST::ASTNode* childNode : node->mDefaultBody)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitElseIfNode(AST::ElseIfNode* node)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = node->mExpression->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* childNode : node->mBody)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            return result;
        }

        antlrcpp::Any ASTVisitor::visitIfNode(AST::IfNode* node)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = node->mExpression->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* childNode : node->mBody)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            for (AST::ASTNode* childNode : node->mElseIfs)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            for (AST::ASTNode* childNode : node->mElseBody)
            {
                childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitDatablockDeclarationNode(AST::DatablockDeclarationNode* datablock)
        {
            antlrcpp::Any result = this->defaultResult();

            for (AST::ASTNode* field : datablock->mFields)
            {
                antlrcpp::Any childResult = field->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }

        antlrcpp::Any ASTVisitor::visitFieldAssignNode(AST::FieldAssignNode* node)
        {
            antlrcpp::Any result = this->defaultResult();
            for (AST::ASTNode* childNode : node->mFieldExpressions)
            {
                antlrcpp::Any childResult = childNode->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            antlrcpp::Any childResult = node->mRight->accept(this);
            result = this->aggregateResult(result, childResult);
            return result;
        }

        antlrcpp::Any ASTVisitor::visitObjectDeclarationNode(AST::ObjectDeclarationNode* object)
        {
            antlrcpp::Any result = this->defaultResult();

            antlrcpp::Any childResult = object->mName->accept(this);
            result = this->aggregateResult(result, childResult);

            childResult = object->mType->accept(this);
            result = this->aggregateResult(result, childResult);

            for (AST::ASTNode* field : object->mFields)
            {
                childResult = field->accept(this);
                result = this->aggregateResult(result, childResult);
            }

            for (AST::ObjectDeclarationNode* child : object->mChildren)
            {
                childResult = child->accept(this);
                result = this->aggregateResult(result, childResult);
            }
            return result;
        }
    }
}
