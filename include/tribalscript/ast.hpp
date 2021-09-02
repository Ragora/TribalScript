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

#include <cstddef>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include <tribalscript/astvisitor.hpp>
#include <tribalscript/stringtable.hpp>
#include <tribalscript/instructionsequence.hpp>

namespace TribalScript
{
    namespace AST
    {
        /**
         *  @brief Base AST node class. All AST nodes should derive from this
         *  class.
         */
        class ASTNode
        {
            public:
                virtual ~ASTNode() = default;

                virtual antlrcpp::Any accept(ASTVisitor* visitor) = 0;
        };

        class ProgramNode
        {
            public:
                explicit ProgramNode(std::vector<ASTNode*> nodes) : mNodes(std::move(nodes))
                {

                }

                virtual ~ProgramNode()
                {
                    for (ASTNode* node : mNodes)
                    {
                        delete node;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitProgramNode(this);
                }

                std::vector<ASTNode*> mNodes;
        };

        class FunctionDeclarationNode : public ASTNode
        {
            public:
                FunctionDeclarationNode(std::string space, std::string name, std::vector<std::string> parameterNames, std::vector<ASTNode*> body) :
                                        mNameSpace(std::move(space)), mName(std::move(name)), mParameterNames(std::move(parameterNames)), mBody(std::move(body))
                {

                }

                ~FunctionDeclarationNode() override
                {
                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitFunctionDeclarationNode(this);
                }

                std::string mNameSpace;
                std::string mName;
                std::vector<std::string> mParameterNames;
                std::vector<ASTNode*> mBody;
        };

        class PackageDeclarationNode : public ASTNode
        {
            public:
                PackageDeclarationNode(std::string name, std::vector<ASTNode*> functions) : mName(std::move(name)), mFunctions(std::move(functions))
                {

                }

                ~PackageDeclarationNode() override
                {
                    for (ASTNode* function : mFunctions)
                    {
                        delete function;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitPackageDeclarationNode(this);
                }

                std::string mName;
                std::vector<ASTNode*> mFunctions;
        };

        class FieldAssignNode : public ASTNode
        {
            public:
                FieldAssignNode(std::string fieldBaseName, std::vector<ASTNode*> fieldExpressions, ASTNode* right) :
                                mFieldBaseName(std::move(fieldBaseName)), mFieldExpressions(std::move(fieldExpressions)), mRight(right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitFieldAssignNode(this);
                }

                std::string mFieldBaseName;
                std::vector<ASTNode*> mFieldExpressions;
                ASTNode* mRight;
        };

        class ObjectDeclarationNode : public ASTNode
        {
            public:
                ObjectDeclarationNode(ASTNode* name, ASTNode* type, std::vector<ObjectDeclarationNode*> children, std::vector<ASTNode*> fields) :
                                      mName(name), mType(type), mChildren(std::move(children)), mFields(std::move(fields))
                {
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitObjectDeclarationNode(this);
                }

                ASTNode* mName;
                ASTNode* mType;

                std::vector<ObjectDeclarationNode*> mChildren;
                std::vector<ASTNode*> mFields;
        };

        class DatablockDeclarationNode : public ASTNode
        {
            public:
                DatablockDeclarationNode(std::string name, std::string type, std::string parentName, std::vector<ASTNode*> fields) :
                                        mName(std::move(name)), mType(std::move(type)), mParentName(std::move(parentName)), mFields(std::move(fields))
                {
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitDatablockDeclarationNode(this);
                }

                std::string mName;
                std::string mType;
                std::string mParentName;

                std::vector<ASTNode*> mFields;
        };

        class FunctionCallNode : public ASTNode
        {
            public:
                FunctionCallNode(std::string space, std::string name, std::vector<ASTNode*> parameters) :
                                mNameSpace(std::move(space)), mName(std::move(name)), mParameters(std::move(parameters))
                {

                }

                ~FunctionCallNode() override
                {
                    for (ASTNode* parameter : mParameters)
                    {
                        delete parameter;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitFunctionCallNode(this);
                }

                std::string mNameSpace;
                std::string mName;
                std::vector<ASTNode*> mParameters;
        };

        class SubFunctionCallNode : public ASTNode
        {
            public:
                SubFunctionCallNode(std::string name, std::vector<ASTNode*> parameters) :
                                    mName(std::move(name)), mParameters(std::move(parameters))
                {

                }

                ~SubFunctionCallNode() override
                {
                    for (ASTNode* parameter : mParameters)
                    {
                        delete parameter;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitSubFunctionCallNode(this);
                }

                std::string mName;
                std::vector<ASTNode*> mParameters;
        };

        class SubreferenceNode : public ASTNode
        {
            public:
                SubreferenceNode(ASTNode* left, ASTNode* target, ASTNode* right) : mLeft(left), mTarget(target), mRight(right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitSubreferenceNode(this);
                }

                ASTNode* mLeft;
                ASTNode* mRight;
                ASTNode* mTarget;
        };

        class SubFieldNode : public ASTNode
        {
            public:
                SubFieldNode(std::string name, std::vector<ASTNode*> indices) : mName(std::move(name)), mIndices(std::move(indices))
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitSubFieldNode(this);
                }

                std::string mName;
                std::vector<ASTNode*> mIndices;
        };

        class InfixExpressionNode : public ASTNode
        {
            public:
                InfixExpressionNode(ASTNode* left, ASTNode* right) : mLeft(left), mRight(right)
                {

                }

                ~InfixExpressionNode() override
                {
                    delete mLeft;
                    delete mRight;
                }


                ASTNode* mLeft;
                ASTNode* mRight;
        };

        class LogicalAndNode : public InfixExpressionNode
        {
            public:
                LogicalAndNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitLogicalAndNode(this);
                }
        };

        class LogicalOrNode : public InfixExpressionNode
        {
            public:
                LogicalOrNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitLogicalOrNode(this);
                }
        };

        class AddNode : public InfixExpressionNode
        {
            public:
                AddNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitAddNode(this);
                }
        };

        class MinusNode : public InfixExpressionNode
        {
            public:
                MinusNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitMinusNode(this);
                }
        };

        class ModulusNode : public InfixExpressionNode
        {
            public:
                ModulusNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitModulusNode(this);
                }
        };

        class SubtractNode : public InfixExpressionNode
        {
            public:
                SubtractNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitSubtractNode(this);
                }
        };

        class MultiplyNode : public InfixExpressionNode
        {
            public:
                MultiplyNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitMultiplyNode(this);
                }
        };

        class DivideNode : public InfixExpressionNode
        {
            public:
                DivideNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitDivideNode(this);
                }
        };

        class ConcatNode : public InfixExpressionNode
        {
            public:
                ConcatNode(ASTNode* left, ASTNode* right, std::string seperator) : InfixExpressionNode(left, right), mSeperator(std::move(seperator))
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitConcatNode(this);
                }

                std::string mSeperator;
        };

        class EqualsNode : public InfixExpressionNode
        {
            public:
                EqualsNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitEqualsNode(this);
                }
        };

        class NotEqualsNode : public InfixExpressionNode
        {
            public:
                NotEqualsNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitNotEqualsNode(this);
                }
        };

        class StringEqualsNode : public InfixExpressionNode
        {
            public:
                StringEqualsNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitStringEqualsNode(this);
                }
        };

        class StringNotEqualNode : public InfixExpressionNode
        {
        public:
            StringNotEqualNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }

            antlrcpp::Any accept(ASTVisitor* visitor) override
            {
                return visitor->visitStringNotEqualNode(this);
            }
        };

        class AssignmentNode : public InfixExpressionNode
        {
            public:
                AssignmentNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitAssignmentNode(this);
                }
        };

        class LessThanNode : public InfixExpressionNode
        {
            public:
                LessThanNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitLessThanNode(this);
                }
        };

        class GreaterThanNode : public InfixExpressionNode
        {
	        public:
	            GreaterThanNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
	            {

	            }

	            antlrcpp::Any accept(ASTVisitor* visitor) override
	            {
	                return visitor->visitGreaterThanNode(this);
	            }
        };

		class GreaterThanOrEqualNode : public InfixExpressionNode
		{
			public:
				GreaterThanOrEqualNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
				{

				}

				antlrcpp::Any accept(ASTVisitor* visitor) override
				{
					return visitor->visitGreaterThanOrEqualNode(this);
				}
		};

        class UnaryNode : public ASTNode
        {
            public:
                explicit UnaryNode(ASTNode* inner) : mInner(inner)
                {

                }

                ~UnaryNode() override
                {
                    delete mInner;
                }

                ASTNode* mInner;
        };

        class NegateNode : public UnaryNode
        {
            public:
                explicit NegateNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitNegateNode(this);
                }
        };

        class NotNode : public UnaryNode
        {
            public:
                explicit NotNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitNotNode(this);
                }
        };

        class IncrementNode : public UnaryNode
        {
            public:
                explicit IncrementNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitIncrementNode(this);
                }
        };

        class DecrementNode : public UnaryNode
        {
            public:
                explicit DecrementNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitDecrementNode(this);
                }
        };

        class ValueNode : public ASTNode
        {
            public:
        };

        class IntegerNode : public ValueNode
        {
            public:
                explicit IntegerNode(const int value) : mValue(value)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitIntegerNode(this);
                }

                int mValue;
        };

        class FloatNode : public ValueNode
        {
            public:
                explicit FloatNode(const float value) : mValue(value)
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitFloatNode(this);
                }

                float mValue;
        };

        class StringNode : public ValueNode
        {
            public:
                explicit StringNode(std::string value) : mValue(std::move(value))
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitStringNode(this);
                }

                std::string mValue;
        };

        class TaggedStringNode : public ValueNode
        {
            public:
                explicit TaggedStringNode(std::string value) : mValue(std::move(value))
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitTaggedStringNode(this);
                }

                std::string mValue;
        };

        class LocalVariableNode : public ValueNode
        {
            public:
                explicit LocalVariableNode(std::vector<std::string> name) : mName(std::move(name))
                {

                }

                std::string getName()
                {
                    std::string result;
                    for (const std::string& component : mName)
                    {
                        if (result.empty())
                        {
                            result = component;
                        }
                        else
                        {
                            result += "::" + component;
                        }
                    }
                    return result;
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitLocalVariableNode(this);
                }

                std::vector<std::string> mName;
        };

        class GlobalVariableNode : public ValueNode
        {
            public:
                explicit GlobalVariableNode(std::vector<std::string> name) : mName(std::move(name))
                {

                }

                std::string getName()
                {
                    std::string result;
                    for (const std::string& component : mName)
                    {
                        if (result.empty())
                        {
                            result = component;
                        }
                        else
                        {
                            result += "::" + component;
                        }
                    }
                    return result;
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitGlobalVariableNode(this);
                }

                std::vector<std::string> mName;
        };

        class ArrayNode : public ASTNode
        {
            public:
                ArrayNode(ASTNode* target, std::vector<ASTNode*> indices) :
                         mTarget(target), mIndices(std::move(indices))
                {

                }

                ~ArrayNode() override
                {
                    delete mTarget;

                    for (ASTNode* index : mIndices)
                    {
                        delete index;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitArrayNode(this);
                }

                ASTNode* mTarget;
                std::vector<ASTNode*> mIndices;
        };

        class WhileNode : public ASTNode
        {
            public:
                WhileNode(ASTNode* expression, std::vector<ASTNode*> body) : mExpression(expression), mBody(std::move(body))
                {

                }

                ~WhileNode() override
                {
                    delete mExpression;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitWhileNode(this);
                }

                ASTNode* mExpression;
                std::vector<ASTNode*> mBody;
        };

        class ForNode : public ASTNode
        {
            public:
                ForNode(ASTNode* initializer, ASTNode* expression, ASTNode* advance, std::vector<ASTNode*> body) :
                        mInitializer(initializer), mExpression(expression), mAdvance(advance), mBody(std::move(body))
                {

                }

                ~ForNode() override
                {
                    delete mInitializer;
                    delete mExpression;
                    delete mAdvance;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitForNode(this);
                }

                ASTNode* mInitializer;
                ASTNode* mExpression;
                ASTNode* mAdvance;
                std::vector<ASTNode*> mBody;
        };

        class ReturnNode : public ASTNode
        {
            public:
                explicit ReturnNode(ASTNode* expression) : mExpression(expression)
                {

                }

                ~ReturnNode() override
                {
                    delete mExpression;
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitReturnNode(this);
                }

                ASTNode* mExpression;
        };

        class BreakNode : public ASTNode
        {
            public:
                BreakNode() = default;

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitBreakNode(this);
                }
        };

        class ContinueNode : public ASTNode
        {
            public:
                ContinueNode() = default;

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitContinueNode(this);
                }
        };

        class TernaryNode : public ASTNode
        {
            public:
                TernaryNode(ASTNode* expression, ASTNode* trueValue, ASTNode* falseValue) :
                          mExpression(expression), mTrueValue(trueValue), mFalseValue(falseValue)
                {

                }

                ~TernaryNode() override
                {
                    delete mExpression;
                    delete mTrueValue;
                    delete mFalseValue;
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitTernaryNode(this);
                }

                ASTNode* mExpression;
                ASTNode* mTrueValue;
                ASTNode* mFalseValue;
        };

        class SwitchCaseNode : public ASTNode
        {
            public:
                SwitchCaseNode(std::vector<ASTNode*> cases, std::vector<ASTNode*> body) : mCases(std::move(cases)), mBody(std::move(body))
                {

                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitSwitchCaseNode(this);
                }

                std::vector<ASTNode*> mCases;
                std::vector<ASTNode*> mBody;
        };

        class SwitchNode : public ASTNode
        {
            public:
                SwitchNode(ASTNode* expression, std::vector<SwitchCaseNode*> cases, std::vector<ASTNode*> defaultBody) :
                          mExpression(expression), mCases(std::move(cases)), mDefaultBody(std::move(defaultBody))
                {

                }

                ~SwitchNode() override
                {
                    delete mExpression;

                    for (SwitchCaseNode* switchCase : mCases)
                    {
                        delete switchCase;
                    }

                    for (ASTNode* node : mDefaultBody)
                    {
                        delete node;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitSwitchNode(this);
                }

                ASTNode* mExpression;
                std::vector<SwitchCaseNode*> mCases;
                std::vector<ASTNode*> mDefaultBody;
        };

        class ElseIfNode : public ASTNode
        {
            public:
                ElseIfNode(ASTNode* expression, std::vector<ASTNode*> body) : mExpression(expression), mBody(std::move(body))
                {

                }

                ~ElseIfNode() override
                {
                    delete mExpression;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitElseIfNode(this);
                }

                ASTNode* mExpression;
                std::vector<ASTNode*> mBody;
        };

        class IfNode : public ASTNode
        {
            public:
                IfNode(ASTNode* expression, std::vector<ASTNode*> body, std::vector<ElseIfNode*> elseIfs, std::vector<ASTNode*> elseBody) :
                      mExpression(expression), mBody(std::move(body)), mElseIfs(std::move(elseIfs)), mElseBody(std::move(elseBody))
                {

                }

                ~IfNode() override
                {
                    delete mExpression;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }

                    for (ElseIfNode* elseIf : mElseIfs)
                    {
                        delete elseIf;
                    }

                    for (ASTNode* node : mElseBody)
                    {
                        delete node;
                    }
                }

                antlrcpp::Any accept(ASTVisitor* visitor) override
                {
                    return visitor->visitIfNode(this);
                }

                ASTNode* mExpression;
                std::vector<ASTNode*> mBody;
                std::vector<ElseIfNode*> mElseIfs;
                std::vector<ASTNode*> mElseBody;
        };
    }
}
