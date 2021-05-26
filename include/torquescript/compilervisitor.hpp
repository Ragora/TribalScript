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

#include <memory>

#include "antlr4-runtime.h"
#include <TorqueBaseVisitor.h>

#include <torquescript/codeblock.hpp>
#include <torquescript/instructions.hpp>
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
{
    typedef std::vector<InstructionSequence> GeneratedInstructions;

    class CompilerVisitor : public TorqueBaseVisitor
    {
        public:
            virtual antlrcpp::Any defaultResult() override;

            virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree *node) override;
            virtual antlrcpp::Any visitPackage_declaration(TorqueParser::Package_declarationContext* context) override;

            virtual antlrcpp::Any visitIncrement(TorqueParser::IncrementContext* context) override;

            virtual antlrcpp::Any visitWhile_control(TorqueParser::While_controlContext* context) override;

            virtual antlrcpp::Any visitSwitch_control(TorqueParser::Switch_controlContext* context) override;

            virtual antlrcpp::Any visitFunction_declaration(TorqueParser::Function_declarationContext* context) override;

            virtual antlrcpp::Any visitEquality(TorqueParser::EqualityContext* context) override;

            virtual antlrcpp::Any visitArithmetic(TorqueParser::ArithmeticContext* context) override;

            virtual antlrcpp::Any visitFor_control(TorqueParser::For_controlContext* context) override;

            virtual antlrcpp::Any visitIf_control(TorqueParser::If_controlContext* context) override;

            virtual antlrcpp::Any visitAssign(TorqueParser::AssignContext* context) override;

            virtual antlrcpp::Any visitRelational(TorqueParser::RelationalContext* context) override;

            virtual antlrcpp::Any visitTernary(TorqueParser::TernaryContext* context) override;

            virtual antlrcpp::Any visitValue(TorqueParser::ValueContext* context) override;

            virtual antlrcpp::Any visitGlobalvariable(TorqueParser::GlobalvariableContext* context) override;

            virtual antlrcpp::Any visitLocalvariable(TorqueParser::LocalvariableContext* context) override;

            InstructionSequence collapseInstructions(GeneratedInstructions instructions);

            virtual antlrcpp::Any visitUnary(TorqueParser::UnaryContext* context) override;
            virtual antlrcpp::Any visitConcat(TorqueParser::ConcatContext* context) override;
            virtual antlrcpp::Any visitSubfield(TorqueParser::SubfieldContext* context) override;

            virtual antlrcpp::Any visitCall(TorqueParser::CallContext* context) override;

            virtual antlrcpp::Any visitSubcall(TorqueParser::SubcallContext* context) override;

            virtual antlrcpp::Any visitReturn_control(TorqueParser::Return_controlContext* context) override;

            virtual antlrcpp::Any visitExpression_statement(TorqueParser::Expression_statementContext* context) override;
            virtual antlrcpp::Any visitArray(TorqueParser::ArrayContext* context) override;

            virtual antlrcpp::Any visitDatablock_declaration(TorqueParser::Datablock_declarationContext* context) override;

            virtual antlrcpp::Any visitObject_declaration(TorqueParser::Object_declarationContext* context) override;

        private:
            std::string mCurrentPackage;
    };
}
