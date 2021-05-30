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

namespace TorqueScript
{
    class ASTNode
    {
        public:
            virtual ~ASTNode() = default;
    };

    class ProgramNode : public ASTNode
    {
        public:
            ProgramNode(const std::vector<ASTNode*>& children) : mChildren(children)
            {

            }

            std::vector<ASTNode*> mChildren;
    };

    class FunctionDeclarationNode : public ASTNode
    {
        public:
            FunctionDeclarationNode(const std::string space, const std::string& name, const std::vector<ASTNode*> body) :
                                    mNameSpace(space), mName(name), mBody(body)
            {

            }

            std::string mNameSpace;
            std::string mName;
            std::vector<ASTNode*> mBody;
    };

    class PackageDeclarationNode : public ASTNode
    {
        public:
            PackageDeclarationNode(const std::string& name, const std::vector<FunctionDeclarationNode*>& functions) : mName(name), mFunctions(functions)
            {

            }

            std::string mName;
            std::vector<FunctionDeclarationNode*> mFunctions;
    };

    class FunctionCallNode : public ASTNode
    {
        public:
            FunctionCallNode(const std::string& space, const std::string& name, const std::vector<ASTNode*> parameters) :
                            mNameSpace(space), mName(name), mParameters(parameters)
            {

            }

            std::string mNameSpace;
            std::string mName;
            std::vector<ASTNode*> mParameters;
    };

    class InfixExpressionNode : public ASTNode
    {
        public:
            InfixExpressionNode(ASTNode* left, ASTNode* right) : mLeft(left), mRight(right)
            {

            }

            ASTNode* mLeft;
            ASTNode* mRight;
    };

    class AddNode : public InfixExpressionNode
    {
        public:
            AddNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class SubtractNode : public InfixExpressionNode
    {
        public:
            SubtractNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class MultiplyNode : public InfixExpressionNode
    {
        public:
            MultiplyNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class DivideNode : public InfixExpressionNode
    {
        public:
            DivideNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class ConcatNode : public InfixExpressionNode
    {
        public:
            ConcatNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class UnaryNode : public ASTNode
    {
        public:
            UnaryNode(ASTNode* inner) : mInner(inner)
            {

            }

            ASTNode* mInner;
    };

    class NegateNode : public UnaryNode
    {
        public:
            NegateNode(ASTNode* inner) : UnaryNode(inner)
            {

            }
    };

    class NotNode : public UnaryNode
    {
        public:
            NotNode(ASTNode* inner) : UnaryNode(inner)
            {

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

            int mValue;
    };

    class FloatNode : public ValueNode
    {
        public:
            FloatNode(const float value) : mValue(value)
            {

            }

            float mValue;
    };

    class StringNode : public ValueNode
    {
        public:
            StringNode(const std::string& value) : mValue(value)
            {

            }

            std::string mValue;
    };

    class TaggedStringNode : public ValueNode
    {
        public:
            TaggedStringNode(const std::string& value) : mValue(value)
            {

            }

            std::string mValue;
    };

    class LocalVariableNode : public ValueNode
    {
        public:
            LocalVariableNode(const std::vector<std::string>& name) : mName(name)
            {

            }

            std::vector<std::string> mName;
    };

    class GlobalVariableNode : public ValueNode
    {
        public:
            GlobalVariableNode(const std::vector<std::string>& name) : mName(name)
            {

            }

            std::vector<std::string> mName;
    };

    class WhileNode : public ASTNode
    {
        public:
            WhileNode(ASTNode* expression, const std::vector<ASTNode*>& body) : mExpression(expression), mBody(body)
            {

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

            ASTNode* mInitializer;
            ASTNode* mExpression;
            ASTNode* mAdvance;
            std::vector<ASTNode*> mBody;
    };
}
