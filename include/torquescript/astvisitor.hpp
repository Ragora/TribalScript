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

namespace TorqueScript
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

    class ASTVisitor
    {
        public:
            virtual antlrcpp::Any defaultResult();
            virtual antlrcpp::Any aggregateResult(antlrcpp::Any aggregate, antlrcpp::Any nextResult);

            /*
                Visitor Routines =========================
            */

            virtual antlrcpp::Any visitProgramNode(ProgramNode* program);
            virtual antlrcpp::Any visitFunctionDeclarationNode(FunctionDeclarationNode* function);
            virtual antlrcpp::Any visitPackageDeclarationNode(PackageDeclarationNode* package);
            virtual antlrcpp::Any visitFunctionCallNode(FunctionCallNode* call);
            virtual antlrcpp::Any visitSubFunctionCallNode(SubFunctionCallNode* call);
            virtual antlrcpp::Any visitSubFieldNode(SubFieldNode* subfield);
            virtual antlrcpp::Any visitAddNode(AddNode* expression);
            virtual antlrcpp::Any visitSubtractNode(SubtractNode* expression);
            virtual antlrcpp::Any visitMultiplyNode(MultiplyNode* expression);
            virtual antlrcpp::Any visitDivideNode(DivideNode* expression);
            virtual antlrcpp::Any visitConcatNode(ConcatNode* expression);
            virtual antlrcpp::Any visitEqualsNode(EqualsNode* expression);
            virtual antlrcpp::Any visitAssignmentNode(AssignmentNode* expression);
            virtual antlrcpp::Any visitLessThanNode(LessThanNode* expression);
            virtual antlrcpp::Any visitNegateNode(NegateNode* expression);
            virtual antlrcpp::Any visitNotNode(NotNode* expression);
            virtual antlrcpp::Any visitIncrementNode(IncrementNode* expression);
            virtual antlrcpp::Any visitDecrementNode(DecrementNode* expression);
            virtual antlrcpp::Any visitIntegerNode(IntegerNode* value);
            virtual antlrcpp::Any visitFloatNode(FloatNode* value);
            virtual antlrcpp::Any visitStringNode(StringNode* value);
            virtual antlrcpp::Any visitTaggedStringNode(TaggedStringNode* value);
            virtual antlrcpp::Any visitLocalVariableNode(LocalVariableNode* value);
            virtual antlrcpp::Any visitGlobalVariableNode(GlobalVariableNode* value);
            virtual antlrcpp::Any visitArrayNode(ArrayNode* array);
            virtual antlrcpp::Any visitWhileNode(WhileNode* node);
            virtual antlrcpp::Any visitForNode(ForNode* node);
            virtual antlrcpp::Any visitReturnNode(ReturnNode* node);
            virtual antlrcpp::Any visitBreakNode(BreakNode* node);
            virtual antlrcpp::Any visitTernaryNode(TernaryNode* node);
            virtual antlrcpp::Any visitSwitchCaseNode(SwitchCaseNode* node);
            virtual antlrcpp::Any visitSwitchNode(SwitchNode* node);
            virtual antlrcpp::Any visitElseIfNode(ElseIfNode* node);
            virtual antlrcpp::Any visitIfNode(IfNode* node);
    };
}
