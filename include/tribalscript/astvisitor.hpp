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

#include "antlr4-runtime.h"

#include <vector>

namespace TribalScript
{
    namespace AST
    {
        class ASTNode;
        class ProgramNode;
        class FunctionDeclarationNode;
        class PackageDeclarationNode;
        class FunctionCallNode;
        class SubFunctionCallNode;
        class SubFieldNode;
        class AddNode;
        class SubtractNode;
        class MultiplyNode;
        class DivideNode;
        class ConcatNode;
        class EqualsNode;
        class AssignmentNode;
        class LessThanNode;
        class NegateNode;
        class NotNode;
        class IncrementNode;
        class DecrementNode;
        class IntegerNode;
        class FloatNode;
        class StringNode;
        class TaggedStringNode;
        class LocalVariableNode;
        class GlobalVariableNode;
        class ArrayNode;
        class WhileNode;
        class ForNode;
        class ReturnNode;
        class BreakNode;
        class TernaryNode;
        class SwitchCaseNode;
        class SwitchNode;
        class ElseIfNode;
        class IfNode;
        class LogicalAndNode;
        class LogicalOrNode;
        class DatablockDeclarationNode;
        class FieldAssignNode;
        class ObjectDeclarationNode;
        class NotEqualsNode;
        class StringEqualsNode;
        class ModulusNode;
        class MinusNode;
        class GreaterThanNode;
        class StringNotEqualNode;
        class SubreferenceNode;
        class ContinueNode;
		class GreaterThanOrEqualNode;
        class BitwiseOrNode;

        class ASTVisitor
        {
            public:
                virtual antlrcpp::Any defaultResult();
                virtual antlrcpp::Any aggregateResult(antlrcpp::Any& aggregate, antlrcpp::Any& nextResult);

                /*
                    Visitor Routines =========================
                */

                virtual antlrcpp::Any visitProgramNode(AST::ProgramNode* program);
                virtual antlrcpp::Any visitFunctionDeclarationNode(AST::FunctionDeclarationNode* function);
                virtual antlrcpp::Any visitPackageDeclarationNode(AST::PackageDeclarationNode* package);
                virtual antlrcpp::Any visitObjectDeclarationNode(AST::ObjectDeclarationNode* object);
                virtual antlrcpp::Any visitDatablockDeclarationNode(AST::DatablockDeclarationNode* datablock);
                virtual antlrcpp::Any visitFieldAssignNode(AST::FieldAssignNode* node);
                virtual antlrcpp::Any visitFunctionCallNode(AST::FunctionCallNode* call);
                virtual antlrcpp::Any visitSubFunctionCallNode(AST::SubFunctionCallNode* call);
                virtual antlrcpp::Any visitSubFieldNode(AST::SubFieldNode* subfield);
                virtual antlrcpp::Any visitAddNode(AST::AddNode* expression);
                virtual antlrcpp::Any visitBitwiseOrNode(AST::BitwiseOrNode* expression);
                virtual antlrcpp::Any visitMinusNode(AST::MinusNode* expression);
                virtual antlrcpp::Any visitModulusNode(AST::ModulusNode* expression);
                virtual antlrcpp::Any visitLogicalAndNode(AST::LogicalAndNode* expression);
                virtual antlrcpp::Any visitLogicalOrNode(AST::LogicalOrNode* expression);
                virtual antlrcpp::Any visitSubtractNode(AST::SubtractNode* expression);
                virtual antlrcpp::Any visitMultiplyNode(AST::MultiplyNode* expression);
                virtual antlrcpp::Any visitDivideNode(AST::DivideNode* expression);
                virtual antlrcpp::Any visitConcatNode(AST::ConcatNode* expression);
                virtual antlrcpp::Any visitEqualsNode(AST::EqualsNode* expression);
                virtual antlrcpp::Any visitSubreferenceNode(AST::SubreferenceNode* reference);
                virtual antlrcpp::Any visitNotEqualsNode(AST::NotEqualsNode* expression);
                virtual antlrcpp::Any visitStringEqualsNode(AST::StringEqualsNode* expression);
                virtual antlrcpp::Any visitAssignmentNode(AST::AssignmentNode* expression);
                virtual antlrcpp::Any visitLessThanNode(AST::LessThanNode* expression);
                virtual antlrcpp::Any visitGreaterThanNode(AST::GreaterThanNode* expression);
                virtual antlrcpp::Any visitNegateNode(AST::NegateNode* expression);
                virtual antlrcpp::Any visitNotNode(AST::NotNode* expression);
                virtual antlrcpp::Any visitIncrementNode(AST::IncrementNode* expression);
                virtual antlrcpp::Any visitDecrementNode(AST::DecrementNode* expression);
                virtual antlrcpp::Any visitIntegerNode(AST::IntegerNode* value);
                virtual antlrcpp::Any visitFloatNode(AST::FloatNode* value);
                virtual antlrcpp::Any visitStringNode(AST::StringNode* value);
                virtual antlrcpp::Any visitTaggedStringNode(AST::TaggedStringNode* value);
                virtual antlrcpp::Any visitLocalVariableNode(AST::LocalVariableNode* value);
                virtual antlrcpp::Any visitGlobalVariableNode(AST::GlobalVariableNode* value);
                virtual antlrcpp::Any visitArrayNode(AST::ArrayNode* array);
                virtual antlrcpp::Any visitWhileNode(AST::WhileNode* node);
                virtual antlrcpp::Any visitForNode(AST::ForNode* node);
                virtual antlrcpp::Any visitReturnNode(AST::ReturnNode* node);
                virtual antlrcpp::Any visitBreakNode(AST::BreakNode* node);
                virtual antlrcpp::Any visitTernaryNode(AST::TernaryNode* node);
                virtual antlrcpp::Any visitSwitchCaseNode(AST::SwitchCaseNode* node);
                virtual antlrcpp::Any visitSwitchNode(AST::SwitchNode* node);
                virtual antlrcpp::Any visitElseIfNode(AST::ElseIfNode* node);
                virtual antlrcpp::Any visitIfNode(AST::IfNode* node);
                virtual antlrcpp::Any visitContinueNode(AST::ContinueNode* expression);
                virtual antlrcpp::Any visitStringNotEqualNode(AST::StringNotEqualNode* expression);
				virtual antlrcpp::Any visitGreaterThanOrEqualNode(AST::GreaterThanOrEqualNode* expression);
        };
    }
}
