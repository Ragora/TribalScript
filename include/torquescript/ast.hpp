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

#include <torquescript/astvisitor.hpp>
#include <torquescript/stringtable.hpp>
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
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
                virtual ~ASTNode()
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor) = 0;
        };

        class ProgramNode
        {
            public:
                ProgramNode(const std::vector<ASTNode*>& nodes) : mNodes(nodes)
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
                FunctionDeclarationNode(const std::string space, const std::string& name, const std::vector<std::string>& parameterNames, const std::vector<ASTNode*> body) :
                                        mNameSpace(space), mName(name), mParameterNames(parameterNames), mBody(body)
                {

                }

                virtual ~FunctionDeclarationNode()
                {
                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                PackageDeclarationNode(const std::string& name, const std::vector<ASTNode*>& functions) : mName(name), mFunctions(functions)
                {

                }

                virtual ~PackageDeclarationNode()
                {
                    for (ASTNode* function : mFunctions)
                    {
                        delete function;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitPackageDeclarationNode(this);
                }

                std::string mName;
                std::vector<ASTNode*> mFunctions;
        };

        class FieldAssignNode : public ASTNode
        {
            public:
                FieldAssignNode(const std::string& fieldBaseName, const std::vector<ASTNode*>& fieldExpressions, ASTNode* right) : mFieldBaseName(fieldBaseName), mFieldExpressions(fieldExpressions), mRight(right)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                ObjectDeclarationNode(ASTNode* name, ASTNode* type, const std::vector<ObjectDeclarationNode*>& children, const std::vector<ASTNode*>& fields) : mName(name), mType(type), mChildren(children), mFields(fields)
                {
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                DatablockDeclarationNode(const std::string& name, const std::string& type, const std::string& parentName, const std::vector<ASTNode*>& fields) : mName(name), mType(type), mParentName(parentName), mFields(fields)
                {
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                FunctionCallNode(const std::string& space, const std::string& name, const std::vector<ASTNode*> parameters) :
                                mNameSpace(space), mName(name), mParameters(parameters)
                {

                }

                virtual ~FunctionCallNode()
                {
                    for (ASTNode* parameter : mParameters)
                    {
                        delete parameter;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                SubFunctionCallNode(ASTNode* target, const std::string& name, const std::vector<ASTNode*> parameters) :
                                    mTarget(target), mName(name), mParameters(parameters)
                {

                }

                virtual ~SubFunctionCallNode()
                {
                    delete mTarget;

                    for (ASTNode* parameter : mParameters)
                    {
                        delete parameter;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitSubFunctionCallNode(this);
                }

                ASTNode* mTarget;
                std::string mName;
                std::vector<ASTNode*> mParameters;
        };

        class SubFieldNode : public ASTNode
        {
            public:
                SubFieldNode(ASTNode* target, const std::string& name, const std::vector<ASTNode*>& indices) : mTarget(target), mName(name), mIndices(indices)
                {

                }

                virtual ~SubFieldNode()
                {
                    delete mTarget;
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitSubFieldNode(this);
                }

                ASTNode* mTarget;
                std::string mName;
                std::vector<ASTNode*> mIndices;
        };

        class InfixExpressionNode : public ASTNode
        {
            public:
                InfixExpressionNode(ASTNode* left, ASTNode* right) : mLeft(left), mRight(right)
                {

                }

                virtual ~InfixExpressionNode()
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitAddNode(this);
                }
        };

        class SubtractNode : public InfixExpressionNode
        {
            public:
                SubtractNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitDivideNode(this);
                }
        };

        class ConcatNode : public InfixExpressionNode
        {
            public:
                ConcatNode(ASTNode* left, ASTNode* right, const std::string& seperator) : InfixExpressionNode(left, right), mSeperator(seperator)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitEqualsNode(this);
                }
        };

        class AssignmentNode : public InfixExpressionNode
        {
            public:
                AssignmentNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitLessThanNode(this);
                }
        };

        class UnaryNode : public ASTNode
        {
            public:
                UnaryNode(ASTNode* inner) : mInner(inner)
                {

                }

                virtual ~UnaryNode()
                {
                    delete mInner;
                }

                ASTNode* mInner;
        };

        class NegateNode : public UnaryNode
        {
            public:
                NegateNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitNegateNode(this);
                }
        };

        class NotNode : public UnaryNode
        {
            public:
                NotNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitNotNode(this);
                }
        };

        class IncrementNode : public UnaryNode
        {
            public:
                IncrementNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitIncrementNode(this);
                }
        };

        class DecrementNode : public UnaryNode
        {
            public:
                DecrementNode(ASTNode* inner) : UnaryNode(inner)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                IntegerNode(const int value) : mValue(value)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitIntegerNode(this);
                }

                int mValue;
        };

        class FloatNode : public ValueNode
        {
            public:
                FloatNode(const float value) : mValue(value)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitFloatNode(this);
                }

                float mValue;
        };

        class StringNode : public ValueNode
        {
            public:
                StringNode(const std::string& value) : mValue(value)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitStringNode(this);
                }

                std::string mValue;
        };

        class TaggedStringNode : public ValueNode
        {
            public:
                TaggedStringNode(const std::string& value) : mValue(value)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitTaggedStringNode(this);
                }

                std::string mValue;
        };

        class LocalVariableNode : public ValueNode
        {
            public:
                LocalVariableNode(const std::vector<std::string>& name) : mName(name)
                {

                }

                std::string getName()
                {
                    std::string result = "";
                    for (const std::string& component : mName)
                    {
                        if (result.size() == 0)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitLocalVariableNode(this);
                }

                std::vector<std::string> mName;
        };

        class GlobalVariableNode : public ValueNode
        {
            public:
                GlobalVariableNode(const std::vector<std::string>& name) : mName(name)
                {

                }

                std::string getName()
                {
                    std::string result = "";
                    for (const std::string& component : mName)
                    {
                        if (result.size() == 0)
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitGlobalVariableNode(this);
                }

                std::vector<std::string> mName;
        };

        class ArrayNode : public ASTNode
        {
            public:
                ArrayNode(ASTNode* target, const std::vector<ASTNode*>& indices) :
                         mTarget(target), mIndices(indices)
                {

                }

                virtual ~ArrayNode()
                {
                    delete mTarget;

                    for (ASTNode* index : mIndices)
                    {
                        delete index;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitArrayNode(this);
                }

                ASTNode* mTarget;
                std::vector<ASTNode*> mIndices;
        };

        class WhileNode : public ASTNode
        {
            public:
                WhileNode(ASTNode* expression, const std::vector<ASTNode*>& body) : mExpression(expression), mBody(body)
                {

                }

                virtual ~WhileNode()
                {
                    delete mExpression;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitWhileNode(this);
                }

                ASTNode* mExpression;
                std::vector<ASTNode*> mBody;
        };

        class ForNode : public ASTNode
        {
            public:
                ForNode(ASTNode* initializer, ASTNode* expression, ASTNode* advance, const std::vector<ASTNode*>& body) :
                        mInitializer(initializer), mExpression(expression), mAdvance(advance), mBody(body)
                {

                }

                virtual ~ForNode()
                {
                    delete mInitializer;
                    delete mExpression;
                    delete mAdvance;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                ReturnNode(ASTNode* expression) : mExpression(expression)
                {

                }

                virtual ~ReturnNode()
                {
                    if (mExpression)
                    {
                        delete mExpression;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitReturnNode(this);
                }

                ASTNode* mExpression;
        };

        class BreakNode : public ASTNode
        {
            public:
                BreakNode()
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitBreakNode(this);
                }
        };

        class TernaryNode : public ASTNode
        {
            public:
                TernaryNode(ASTNode* expression, ASTNode* trueValue, ASTNode* falseValue) :
                          mExpression(expression), mTrueValue(trueValue), mFalseValue(falseValue)
                {

                }

                virtual ~TernaryNode()
                {
                    delete mExpression;
                    delete mTrueValue;
                    delete mFalseValue;
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                SwitchCaseNode(const std::vector<ASTNode*> cases, const std::vector<ASTNode*> body) : mCases(cases), mBody(body)
                {

                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitSwitchCaseNode(this);
                }

                std::vector<ASTNode*> mCases;
                std::vector<ASTNode*> mBody;
        };

        class SwitchNode : public ASTNode
        {
            public:
                SwitchNode(ASTNode* expression, const std::vector<SwitchCaseNode*> cases, const std::vector<ASTNode*> defaultBody) :
                          mExpression(expression), mCases(cases), mDefaultBody(defaultBody)
                {

                }

                virtual ~SwitchNode()
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
                ElseIfNode(ASTNode* expression, std::vector<ASTNode*> body) : mExpression(expression), mBody(body)
                {

                }

                virtual ~ElseIfNode()
                {
                    delete mExpression;

                    for (ASTNode* node : mBody)
                    {
                        delete node;
                    }
                }

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
                {
                    return visitor->visitElseIfNode(this);
                }

                ASTNode* mExpression;
                std::vector<ASTNode*> mBody;
        };

        class IfNode : public ASTNode
        {
            public:
                IfNode(ASTNode* expression, const std::vector<ASTNode*>& body, const std::vector<ElseIfNode*>& elseIfs, const std::vector<ASTNode*>& elseBody) :
                      mExpression(expression), mBody(body), mElseIfs(elseIfs), mElseBody(elseBody)
                {

                }

                virtual ~IfNode()
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

                virtual antlrcpp::Any accept(ASTVisitor* visitor)
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
