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

#include <torquescript/astbuilder.hpp>
#include <torquescript/instructionsequence.hpp>
#include <torquescript/parsererrorlistener.hpp>

namespace TorqueScript
{
    ASTBuilder::ASTBuilder(StringTable* stringTable) : mStringTable(stringTable), TorqueBaseVisitor()
    {

    }

    antlrcpp::Any ASTBuilder::defaultResult()
    {
        return std::vector<ASTNode*>();
    }

    antlrcpp::Any ASTBuilder::visitChildren(antlr4::tree::ParseTree *node)
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

    antlrcpp::Any ASTBuilder::visitProgram(TorqueParser::ProgramContext* context)
    {
        std::vector<ASTNode*> children = this->visitChildren(context).as<std::vector<ASTNode*>>();
        return new ProgramNode(children);
    }

    antlrcpp::Any ASTBuilder::visitPackage_declaration(TorqueParser::Package_declarationContext* context)
    {
        std::vector<ASTNode*> result;

        std::vector<ASTNode*> children = this->visitChildren(context).as<std::vector<ASTNode*>>();
        PackageDeclarationNode* package = new PackageDeclarationNode(context->LABEL()->getText(), children);

        result.push_back(package);
        return result;
    }

    antlrcpp::Any ASTBuilder::visitFunction_declaration(TorqueParser::Function_declarationContext* context)
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
        FunctionDeclarationNode* function = new FunctionDeclarationNode(functionNameSpace, functionName, parameterNames, bodyStatements);

        result.push_back(function);
        return result;
    }

    antlrcpp::Any ASTBuilder::visitCall(TorqueParser::CallContext* context)
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

    antlrcpp::Any ASTBuilder::visitSubfield(TorqueParser::SubfieldContext* context)
    {
        std::vector<ASTNode*> parent = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(parent.size() == 1);

        std::vector<ASTNode*> result;
        result.push_back(new SubFieldNode(parent[0], context->LABEL()->getText()));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitArray(TorqueParser::ArrayContext* context)
    {
        std::vector<ASTNode*> parameters = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(parameters.size() >= 2);

        std::vector<ASTNode*> result;
        ASTNode* target = parameters[0];
        parameters.erase(parameters.begin());

        result.push_back(new ArrayNode(target, parameters));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitSubcall(TorqueParser::SubcallContext* context)
    {
        const std::string calledFunctionName = context->LABEL()->getText();

        std::vector<ASTNode*> parameters = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(parameters.size() >= 1);

        ASTNode* target = parameters[0];
        parameters.erase(parameters.begin());

        std::vector<ASTNode*> result;
        result.push_back(new SubFunctionCallNode(target, calledFunctionName, parameters));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitValue(TorqueParser::ValueContext* context)
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

    antlrcpp::Any ASTBuilder::visitArithmetic(TorqueParser::ArithmeticContext* context)
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

    antlrcpp::Any ASTBuilder::visitAssign(TorqueParser::AssignContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 2);

        ASTNode* right = result.back();
        result.pop_back();
        ASTNode* left = result.back();
        result.pop_back();

        if (context->ASSIGN())
        {
            result.push_back(new AssignmentNode(left, right));
        }
        else
        {
            throw std::runtime_error("Unhandled assignment type!");
        }

        return result;
    }

    antlrcpp::Any ASTBuilder::visitRelational(TorqueParser::RelationalContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 2);

        ASTNode* right = result.back();
        result.pop_back();
        ASTNode* left = result.back();
        result.pop_back();

        if (context->LESSTHAN())
        {
            result.push_back(new LessThanNode(left, right));
        }
        else
        {
            throw std::runtime_error("Unhandled Relational Type!");
        }

        return result;
    }

    antlrcpp::Any ASTBuilder::visitEquality(TorqueParser::EqualityContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 2);

        ASTNode* right = result.back();
        result.pop_back();
        ASTNode* left = result.back();
        result.pop_back();

        if (context->EQUALS())
        {
            result.push_back(new EqualsNode(left, right));
        }
        else
        {
            throw std::runtime_error("Unknown equality type!");
        }

        return result;
    }

    antlrcpp::Any ASTBuilder::visitConcat(TorqueParser::ConcatContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 2);

        ASTNode* right = result.back();
        result.pop_back();
        ASTNode* left = result.back();
        result.pop_back();

        if (context->CONCAT())
        {
            result.push_back(new ConcatNode(left, right));
        }
        else
        {
            throw std::runtime_error("Unhandled concat type!");
        }

        return result;
    }

    antlrcpp::Any ASTBuilder::visitUnary(TorqueParser::UnaryContext* context)
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

    antlrcpp::Any ASTBuilder::visitIncrement(TorqueParser::IncrementContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() == 1);

        ASTNode* incremented = result.back();
        result.pop_back();

        result.push_back(new IncrementNode(incremented));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitWhile_control(TorqueParser::While_controlContext* context)
    {
        std::vector<ASTNode*> result;
        std::vector<ASTNode*> whileContent = this->visitChildren(context).as<std::vector<ASTNode*>>();

        ASTNode* whileExpression = whileContent.front();
        whileContent.erase(whileContent.begin());

        result.push_back(new WhileNode(whileExpression, whileContent));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitIf_control(TorqueParser::If_controlContext* context)
    {
        std::vector<ASTNode*> ifContent = this->visitChildren(context).as<std::vector<ASTNode*>>();

        // Should be at least one entry
        assert(ifContent.size() >= 1);

        // Here we have to be a bit more careful about deconstructing our payload
        std::vector<ASTNode*> elseBody;
        if (context->else_control())
        {
            const unsigned int elseStatementCount = context->else_control()->control_statements()->expression_statement().size();

            if (elseStatementCount)
            {
                elseBody.insert(elseBody.end(), ifContent.end() - elseStatementCount, ifContent.end());
                ifContent.erase(ifContent.end() - elseStatementCount, ifContent.end());
            }

            assert(elseBody.size() == elseStatementCount);
        }

        // Handle all else ifs
        std::vector<ElseIfNode*> elseIfs;
        std::vector<TorqueParser::Elseif_controlContext*> elseIfControl = context->elseif_control();
        for (TorqueParser::Elseif_controlContext* elseIf : elseIfControl)
        {
            TorqueParser::Control_statementsContext* elseIfControlStatements = elseIf->control_statements();

            std::vector<ASTNode*> elseIfBody;
            if (elseIfControlStatements)
            {
                const unsigned int elseIfStatementCount = elseIfControlStatements->expression_statement().size();

                if (elseIfStatementCount)
                {
                    elseIfBody.insert(elseIfBody.end(), ifContent.end() - elseIfStatementCount, ifContent.end());
                    ifContent.erase(ifContent.end() - elseIfStatementCount, ifContent.end());
                }

                assert(elseIfBody.size() == elseIfStatementCount);
            }

            // Load the expression
            ASTNode* elseIfExpression = ifContent.back();
            ifContent.pop_back();

            elseIfs.push_back(new ElseIfNode(elseIfExpression, elseIfBody));
        }

        // Handle primary if
        std::vector<ASTNode*> ifBody;
        if (context->control_statements())
        {
            const unsigned int ifStatementCount = context->control_statements()->expression_statement().size();

            if (ifStatementCount)
            {
                ifBody.insert(ifBody.end(), ifContent.end() - ifStatementCount, ifContent.end());
                ifContent.erase(ifContent.end() - ifStatementCount, ifContent.end());
            }

            assert(ifBody.size() == ifStatementCount);
        }
        ASTNode* ifExpression = ifContent.back();
        ifContent.pop_back();

        assert(ifContent.size() == 0);

        // Output final result
        std::vector<ASTNode*> result;
        result.push_back(new IfNode(ifExpression, ifBody, elseIfs, elseBody));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitSwitch_control(TorqueParser::Switch_controlContext* context)
    {
        std::vector<ASTNode*> switchContent = this->visitChildren(context).as<std::vector<ASTNode*>>();

        // Should be at least one entry
        assert(switchContent.size() >= 1);

        // Load default body if present
        std::vector<ASTNode*> defaultBody;
        if (context->default_control())
        {
            const unsigned int defaultStatementCount = context->default_control()->expression_statement().size();

            if (defaultStatementCount)
            {
                defaultBody.insert(defaultBody.end(), switchContent.end() - defaultStatementCount, switchContent.end());
                switchContent.erase(switchContent.end() - defaultStatementCount, switchContent.end());
            }

            assert(defaultBody.size() == defaultStatementCount);
        }

        // Load all switch statements
        std::vector<SwitchCaseNode*> switchCases;
        std::vector<TorqueParser::Case_controlContext*> switchCaseControl = context->case_control();
        for (auto iterator = switchCaseControl.rbegin(); iterator != switchCaseControl.rend(); ++iterator)
        {
            TorqueParser::Case_controlContext* caseContext = *iterator;

            const unsigned int caseStatementCount = caseContext->expression_statement().size();
            const unsigned int caseExpressionCount = caseContext->expression().size();

            std::vector<ASTNode*> caseBody;
            if (caseStatementCount)
            {
                caseBody.insert(caseBody.end(), switchContent.end() - caseStatementCount, switchContent.end());
                switchContent.erase(switchContent.end() - caseStatementCount, switchContent.end());

                assert(caseBody.size() == caseStatementCount);
            }

            std::vector<ASTNode*> caseExpressions;
            caseExpressions.insert(caseExpressions.end(), switchContent.end() - caseExpressionCount, switchContent.end());
            switchContent.erase(switchContent.end() - caseExpressionCount, switchContent.end());

            assert(caseExpressions.size() == caseExpressionCount);


            switchCases.push_back(new SwitchCaseNode(caseExpressions, caseBody));
        }

        // Load switch expression
        ASTNode* switchExpression = switchContent.back();
        switchContent.pop_back();

        assert(switchContent.size() == 0);

        // Output final result
        std::vector<ASTNode*> result;
        result.push_back(new SwitchNode(switchExpression, switchCases, defaultBody));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitFor_control(TorqueParser::For_controlContext* context)
    {
        std::vector<ASTNode*> forContent = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(forContent.size() >= 3);

        std::vector<ASTNode*> result;

        // First three nodes should be initializer, expression & advance
        ASTNode* initializer = forContent[0];
        ASTNode* expression =  forContent[1];
        ASTNode* advance = forContent[2];
        forContent.erase(forContent.begin(), forContent.begin() + 3);

        // Remaining content is for body
        result.push_back(new ForNode(initializer, expression, advance, forContent));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitTernary(TorqueParser::TernaryContext* context)
    {
        std::vector<ASTNode*> ternaryContent = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(ternaryContent.size() == 3);

        std::vector<ASTNode*> result;
        result.push_back(new TernaryNode(ternaryContent[0], ternaryContent[1], ternaryContent[2]));
        return result;
    }

    antlrcpp::Any ASTBuilder::visitLocalvariable(TorqueParser::LocalvariableContext* context)
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

    antlrcpp::Any ASTBuilder::visitGlobalvariable(TorqueParser::GlobalvariableContext* context)
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

    antlrcpp::Any ASTBuilder::visitReturn_control(TorqueParser::Return_controlContext* context)
    {
        std::vector<ASTNode*> result = this->visitChildren(context).as<std::vector<ASTNode*>>();
        assert(result.size() <= 1);

        ASTNode* expression = nullptr;
        if (result.size() == 1)
        {
            expression = result.back();
            result.pop_back();
        }

        result.push_back(new ReturnNode(expression));
        return result;
    }

    InstructionSequence ASTBuilder::collapseInstructions(GeneratedInstructions instructions)
    {
        InstructionSequence result;

        for (InstructionSequence sequence : instructions)
        {
            result.insert(result.end(), sequence.begin(), sequence.end());
        }
        return result;
    }
}
