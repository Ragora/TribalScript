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
#include <Tribes2BaseVisitor.h>

#include <tribalscript/codeblock.hpp>
#include <tribalscript/stringtable.hpp>
#include <tribalscript/instructions.hpp>
#include <tribalscript/ast.hpp>
#include <tribalscript/instructionsequence.hpp>

namespace TribalScript
{
    typedef std::vector<InstructionSequence> GeneratedInstructions;

    namespace AST
    {
        class ASTBuilder : public Tribes2BaseVisitor
        {
            public:
                ASTBuilder(StringTable* stringTable);

                virtual antlrcpp::Any defaultResult() override;

                virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree *node) override;

                virtual antlrcpp::Any visitProgram(Tribes2Parser::ProgramContext* context) override;
                virtual antlrcpp::Any visitPackage_declaration(Tribes2Parser::Package_declarationContext* context) override;
                virtual antlrcpp::Any visitFunction_declaration(Tribes2Parser::Function_declarationContext* context) override;
                virtual antlrcpp::Any visitValue(Tribes2Parser::ValueContext* context) override;
                virtual antlrcpp::Any visitArithmetic(Tribes2Parser::ArithmeticContext* context) override;
                virtual antlrcpp::Any visitLogical(Tribes2Parser::LogicalContext* context) override;
                virtual antlrcpp::Any visitAssign(Tribes2Parser::AssignContext* context) override;
                virtual antlrcpp::Any visitAssignable_chain(Tribes2Parser::Assignable_chainContext* context) override;
                virtual antlrcpp::Any visitUnary(Tribes2Parser::UnaryContext* context) override;
                virtual antlrcpp::Any visitChain(Tribes2Parser::ChainContext* context) override;
                virtual antlrcpp::Any visitWhile_control(Tribes2Parser::While_controlContext* context) override;
                virtual antlrcpp::Any visitIf_control(Tribes2Parser::If_controlContext* context) override;
                virtual antlrcpp::Any visitFor_control(Tribes2Parser::For_controlContext* context) override;
                virtual antlrcpp::Any visitLocalvariable(Tribes2Parser::LocalvariableContext* context) override;
                virtual antlrcpp::Any visitGlobalvariable(Tribes2Parser::GlobalvariableContext* context) override;
                virtual antlrcpp::Any visitIncrement(Tribes2Parser::IncrementContext* context) override;
                virtual antlrcpp::Any visitReturn_control(Tribes2Parser::Return_controlContext* context) override;
                virtual antlrcpp::Any visitEquality(Tribes2Parser::EqualityContext* context) override;
                virtual antlrcpp::Any visitConcat(Tribes2Parser::ConcatContext* context) override;
                virtual antlrcpp::Any visitTernary(Tribes2Parser::TernaryContext* context) override;
                virtual antlrcpp::Any visitRelational(Tribes2Parser::RelationalContext* context) override;
                virtual antlrcpp::Any visitSwitch_control(Tribes2Parser::Switch_controlContext* context) override;
                virtual antlrcpp::Any visitLocalarray(Tribes2Parser::LocalarrayContext* context) override;
                virtual antlrcpp::Any visitGlobalarray(Tribes2Parser::GlobalarrayContext* context) override;
                virtual antlrcpp::Any visitPrimary_chain(Tribes2Parser::Primary_chainContext* context) override;

                virtual antlrcpp::Any visitQualified_functioncall_expression(Tribes2Parser::Qualified_functioncall_expressionContext* context) override;
                virtual antlrcpp::Any visitFunctioncall_expression(Tribes2Parser::Functioncall_expressionContext* context) override;


                virtual antlrcpp::Any visitSubfunctioncall_expression(Tribes2Parser::Subfunctioncall_expressionContext* context) override;
                virtual antlrcpp::Any visitField(Tribes2Parser::FieldContext* context) override;
                virtual antlrcpp::Any visitFieldarray(Tribes2Parser::FieldarrayContext* context) override;

                virtual antlrcpp::Any visitField_assign(Tribes2Parser::Field_assignContext* context) override;
                virtual antlrcpp::Any visitDatablock_declaration(Tribes2Parser::Datablock_declarationContext* context) override;
                virtual antlrcpp::Any visitObject_declaration(Tribes2Parser::Object_declarationContext* context) override;

            private:
                StringTable* mStringTable;
        };
    }
}
