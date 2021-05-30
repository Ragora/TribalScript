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

#include <assert.h>

#include <TorqueLexer.h>
#include <TorqueParser.h>

#include <torquescript/compilervisitor.hpp>
#include <torquescript/instructionsequence.hpp>
#include <torquescript/parsererrorlistener.hpp>

namespace TorqueScript
{
    CompilerVisitor::CompilerVisitor(StringTable* stringTable) : mStringTable(stringTable), TorqueBaseVisitor()
    {

    }

    antlrcpp::Any CompilerVisitor::defaultResult()
    {
        return std::vector<ASTNode*>();
    }

    antlrcpp::Any CompilerVisitor::visitChildren(antlr4::tree::ParseTree *node)
    {
        std::vector<ASTNode*> result;

        size_t n = node->children.size();
        for (size_t i = 0; i < n; i++)
        {
            std::vector<ASTNode*> childResult = node->children[i]->accept(this).as<std::vector<ASTNode*>>();
            result.insert(result.end(), childResult.begin(), childResult.end());
        }
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitPackage_declaration(TorqueParser::Package_declarationContext* context)
    {
        std::vector<ASTNode*> result;

        std::vector<ASTNode*> children = this->visitChildren(context).as<std::vector<ASTNode*>>();

        std::vector<FunctionDeclarationNode*> functions;
        for (ASTNode* child : children)
        {
            FunctionDeclarationNode* function = dynamic_cast<FunctionDeclarationNode*>(child);
            assert(function);

            functions.push_back(function);
        }

        PackageDeclarationNode* package = new PackageDeclarationNode(context->LABEL()->getText(), functions);

        result.push_back(package);
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitFunction_declaration(TorqueParser::Function_declarationContext* context)
    {
        std::vector<ASTNode*> result;

        // Load declaration parameter names
        // NOTE: For now we force locals but it appears globals are technically valid but buggy?
        std::vector<std::string> parameterNames;
        if (context->function_declaration_parameters())
        {
            // Function_declaration_parametersContext
            std::vector<TorqueParser::LocalvariableContext*> parameters = context->function_declaration_parameters()->localvariable();

            for (TorqueParser::LocalvariableContext* variable : parameters)
            {
                std::string currentVariableName = "";
                std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = variable->labelwithkeywords();

                // NOTE: For now we just combine :: into a single variable name
                for (TorqueParser::LabelwithkeywordsContext* variableNameComponent : variableNameComponents)
                {
                    if (currentVariableName == "")
                    {
                        currentVariableName += variableNameComponent->getText();
                    }
                    else
                    {
                        currentVariableName += "::" + variableNameComponent->getText();
                    }
                }

                parameterNames.push_back(currentVariableName);
            }
        }

        // Determine function name
        std::string functionName = "";
        std::string functionNameSpace = "";

        std::vector<antlr4::tree::TerminalNode*> functionNameComponents = context->LABEL();
        assert(functionNameComponents.size() <= 2);

        if (functionNameComponents.size() == 2)
        {
            functionName = functionNameComponents[1]->getText();
            functionNameSpace = functionNameComponents[0]->getText();
        }
        else if (functionNameComponents.size() == 1)
        {
            functionName = functionNameComponents[0]->getText();
        }
        else
        {
            // Don't do anything if no function name is provided
            return result;
        }

        // Load body expressions
        std::vector<ASTNode*> bodyStatements = this->visitChildren(context).as<std::vector<ASTNode*>>();
        FunctionDeclarationNode* function = new FunctionDeclarationNode(functionNameSpace, functionName, bodyStatements);

        result.push_back(function);
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitCall(TorqueParser::CallContext* context)
    {
        std::vector<ASTNode*> result;

        // Load function name
        std::string calledFunctionName = "";
        std::string calledFunctionNameSpace = "";

        std::vector<antlr4::tree::TerminalNode*> calledFunctionNameComponents = context->LABEL();
        assert(calledFunctionNameComponents.size() <= 2);

        if (calledFunctionNameComponents.size() == 2)
        {
            calledFunctionName = calledFunctionNameComponents[1]->getText();
            calledFunctionNameSpace = calledFunctionNameComponents[0]->getText();
        }
        else
        {
            calledFunctionName = calledFunctionNameComponents[0]->getText();
        }

        std::vector<ASTNode*> parameters = this->visitChildren(context).as<std::vector<ASTNode*>>();
        FunctionCallNode* call = new FunctionCallNode(calledFunctionNameSpace, calledFunctionName, parameters);

        result.push_back(call);
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitValue(TorqueParser::ValueContext* context)
    {
        std::vector<ASTNode*> result;

        if (context->INT())
        {
            result.push_back(new IntegerNode(std::stoi(context->INT()->getText())));
        }
        else if (context->FLOAT())
        {
            result.push_back(new FloatNode(std::stof(context->FLOAT()->getText())));
        }
        else if (context->STRING())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
            const std::string rawString = context->STRING()->getText();
            const std::string stringContent = rawString.substr(1, rawString.size() - 2);

            result.push_back(new StringNode(stringContent));
        }
        else if (context->TAGGEDSTRING())
        {
            // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
            const std::string rawString = context->TAGGEDSTRING()->getText();
            const std::string stringContent = rawString.substr(1, rawString.size() - 2);

            result.push_back(new TaggedStringNode(stringContent));
        }
        else if (context->LABEL())
        {
            result.push_back(new StringNode(context->LABEL()->getText()));
        }
        else if (context->TRUE())
        {
            result.push_back(new IntegerNode(1));
        }
        else if (context->FALSE())
        {
            result.push_back(new IntegerNode(0));
        }
        else if (context->HEXINT())
        {
            result.push_back(new IntegerNode(std::stoul(context->HEXINT()->getText(), nullptr, 16)));
        }
        else
        {
            throw std::runtime_error("Unhandled expression value type!");
        }

        return result;
    }

    antlrcpp::Any CompilerVisitor::visitArithmetic(TorqueParser::ArithmeticContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 2);

        ASTNode* right = result.back();
        result.pop_back();
        ASTNode* left = result.back();
        result.pop_back();

        if (context->PLUS())
        {
            result.push_back(new AddNode(left, right));
        }
        else if (context->MULTIPLY())
        {
            result.push_back(new MultiplyNode(left, right));
        }
        else if (context->DIVIDE())
        {
            result.push_back(new DivideNode(left, right));
        }
        else
        {
            throw std::runtime_error("Unhandled arithmetic type!");
        }

        return result;
    }

    antlrcpp::Any CompilerVisitor::visitUnary(TorqueParser::UnaryContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 1);

        ASTNode* inner = result.back();
        result.pop_back();

        if (context->MINUS())
        {
            result.push_back(new NegateNode(inner));
        }
        else
        {
            throw std::runtime_error("Unknown Unary Type!");
        }

        return result;
    }

    antlrcpp::Any CompilerVisitor::visitWhile_control(TorqueParser::While_controlContext* context)
    {
        std::vector<ASTNode*> result;
        std::vector<ASTNode*> whileContent = this->visitChildren(context).as<std::vector<ASTNode*>>();

        ASTNode* whileExpression = whileContent.front();
        whileContent.erase(whileContent.begin());

        result.push_back(new WhileNode(whileExpression, whileContent));
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitFor_control(TorqueParser::For_controlContext* context)
    {
        std::vector<ASTNode*> forContent = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(forContent.size() >= 3);

        std::vector<ASTNode*> result;

        // First three nodes should be initializer, expression & advance
        ASTNode* initializer = *forContent.begin();
        ASTNode* expression = *(forContent.begin() + 1);
        ASTNode* advance = *(forContent.begin() + 2);
        forContent.erase(forContent.begin(), forContent.begin() + 2);

        // Remaining content is for body
        result.push_back(new ForNode(initializer, expression, advance, forContent));
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitLocalvariable(TorqueParser::LocalvariableContext* context)
    {
        std::vector<std::string> variableName;
        std::vector<ASTNode*> result;
        std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = context->labelwithkeywords();

        for (TorqueParser::LabelwithkeywordsContext* component : variableNameComponents)
        {
            variableName.push_back(component->getText());
        }

        result.push_back(new LocalVariableNode(variableName));
        return result;
    }

    antlrcpp::Any CompilerVisitor::visitGlobalvariable(TorqueParser::GlobalvariableContext* context)
    {
        std::vector<std::string> variableName;
        std::vector<ASTNode*> result;
        std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = context->labelwithkeywords();

        for (TorqueParser::LabelwithkeywordsContext* component : variableNameComponents)
        {
            variableName.push_back(component->getText());
        }

        result.push_back(new GlobalVariableNode(variableName));
        return result;
    }

    InstructionSequence CompilerVisitor::collapseInstructions(GeneratedInstructions instructions)
    {
        InstructionSequence result;

        for (InstructionSequence sequence : instructions)
        {
            result.insert(result.end(), sequence.begin(), sequence.end());
        }
        return result;
    }
}
