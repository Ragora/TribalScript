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

#include <torquescript/astvisitor.hpp>
#include <torquescript/codeblock.hpp>
#include <torquescript/stringtable.hpp>
#include <torquescript/instructions.hpp>
#include <torquescript/instructionsequence.hpp>

namespace TorqueScript
{
    /**
     *  @brief Primary compiler class. Here we use a listener to walk the tree generated by ANTLR in order
     *  to generate executable bytecode for the virtual machine.
     */
    class Compiler : public ASTVisitor
    {
        public:
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

        private:
            std::string mCurrentPackage;
            StringTable* mStringTable;

            /*
                Compiler Routines ==============================
            */

            virtual antlrcpp::Any visitPackageDeclarationNode(PackageDeclarationNode* package) override;
            virtual antlrcpp::Any visitFunctionDeclarationNode(FunctionDeclarationNode* function) override;
            virtual antlrcpp::Any visitFunctionCallNode(FunctionCallNode* call) override;
            virtual antlrcpp::Any visitSubFunctionCallNode(SubFunctionCallNode* call) override;
            virtual antlrcpp::Any visitAddNode(AddNode* expression) override;
            virtual antlrcpp::Any visitIntegerNode(IntegerNode* value) override;
            virtual antlrcpp::Any visitFloatNode(FloatNode* value) override;
            virtual antlrcpp::Any visitStringNode(StringNode* value) override;
            virtual antlrcpp::Any visitTaggedStringNode(TaggedStringNode* value) override;
            virtual antlrcpp::Any visitLocalVariableNode(LocalVariableNode* value) override;
            virtual antlrcpp::Any visitGlobalVariableNode(GlobalVariableNode* value) override;
            virtual antlrcpp::Any visitAssignmentNode(AssignmentNode* expression) override;
            virtual antlrcpp::Any visitLessThanNode(LessThanNode* expression) override;
            virtual antlrcpp::Any visitNegateNode(NegateNode* expression) override;
            virtual antlrcpp::Any visitIncrementNode(IncrementNode* expression) override;
            virtual antlrcpp::Any visitWhileNode(WhileNode* node) override;
            virtual antlrcpp::Any visitForNode(ForNode* node) override;
            virtual antlrcpp::Any visitBreakNode(BreakNode* node) override;
            virtual antlrcpp::Any visitReturnNode(ReturnNode* node) override;
            virtual antlrcpp::Any visitTernaryNode(TernaryNode* node) override;
            virtual antlrcpp::Any visitSwitchNode(SwitchNode* node) override;
            virtual antlrcpp::Any visitIfNode(IfNode* node) override;
            virtual antlrcpp::Any visitArrayNode(ArrayNode* array) override;
            virtual antlrcpp::Any visitEqualsNode(EqualsNode* expression) override;
            virtual antlrcpp::Any visitConcatNode(ConcatNode* expression) override;
            virtual antlrcpp::Any visitMultiplyNode(MultiplyNode* expression) override;
            virtual antlrcpp::Any visitDatablockDeclarationNode(DatablockDeclarationNode* datablock) override;
            virtual antlrcpp::Any visitObjectDeclarationNode(ObjectDeclarationNode* object);
    };
}
