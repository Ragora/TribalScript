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

#include <tribalscript/astvisitor.hpp>
#include <tribalscript/codeblock.hpp>
#include <tribalscript/stringtable.hpp>
#include <tribalscript/instructions.hpp>
#include <tribalscript/interpreterconfiguration.hpp>
#include <tribalscript/instructionsequence.hpp>

namespace TribalScript
{
    /**
     *  @brief Primary compiler class. This class is an AST visitor that walks the AST tree
     *  generated by a AST::ASTBuilder instance to generate an InstructionSequence instance.
     */
    class Compiler : public AST::ASTVisitor
    {
        public:
            Compiler(const InterpreterConfiguration& config);

            /**
             *  @brief Ask the compiler to generate a codeblock from the provided stream.
             *  @param input The input to generate from.
             *  @return A CodeBlock representing the compiled output. If an error has occurred, nullptr is returned.
             */
            CodeBlock* compileStream(std::istream& input, StringTable* stringTable);

            /**
             *  @brief Ask the compiler to generate a codeblock from the provided string.
             *  @param input The string to generate from.
             */
            CodeBlock* compileString(const std::string& input, StringTable* stringTable);

            /**
             *  @brief Ask the compiler to generate a codeblock from the provided file.
             *  @param path The path to load from.
             */
            CodeBlock* compileFile(const std::string& path, StringTable* stringTable);

            virtual antlrcpp::Any defaultResult() override;
            virtual antlrcpp::Any aggregateResult(antlrcpp::Any& aggregate, antlrcpp::Any& nextResult) override;

            const InterpreterConfiguration mConfig;

        private:
            std::string mCurrentPackage;
            StringTable* mStringTable;

            /*
                Compiler Routines ==============================
            */

            virtual antlrcpp::Any visitPackageDeclarationNode(AST::PackageDeclarationNode* package) override;
            virtual antlrcpp::Any visitFunctionDeclarationNode(AST::FunctionDeclarationNode* function) override;
            virtual antlrcpp::Any visitFunctionCallNode(AST::FunctionCallNode* call) override;
            virtual antlrcpp::Any visitSubFunctionCallNode(AST::SubFunctionCallNode* call) override;
            virtual antlrcpp::Any visitSubFieldNode(AST::SubFieldNode* subfield) override;
            virtual antlrcpp::Any visitAddNode(AST::AddNode* expression) override;
            virtual antlrcpp::Any visitMinusNode(AST::MinusNode* expression) override;
            virtual antlrcpp::Any visitModulusNode(AST::ModulusNode* expression) override;
            virtual antlrcpp::Any visitLogicalOrNode(AST::LogicalOrNode* expression) override;
            virtual antlrcpp::Any visitLogicalAndNode(AST::LogicalAndNode* expression) override;
            virtual antlrcpp::Any visitIntegerNode(AST::IntegerNode* value) override;
            virtual antlrcpp::Any visitFloatNode(AST::FloatNode* value) override;
            virtual antlrcpp::Any visitStringNode(AST::StringNode* value) override;
            virtual antlrcpp::Any visitTaggedStringNode(AST::TaggedStringNode* value) override;
            virtual antlrcpp::Any visitLocalVariableNode(AST::LocalVariableNode* value) override;
            virtual antlrcpp::Any visitGlobalVariableNode(AST::GlobalVariableNode* value) override;
            virtual antlrcpp::Any visitAssignmentNode(AST::AssignmentNode* expression) override;
            virtual antlrcpp::Any visitGreaterThanNode(AST::GreaterThanNode* expression);
            virtual antlrcpp::Any visitLessThanNode(AST::LessThanNode* expression) override;
            virtual antlrcpp::Any visitNegateNode(AST::NegateNode* expression) override;
            virtual antlrcpp::Any visitNotNode(AST::NotNode* expression) override;
            virtual antlrcpp::Any visitIncrementNode(AST::IncrementNode* expression) override;
            virtual antlrcpp::Any visitWhileNode(AST::WhileNode* node) override;
            virtual antlrcpp::Any visitForNode(AST::ForNode* node) override;
            virtual antlrcpp::Any visitBreakNode(AST::BreakNode* node) override;
            virtual antlrcpp::Any visitReturnNode(AST::ReturnNode* node) override;
            virtual antlrcpp::Any visitTernaryNode(AST::TernaryNode* node) override;
            virtual antlrcpp::Any visitSwitchNode(AST::SwitchNode* node) override;
            virtual antlrcpp::Any visitIfNode(AST::IfNode* node) override;
            virtual antlrcpp::Any visitArrayNode(AST::ArrayNode* array) override;
            virtual antlrcpp::Any visitEqualsNode(AST::EqualsNode* expression) override;
            virtual antlrcpp::Any visitNotEqualsNode(AST::NotEqualsNode* expression) override;
            virtual antlrcpp::Any visitStringEqualsNode(AST::StringEqualsNode* expression) override;
            virtual antlrcpp::Any visitConcatNode(AST::ConcatNode* expression) override;
            virtual antlrcpp::Any visitMultiplyNode(AST::MultiplyNode* expression) override;
            virtual antlrcpp::Any visitDivideNode(AST::DivideNode* expression) override;
            virtual antlrcpp::Any visitDatablockDeclarationNode(AST::DatablockDeclarationNode* datablock) override;
            virtual antlrcpp::Any visitObjectDeclarationNode(AST::ObjectDeclarationNode* object) override;
            virtual antlrcpp::Any visitFieldAssignNode(AST::FieldAssignNode* node) override;
            virtual antlrcpp::Any visitStringNotEqualNode(AST::StringNotEqualNode* expression) override;
    };
}