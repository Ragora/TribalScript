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

#include <torquescript/stringtable.hpp>
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
{
    class ASTNode
    {
        public:
            virtual InstructionSequence compile(StringTable* stringTable)
            {
                throw std::runtime_error("Not Implemented");
            }
    };

    class FunctionDeclarationNode : public ASTNode
    {
        public:
            FunctionDeclarationNode(const std::string space, const std::string& name, const std::vector<ASTNode*> body) :
                                    mNameSpace(space), mName(name), mBody(body)
            {

            }

            ~FunctionDeclarationNode()
            {
                for (ASTNode* node : mBody)
                {
                    delete node;
                }
            }

            virtual InstructionSequence compile(StringTable* stringTable) override
            {
                InstructionSequence out;

                InstructionSequence functionBody;
                for (ASTNode* node : mBody)
                {
                    InstructionSequence childInstructions = node->compile(stringTable);
                    functionBody.insert(functionBody.end(), childInstructions.begin(), childInstructions.end());
                }
                functionBody.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(0))); // Add an empty return if we hit end of control but nothing returned

                // Generate final declaration
                std::vector<std::string> parameterNames;
                out.push_back(std::shared_ptr<Instruction>(new FunctionDeclarationInstruction(PACKAGE_EMPTY, mNameSpace, mName, parameterNames, functionBody)));
                return out;
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

            ~PackageDeclarationNode()
            {
                for (FunctionDeclarationNode* function : mFunctions)
                {
                    delete function;
                }
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

            ~FunctionCallNode()
            {
                for (ASTNode* parameter : mParameters)
                {
                    delete parameter;
                }
            }

            virtual InstructionSequence compile(StringTable* stringTable) override
            {
                InstructionSequence out;

                // Ask all parameters to generate their code
                for (ASTNode* node : mParameters)
                {
                    InstructionSequence childInstructions = node->compile(stringTable);
                    out.insert(out.end(), childInstructions.begin(), childInstructions.end());
                }

                out.push_back(std::shared_ptr<Instruction>(new CallFunctionInstruction(mNameSpace, mName, mParameters.size())));
                return out;
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

            ~SubFunctionCallNode()
            {
                delete mTarget;

                for (ASTNode* parameter : mParameters)
                {
                    delete parameter;
                }
            }

            ASTNode* mTarget;
            std::string mName;
            std::vector<ASTNode*> mParameters;
    };

    class SubFieldNode : public ASTNode
    {
        public:
            SubFieldNode(ASTNode* target, const std::string& name) : mTarget(target), mName(name)
            {

            }

            ~SubFieldNode()
            {
                delete mTarget;
            }

            ASTNode* mTarget;
            std::string mName;
    };

    class ArrayNode : public ASTNode
    {
        public:
            ArrayNode(ASTNode* target, const std::vector<ASTNode*>& indices) :
                     mTarget(target), mIndices(indices)
            {

            }

            ~ArrayNode()
            {
                delete mTarget;

                for (ASTNode* index : mIndices)
                {
                    delete index;
                }
            }

            ASTNode* mTarget;
            std::vector<ASTNode*> mIndices;
    };


    class InfixExpressionNode : public ASTNode
    {
        public:
            InfixExpressionNode(ASTNode* left, ASTNode* right) : mLeft(left), mRight(right)
            {

            }

            ~InfixExpressionNode()
            {
                delete mLeft;
                delete mRight;
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

            virtual InstructionSequence compile(StringTable* stringTable) override
            {
                InstructionSequence out;

                InstructionSequence lhsCode = mLeft->compile(stringTable);
                InstructionSequence rhsCode = mRight->compile(stringTable);

                out.insert(out.end(), lhsCode.begin(), lhsCode.end());
                out.insert(out.end(), rhsCode.begin(), rhsCode.end());
                out.push_back(std::shared_ptr<Instruction>(new AddInstruction()));

                return out;
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

            virtual InstructionSequence compile(StringTable* stringTable) override
            {
                InstructionSequence out;

                InstructionSequence lhsCode = mLeft->compile(stringTable);
                InstructionSequence rhsCode = mRight->compile(stringTable);

                out.insert(out.end(), lhsCode.begin(), lhsCode.end());
                out.insert(out.end(), rhsCode.begin(), rhsCode.end());
                out.push_back(std::shared_ptr<Instruction>(new MultiplyInstruction()));

                return out;
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

    class EqualsNode : public InfixExpressionNode
    {
        public:
            EqualsNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class LessThanNode : public InfixExpressionNode
    {
        public:
            LessThanNode(ASTNode* left, ASTNode* right) : InfixExpressionNode(left, right)
            {

            }
    };

    class UnaryNode : public ASTNode
    {
        public:
            UnaryNode(ASTNode* inner) : mInner(inner)
            {

            }

            ~UnaryNode()
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
    };

    class NotNode : public UnaryNode
    {
        public:
            NotNode(ASTNode* inner) : UnaryNode(inner)
            {

            }
    };

    class IncrementNode : public UnaryNode
    {
        public:
            IncrementNode(ASTNode* inner) : UnaryNode(inner)
            {

            }
    };

    class DecrementNode : public UnaryNode
    {
        public:
            DecrementNode(ASTNode* inner) : UnaryNode(inner)
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

            virtual InstructionSequence compile(StringTable* stringTable) override
            {
                InstructionSequence out;
                out.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(mValue)));
                return out;
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

            ~WhileNode()
            {
                for (ASTNode* node : mBody)
                {
                    delete node;
                }
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

            ~ForNode()
            {
                delete mInitializer;
                delete mExpression;
                delete mAdvance;

                for (ASTNode* node : mBody)
                {
                    delete node;
                }
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

            ~ReturnNode()
            {
                if (mExpression)
                {
                    delete mExpression;
                }
            }

            ASTNode* mExpression;
    };

    class BreakNode : public ASTNode
    {
        public:
            BreakNode()
            {

            }
    };

    class TernaryNode : public ASTNode
    {
        public:
            TernaryNode(ASTNode* expression, ASTNode* trueValue, ASTNode* falseValue) :
                      mExpression(expression), mTrueValue(trueValue), mFalseValue(falseValue)
            {

            }

            ~TernaryNode()
            {
                delete mExpression;
                delete mTrueValue;
                delete mFalseValue;
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

            ~SwitchNode()
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

            ~ElseIfNode()
            {
                delete mExpression;

                for (ASTNode* node : mBody)
                {
                    delete node;
                }
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

            ~IfNode()
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

            ASTNode* mExpression;
            std::vector<ASTNode*> mBody;
            std::vector<ElseIfNode*> mElseIfs;
            std::vector<ASTNode*> mElseBody;
    };
}
