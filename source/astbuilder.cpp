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
    namespace AST
    {
        ASTBuilder::ASTBuilder(StringTable* stringTable) : TorqueBaseVisitor(), mStringTable(stringTable)
        {

        }

        antlrcpp::Any ASTBuilder::defaultResult()
        {
            return std::vector<AST::ASTNode*>();
        }

        antlrcpp::Any ASTBuilder::visitChildren(antlr4::tree::ParseTree *node)
        {
            std::vector<AST::ASTNode*> result;

            const size_t childCount = node->children.size();
            for (size_t iteration = 0; iteration < childCount; iteration++)
            {
                std::vector<AST::ASTNode*> childResult = node->children[iteration]->accept(this).as<std::vector<AST::ASTNode*>>();
                result.insert(result.end(), childResult.begin(), childResult.end());
            }
            return result;
        }

        antlrcpp::Any ASTBuilder::visitProgram(TorqueParser::ProgramContext* context)
        {
            std::vector<AST::ASTNode*> children = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            return new AST::ProgramNode(children);
        }

        antlrcpp::Any ASTBuilder::visitPackage_declaration(TorqueParser::Package_declarationContext* context)
        {
            std::vector<AST::ASTNode*> result;

            std::vector<AST::ASTNode*> children = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            AST::PackageDeclarationNode* package = new AST::PackageDeclarationNode(context->LABEL()->getText(), children);

            result.push_back(package);
            return result;
        }

        antlrcpp::Any ASTBuilder::visitLocalarray(TorqueParser::LocalarrayContext* context)
        {
            std::vector<AST::ASTNode*> parameters = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(parameters.size() >= 1);

            std::vector<ASTNode*> result;
            ASTNode* parent = parameters[0];
            parameters.erase(parameters.begin());

            result.push_back(new AST::ArrayNode(parent, parameters));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitGlobalarray(TorqueParser::GlobalarrayContext* context)
        {
            std::vector<AST::ASTNode*> parameters = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(parameters.size() >= 1);

            std::vector<ASTNode*> result;
            ASTNode* parent = parameters[0];
            parameters.erase(parameters.begin());

            result.push_back(new AST::ArrayNode(parent, parameters));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitQualified_functioncall_expression(TorqueParser::Qualified_functioncall_expressionContext* context)
        {
            std::vector<AST::ASTNode*> result;

            // Load function name
            std::string calledFunctionName = "";
            std::string calledFunctionNameSpace = "";

            std::vector<antlr4::tree::TerminalNode*> calledFunctionNameComponents = context->LABEL();
            assert(calledFunctionNameComponents.size() == 2);

            calledFunctionName = calledFunctionNameComponents[1]->getText();
            calledFunctionNameSpace = calledFunctionNameComponents[0]->getText();
 
            std::vector<AST::ASTNode*> parameters = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            AST::FunctionCallNode* call = new AST::FunctionCallNode(calledFunctionNameSpace, calledFunctionName, parameters);

            result.push_back(call);
            return result;
        }

        antlrcpp::Any ASTBuilder::visitFunctioncall_expression(TorqueParser::Functioncall_expressionContext* context)
        {
            std::vector<AST::ASTNode*> result;
            std::vector<AST::ASTNode*> parameters = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();

            const bool isSubcall = dynamic_cast<TorqueParser::Chain_startContext*>(context->parent) ? false : true;

            if (isSubcall)
            {
                result.push_back(new AST::SubFunctionCallNode(context->LABEL()->getText(), parameters));
            }
            else
            {
                AST::FunctionCallNode* call = new AST::FunctionCallNode("", context->LABEL()->getText(), parameters);
                result.push_back(call);
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitField(TorqueParser::FieldContext* context)
        {
            std::vector<AST::ASTNode*> result;
            result.push_back(new AST::SubFieldNode(context->LABEL()->getText(), std::vector<ASTNode*>()));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitFieldarray(TorqueParser::FieldarrayContext* context)
        {
            std::vector<AST::ASTNode*> parameters = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
;
            std::vector<ASTNode*> result;
            result.push_back(new AST::SubFieldNode(context->LABEL()->getText(), parameters));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitFunction_declaration(TorqueParser::Function_declarationContext* context)
        {
            std::vector<AST::ASTNode*> result;

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
            std::vector<AST::ASTNode*> bodyStatements;
            for (TorqueParser::Expression_statementContext* statement : context->expression_statement())
            {
                std::vector<AST::ASTNode*> statementNodes = this->visitChildren(statement).as<std::vector<AST::ASTNode*>>();
                bodyStatements.insert(bodyStatements.end(), statementNodes.begin(), statementNodes.end());
            }
            AST::FunctionDeclarationNode* function = new AST::FunctionDeclarationNode(functionNameSpace, functionName, parameterNames, bodyStatements);

            result.push_back(function);
            return result;
        }

        antlrcpp::Any ASTBuilder::visitValue(TorqueParser::ValueContext* context)
        {
            std::vector<AST::ASTNode*> result;

            if (context->INT())
            {
                result.push_back(new AST::IntegerNode(std::stoi(context->INT()->getText())));
            }
            else if (context->FLOAT())
            {
                result.push_back(new AST::FloatNode(std::stof(context->FLOAT()->getText())));
            }
            else if (context->STRING())
            {
                // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
                const std::string rawString = context->STRING()->getText();
                const std::string stringContent = rawString.substr(1, rawString.size() - 2);

                result.push_back(new AST::StringNode(stringContent));
            }
            else if (context->TAGGEDSTRING())
            {
                // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
                const std::string rawString = context->TAGGEDSTRING()->getText();
                const std::string stringContent = rawString.substr(1, rawString.size() - 2);

                result.push_back(new AST::TaggedStringNode(stringContent));
            }
            else if (context->LABEL())
            {
                result.push_back(new AST::StringNode(context->LABEL()->getText()));
            }
            else if (context->TRUE())
            {
                result.push_back(new AST::IntegerNode(1));
            }
            else if (context->FALSE())
            {
                result.push_back(new AST::IntegerNode(0));
            }
            else if (context->HEXINT())
            {
                result.push_back(new AST::IntegerNode(std::stoul(context->HEXINT()->getText(), nullptr, 16)));
            }
            else
            {
                throw std::runtime_error("Unhandled expression value type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitLogical(TorqueParser::LogicalContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 2);

            AST::ASTNode* right = result.back();
            result.pop_back();
            AST::ASTNode* left = result.back();
            result.pop_back();

            if (context->LOGICALAND())
            {
                result.push_back(new AST::LogicalAndNode(left, right));
            }
            else if (context->LOGICALOR())
            {
                result.push_back(new AST::LogicalOrNode(left, right));
            }
            else
            {
                throw std::runtime_error("Unhandled logical type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitArithmetic(TorqueParser::ArithmeticContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 2);

            AST::ASTNode* right = result.back();
            result.pop_back();
            AST::ASTNode* left = result.back();
            result.pop_back();

            if (context->PLUS())
            {
                result.push_back(new AST::AddNode(left, right));
            }
            else if (context->MINUS())
            {
                result.push_back(new AST::MinusNode(left, right));
            }
            else if (context->MULTIPLY())
            {
                result.push_back(new AST::MultiplyNode(left, right));
            }
            else if (context->DIVIDE())
            {
                result.push_back(new AST::DivideNode(left, right));
            }
            else if (context->MODULUS())
            {
                result.push_back(new AST::ModulusNode(left, right));
            }
            else
            {
                throw std::runtime_error("Unhandled arithmetic type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitAssign(TorqueParser::AssignContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 2);

            AST::ASTNode* right = result.back();
            result.pop_back();
            AST::ASTNode* left = result.back();
            result.pop_back();

            if (context->ASSIGN())
            {
                result.push_back(new AST::AssignmentNode(left, right));
            }
            else
            {
                throw std::runtime_error("Unhandled assignment type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitRelational(TorqueParser::RelationalContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 2);

            AST::ASTNode* right = result.back();
            result.pop_back();
            AST::ASTNode* left = result.back();
            result.pop_back();

            if (context->LESSTHAN())
            {
                result.push_back(new AST::LessThanNode(left, right));
            }
            else if (context->GREATERTHAN())
            {
                result.push_back(new AST::GreaterThanNode(left, right));
            }
            else
            {
                throw std::runtime_error("Unhandled Relational Type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitEquality(TorqueParser::EqualityContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 2);

            AST::ASTNode* right = result.back();
            result.pop_back();
            AST::ASTNode* left = result.back();
            result.pop_back();

            if (context->EQUALS())
            {
                result.push_back(new AST::EqualsNode(left, right));
            }
            else if (context->NOTEQUAL())
            {
                result.push_back(new AST::NotEqualsNode(left, right));
            }
            else if (context->STRINGEQUALS())
            {
                result.push_back(new AST::StringEqualsNode(left, right));
            }
            else if (context->STRINGNOTEQUAL())
            {
                result.push_back(new AST::StringNotEqualNode(left, right));
            }
            else
            {
                throw std::runtime_error("Unknown equality type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitConcat(TorqueParser::ConcatContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 2);

            AST::ASTNode* right = result.back();
            result.pop_back();
            AST::ASTNode* left = result.back();
            result.pop_back();

            if (context->CONCAT())
            {
                result.push_back(new AST::ConcatNode(left, right, ""));
            }
            else if (context->SPACECONCAT())
            {
                result.push_back(new AST::ConcatNode(left, right, " "));
            }
            else if (context->NEWLINECONCAT())
            {
                result.push_back(new AST::ConcatNode(left, right, "\n"));
            }
            else if (context->TABCONCAT())
            {
                result.push_back(new AST::ConcatNode(left, right, "\t"));
            }
            else
            {
                throw std::runtime_error("Unhandled concat type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitUnary(TorqueParser::UnaryContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 1);

            AST::ASTNode* inner = result.back();
            result.pop_back();

            if (context->MINUS())
            {
                result.push_back(new AST::NegateNode(inner));
            }
            else if (context->NOT())
            {
                result.push_back(new AST::NotNode(inner));
            }
            else
            {
                throw std::runtime_error("Unknown Unary Type!");
            }

            return result;
        }

        antlrcpp::Any ASTBuilder::visitIncrement(TorqueParser::IncrementContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() == 1);

            AST::ASTNode* incremented = result.back();
            result.pop_back();

            result.push_back(new AST::IncrementNode(incremented));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitWhile_control(TorqueParser::While_controlContext* context)
        {
            std::vector<AST::ASTNode*> result;
            std::vector<AST::ASTNode*> whileContent = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();

            AST::ASTNode* whileExpression = whileContent.front();
            whileContent.erase(whileContent.begin());

            result.push_back(new AST::WhileNode(whileExpression, whileContent));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitIf_control(TorqueParser::If_controlContext* context)
        {
            std::vector<AST::ASTNode*> ifContent = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();

            // Should be at least one entry
            assert(ifContent.size() >= 1);

            // Here we have to be a bit more careful about deconstructing our payload
            std::vector<AST::ASTNode*> elseBody;
            if (context->else_control())
            {
                const std::size_t elseStatementCount = context->else_control()->control_statements()->expression_statement().size();

                if (elseStatementCount)
                {
                    elseBody.insert(elseBody.end(), ifContent.end() - elseStatementCount, ifContent.end());
                    ifContent.erase(ifContent.end() - elseStatementCount, ifContent.end());
                }

                assert(elseBody.size() == elseStatementCount);
            }

            // Handle all else ifs
            std::vector<AST::ElseIfNode*> elseIfs;
            std::vector<TorqueParser::Elseif_controlContext*> elseIfControl = context->elseif_control();
            for (TorqueParser::Elseif_controlContext* elseIf : elseIfControl)
            {
                TorqueParser::Control_statementsContext* elseIfControlStatements = elseIf->control_statements();

                std::vector<AST::ASTNode*> elseIfBody;
                if (elseIfControlStatements)
                {
                    const std::size_t elseIfStatementCount = elseIfControlStatements->expression_statement().size();

                    if (elseIfStatementCount)
                    {
                        elseIfBody.insert(elseIfBody.end(), ifContent.end() - elseIfStatementCount, ifContent.end());
                        ifContent.erase(ifContent.end() - elseIfStatementCount, ifContent.end());
                    }

                    assert(elseIfBody.size() == elseIfStatementCount);
                }

                // Load the expression
                AST::ASTNode* elseIfExpression = ifContent.back();
                ifContent.pop_back();

                elseIfs.push_back(new AST::ElseIfNode(elseIfExpression, elseIfBody));
            }

            // Handle primary if
            std::vector<AST::ASTNode*> ifBody;
            if (context->control_statements())
            {
                const std::size_t ifStatementCount = context->control_statements()->expression_statement().size();

                if (ifStatementCount)
                {
                    ifBody.insert(ifBody.end(), ifContent.end() - ifStatementCount, ifContent.end());
                    ifContent.erase(ifContent.end() - ifStatementCount, ifContent.end());
                }

                assert(ifBody.size() == ifStatementCount);
            }
            AST::ASTNode* ifExpression = ifContent.back();
            ifContent.pop_back();

            assert(ifContent.size() == 0);

            // Output final result
            std::vector<AST::ASTNode*> result;
            result.push_back(new AST::IfNode(ifExpression, ifBody, elseIfs, elseBody));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitSwitch_control(TorqueParser::Switch_controlContext* context)
        {
            std::vector<AST::ASTNode*> switchContent = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();

            // Should be at least one entry
            assert(switchContent.size() >= 1);

            // Load default body if present
            std::vector<AST::ASTNode*> defaultBody;
            if (context->default_control())
            {
                const std::size_t defaultStatementCount = context->default_control()->expression_statement().size();

                if (defaultStatementCount)
                {
                    defaultBody.insert(defaultBody.end(), switchContent.end() - defaultStatementCount, switchContent.end());
                    switchContent.erase(switchContent.end() - defaultStatementCount, switchContent.end());
                }

                assert(defaultBody.size() == defaultStatementCount);
            }

            // Load all switch statements
            std::vector<AST::SwitchCaseNode*> switchCases;
            std::vector<TorqueParser::Case_controlContext*> switchCaseControl = context->case_control();
            for (auto iterator = switchCaseControl.rbegin(); iterator != switchCaseControl.rend(); ++iterator)
            {
                TorqueParser::Case_controlContext* caseContext = *iterator;

                const std::size_t caseStatementCount = caseContext->expression_statement().size();
                const std::size_t caseExpressionCount = caseContext->expression().size();

                std::vector<AST::ASTNode*> caseBody;
                if (caseStatementCount)
                {
                    caseBody.insert(caseBody.end(), switchContent.end() - caseStatementCount, switchContent.end());
                    switchContent.erase(switchContent.end() - caseStatementCount, switchContent.end());

                    assert(caseBody.size() == caseStatementCount);
                }

                std::vector<AST::ASTNode*> caseExpressions;
                caseExpressions.insert(caseExpressions.end(), switchContent.end() - caseExpressionCount, switchContent.end());
                switchContent.erase(switchContent.end() - caseExpressionCount, switchContent.end());

                assert(caseExpressions.size() == caseExpressionCount);

                switchCases.push_back(new AST::SwitchCaseNode(caseExpressions, caseBody));
            }

            // Load switch expression
            AST::ASTNode* switchExpression = switchContent.back();
            switchContent.pop_back();

            assert(switchContent.size() == 0);

            // Output final result
            std::vector<AST::ASTNode*> result;
            result.push_back(new AST::SwitchNode(switchExpression, switchCases, defaultBody));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitFor_control(TorqueParser::For_controlContext* context)
        {
            std::vector<AST::ASTNode*> forContent = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(forContent.size() >= 3);

            std::vector<AST::ASTNode*> result;

            // First three nodes should be initializer, expression & advance
            AST::ASTNode* initializer = forContent[0];
            AST::ASTNode* expression =  forContent[1];
            AST::ASTNode* advance = forContent[2];
            forContent.erase(forContent.begin(), forContent.begin() + 3);

            // Remaining content is for body
            result.push_back(new AST::ForNode(initializer, expression, advance, forContent));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitTernary(TorqueParser::TernaryContext* context)
        {
            std::vector<AST::ASTNode*> ternaryContent = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(ternaryContent.size() == 3);

            std::vector<AST::ASTNode*> result;
            result.push_back(new AST::TernaryNode(ternaryContent[0], ternaryContent[1], ternaryContent[2]));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitLocalvariable(TorqueParser::LocalvariableContext* context)
        {
            std::vector<std::string> variableName;
            std::vector<AST::ASTNode*> result;
            std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = context->labelwithkeywords();

            for (TorqueParser::LabelwithkeywordsContext* component : variableNameComponents)
            {
                variableName.push_back(component->getText());
            }

            result.push_back(new AST::LocalVariableNode(variableName));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitGlobalvariable(TorqueParser::GlobalvariableContext* context)
        {
            std::vector<std::string> variableName;
            std::vector<AST::ASTNode*> result;
            std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = context->labelwithkeywords();

            for (TorqueParser::LabelwithkeywordsContext* component : variableNameComponents)
            {
                variableName.push_back(component->getText());
            }

            result.push_back(new AST::GlobalVariableNode(variableName));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitReturn_control(TorqueParser::Return_controlContext* context)
        {
            std::vector<AST::ASTNode*> result = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(result.size() <= 1);

            AST::ASTNode* expression = nullptr;
            if (result.size() == 1)
            {
                expression = result.back();
                result.pop_back();
            }

            result.push_back(new AST::ReturnNode(expression));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitField_assign(TorqueParser::Field_assignContext* context)
        {
            std::vector<AST::ASTNode*> fieldExpressions = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();
            assert(fieldExpressions.size() >= 1);

            // Back is rhs of expression
            AST::ASTNode* fieldValue = fieldExpressions.back();
            fieldExpressions.pop_back();

            std::string fieldBaseName = context->labelwithkeywords()->getText();

            std::vector<AST::ASTNode*> result;
            result.push_back(new AST::FieldAssignNode(fieldBaseName, fieldExpressions, fieldValue));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitDatablock_declaration(TorqueParser::Datablock_declarationContext* context)
        {
            std::vector<AST::ASTNode*> result;
            std::vector<AST::ASTNode*> fields = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();

            std::string name = "";
            std::string typeName = "";
            std::string parentName = "";
            std::vector<antlr4::tree::TerminalNode*> labels = context->LABEL();

            // If there's a third element, we have our parent
            if (labels.size() == 3)
            {
                parentName = labels[2]->getText();
            }
            typeName = labels[0]->getText();
            name = labels[1]->getText();

            result.push_back(new AST::DatablockDeclarationNode(name, typeName, parentName, fields));
            return result;
        }

        antlrcpp::Any ASTBuilder::visitObject_declaration(TorqueParser::Object_declarationContext* context)
        {
            std::vector<AST::ASTNode*> result;
            std::vector<AST::ASTNode*> objectContent = this->visitChildren(context).as<std::vector<AST::ASTNode*>>();

            AST::ASTNode* name = nullptr;
            AST::ASTNode* typeName = nullptr;
            if (context->LABEL())
            {
                typeName = new AST::StringNode(context->LABEL()->getText());
            }
            else
            {
                typeName = objectContent.front();
                objectContent.erase(objectContent.begin());
            }

            // Object name is always present at top at this point unless none was provided
            if (context->name)
            {
                name = objectContent.front();
                objectContent.erase(objectContent.begin());
            }

            // Sort out children and fields
            std::vector<AST::ObjectDeclarationNode*> children;
            std::vector<AST::ASTNode*> fields;

            for (AST::ASTNode* component : objectContent)
            {
                AST::ObjectDeclarationNode* child = dynamic_cast<AST::ObjectDeclarationNode*>(component);

                if (child)
                {
                    children.push_back(child);
                }
                else
                {
                    fields.push_back(component);
                }
            }

            result.push_back(new AST::ObjectDeclarationNode(name, typeName, children, fields));
            return result;
        }
    }
}
