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
    class CompilerVisitor : public TorqueBaseVisitor
    {
        public:
            virtual antlrcpp::Any visitProgram(TorqueParser::ProgramContext* context) override
            {
                InstructionSequence generated;

                std::vector<TorqueParser::StatementContext*> statements = context->statement();

                for (TorqueParser::StatementContext* statement : statements)
                {
                    InstructionSequence statementCode = this->visitStatement(statement).as<InstructionSequence>();
                    generated.insert(generated.end(), statementCode.begin(), statementCode.end());
                }
                return generated;
            }

            virtual antlrcpp::Any visitStatement(TorqueParser::StatementContext* context) override
            {
                InstructionSequence generated;

                /*
                Function_declarationContext *function_declaration();
                Package_declarationContext *package_declaration();
                Datablock_declarationContext *datablock_declaration();
                Expression_statementContext *expression_statement();
                */

                if (context->function_declaration())
                {
                    InstructionSequence functionCode = this->visitFunction_declaration(context->function_declaration()).as<InstructionSequence>();
                    generated.insert(generated.end(), functionCode.begin(), functionCode.end());
                }
                else if (context->expression_statement())
                {
                    InstructionSequence expressionCode = this->visitExpression_statement(context->expression_statement()).as<InstructionSequence>();
                    generated.insert(generated.end(), expressionCode.begin(), expressionCode.end());
                }
                else if (context->package_declaration())
                {
                    InstructionSequence packageCode = this->visitPackage_declaration(context->package_declaration()).as<InstructionSequence>();
                    generated.insert(generated.end(), packageCode.begin(), packageCode.end());
                }
                else
                {
                    throw std::runtime_error("Unknown Statement type!");
                }
                return generated;
            }

            virtual antlrcpp::Any visitPackage_declaration(TorqueParser::Package_declarationContext* context) override
            {
                InstructionSequence generated;

                mCurrentPackage = context->LABEL()->getText();

                std::vector<TorqueParser::Function_declarationContext*> functionDeclarations = context->function_declaration();
                for (TorqueParser::Function_declarationContext* functionDeclaration : functionDeclarations)
                {
                    InstructionSequence functionCode = this->visitFunction_declaration(functionDeclaration).as<InstructionSequence>();
                    generated.insert(generated.end(), functionCode.begin(), functionCode.end());
                }

                mCurrentPackage = "";
                return generated;
            }

            virtual antlrcpp::Any visitFunction_declaration(TorqueParser::Function_declarationContext* context) override
            {
                InstructionSequence generated;

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
                else
                {
                    functionName = functionNameComponents[0]->getText();
                }

                // Load function code
                InstructionSequence functionBody;

                std::vector<TorqueParser::Expression_statementContext*> functionStatements = context->expression_statement();
                for (TorqueParser::Expression_statementContext* statement : functionStatements)
                {
                    InstructionSequence statementCode = this->visitExpression_statement(statement).as<InstructionSequence>();
                    functionBody.insert(functionBody.end(), statementCode.begin(), statementCode.end());
                }

                // Generate final declaration
                generated.push_back(std::shared_ptr<Instruction>(new FunctionDeclarationInstruction(mCurrentPackage, functionNameSpace, functionName, parameterNames, functionBody)));
                return generated;
            }

            virtual antlrcpp::Any visitExpression_statement(TorqueParser::Expression_statementContext* context) override
            {
                InstructionSequence generated;

                if (context->expression())
                {
                    InstructionSequence expressionCode = this->visitExpression(context->expression()).as<InstructionSequence>();
                    generated.insert(generated.end(), expressionCode.begin(), expressionCode.end());
                }
                else if (context->while_control())
                {
                    InstructionSequence whileCode = this->visitWhile_control(context->while_control()).as<InstructionSequence>();
                    generated.insert(generated.end(), whileCode.begin(), whileCode.end());
                }
                else if (context->for_control())
                {
                    InstructionSequence forCode = this->visitFor_control(context->for_control()).as<InstructionSequence>();
                    generated.insert(generated.end(), forCode.begin(), forCode.end());
                }
                else
                {
                    throw std::runtime_error("Unhandled expression statement type!");
                }

                return generated;
            }

            virtual antlrcpp::Any visitFor_control(TorqueParser::For_controlContext* context) override
            {
                // There should be three expressions
                std::vector<TorqueParser::ExpressionContext*> forExpressions = context->expression();
                assert(forExpressions.size() == 3);

                InstructionSequence forInitializer = this->visitExpression(forExpressions[0]).as<InstructionSequence>();
                InstructionSequence forCondition = this->visitExpression(forExpressions[1]).as<InstructionSequence>();
                InstructionSequence forAdvance = this->visitExpression(forExpressions[2]).as<InstructionSequence>();

                // Load the body
                InstructionSequence forBody;
                TorqueParser::Control_statementsContext* controlStatements = context->control_statements();
                if (controlStatements)
                {
                    std::vector<TorqueParser::Expression_statementContext*> statements = controlStatements->expression_statement();

                    for (TorqueParser::Expression_statementContext* statement : statements)
                    {
                        InstructionSequence statementCode = this->visitExpression_statement(statement).as<InstructionSequence>();
                        forBody.insert(forBody.end(), statementCode.begin(), statementCode.end());
                    }
                }

                // At the end of our loop, run the advance expression
                forBody.insert(forBody.end(), forAdvance.begin(), forAdvance.end());

                // Our body should return to the expression
                const unsigned int jumpTarget = forCondition.size() + forBody.size();
                forBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(-jumpTarget)));
                forBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

                // Check if our expression is false
                forCondition.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(forBody.size())));

                // Output final code
                InstructionSequence generated;

                generated.insert(generated.end(), forInitializer.begin(), forInitializer.end());
                generated.insert(generated.end(), forCondition.begin(), forCondition.end());
                generated.insert(generated.end(), forBody.begin(), forBody.end());

                return generated;
            }

            virtual antlrcpp::Any visitWhile_control(TorqueParser::While_controlContext* context) override
            {
                assert(context->expression());

                InstructionSequence whileBody;
                InstructionSequence whileExpression = this->visitExpression(context->expression()).as<InstructionSequence>();

                // Load the while body
                TorqueParser::Control_statementsContext* controlStatements = context->control_statements();
                if (controlStatements)
                {
                    std::vector<TorqueParser::Expression_statementContext*> statements = controlStatements->expression_statement();

                    for (TorqueParser::Expression_statementContext* statement : statements)
                    {
                        InstructionSequence statementCode = this->visitExpression_statement(statement).as<InstructionSequence>();
                        whileBody.insert(whileBody.end(), statementCode.begin(), statementCode.end());
                    }
                }

                // Modify expression to be conditional - the +2 is to cover the NOP and Jump back in body
                whileExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(whileBody.size() + 2)));

                // Modify body to jump back
                const unsigned int jumpTarget = whileExpression.size() + whileBody.size();
                whileBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(-jumpTarget)));
                whileBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

                InstructionSequence generated;
                generated.insert(generated.end(), whileExpression.begin(), whileExpression.end());
                generated.insert(generated.end(), whileBody.begin(), whileBody.end());

                return generated;
            }

            antlrcpp::Any visitExpression(TorqueParser::ExpressionContext* context)
            {
                InstructionSequence generated;

                // NOTE: Is there a better way to do this than using a shotgun dynamic_cast ?
                // Using visitChildren didn't seem to cast correctly so we seem to be forced to check each condition ?
                TorqueParser::CallContext* call = dynamic_cast<TorqueParser::CallContext*>(context);
                TorqueParser::ValueContext* value = dynamic_cast<TorqueParser::ValueContext*>(context);
                TorqueParser::ArithmeticContext* arithmetic = dynamic_cast<TorqueParser::ArithmeticContext*>(context);
                TorqueParser::RelationalContext* relational = dynamic_cast<TorqueParser::RelationalContext*>(context);
                TorqueParser::LocalValueContext* localValue = dynamic_cast<TorqueParser::LocalValueContext*>(context);
                TorqueParser::GlobalValueContext* globalValue = dynamic_cast<TorqueParser::GlobalValueContext*>(context);
                TorqueParser::AssignContext* assign = dynamic_cast<TorqueParser::AssignContext*>(context);
                TorqueParser::ParenthesesContext* parentheses = dynamic_cast<TorqueParser::ParenthesesContext*>(context);
                TorqueParser::IncrementContext* increment = dynamic_cast<TorqueParser::IncrementContext*>(context);
                TorqueParser::DecrementContext* decrement = dynamic_cast<TorqueParser::DecrementContext*>(decrement);

                if (call)
                {
                    InstructionSequence callCode = this->visitCall(call).as<InstructionSequence>();
                    generated.insert(generated.end(), callCode.begin(), callCode.end());
                }
                else if (localValue)
                {
                    std::string variableName = "";
                    std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = localValue->localvariable()->labelwithkeywords();

                    // NOTE: For now we just combine namespace components into a single value
                    for (TorqueParser::LabelwithkeywordsContext* component : variableNameComponents)
                    {
                        if (variableName == "")
                        {
                            variableName = component->getText();
                        }
                        else
                        {
                            variableName += "::" + component->getText();
                        }
                    }
                    generated.push_back(std::shared_ptr<Instruction>(new PushLocalReferenceInstruction(variableName)));
                }
                else if (globalValue)
                {
                    std::string variableName = "";
                    std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = globalValue->globalvariable()->labelwithkeywords();

                    // NOTE: For now we just combine namespace components into a single value
                    for (TorqueParser::LabelwithkeywordsContext* component : variableNameComponents)
                    {
                        if (variableName == "")
                        {
                            variableName = component->getText();
                        }
                        else
                        {
                            variableName += "::" + component->getText();
                        }
                    }
                    generated.push_back(std::shared_ptr<Instruction>(new PushGlobalReferenceInstruction(variableName)));
                }
                else if (value)
                {
                    if (value->INT())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoi(value->INT()->getText()))));
                    }
                    else if (value->FLOAT())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new PushFloatInstruction(std::stof(value->FLOAT()->getText()))));
                    }
                    else if (value->STRING())
                    {
                        // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
                        const std::string rawString = value->STRING()->getText();
                        const std::string stringContent = rawString.substr(1, rawString.size() - 2);
                        generated.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(stringContent)));
                    }
                    else if (value->LABEL())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(value->LABEL()->getText())));
                    }
                    else if (value->TRUE())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(1)));
                    }
                    else if (value->FALSE())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(0)));
                    }
                    else if (value->HEXINT())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoul(value->HEXINT()->getText(), nullptr, 16))));
                    }
                    else
                    {
                        throw std::runtime_error("Unhandled expression value type!");
                    }
                }
                else if (increment)
                {
                    InstructionSequence lhsCode = this->visitExpression(increment->expression()).as<InstructionSequence>();

                    generated.insert(generated.end(), lhsCode.begin(), lhsCode.end());

                    generated.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(1)));
                    generated.push_back(std::shared_ptr<Instruction>(new AddAssignmentInstruction()));
                }
                else if (relational)
                {
                    std::vector<TorqueParser::ExpressionContext*> expressions = relational->expression();
                    InstructionSequence lhsCode = this->visitExpression(expressions[0]).as<InstructionSequence>();
                    InstructionSequence rhsCode = this->visitExpression(expressions[1]).as<InstructionSequence>();

                    generated.insert(generated.end(), lhsCode.begin(), lhsCode.end());
                    generated.insert(generated.end(), rhsCode.begin(), rhsCode.end());

                    if (relational->LESSTHAN())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new LessThanInstruction()));
                    }
                    else
                    {
                        throw std::runtime_error("Unhandled Relational Type!");
                    }
                }
                else if (arithmetic)
                {
                    std::vector<TorqueParser::ExpressionContext*> expressions = arithmetic->expression();
                    InstructionSequence lhsCode = this->visitExpression(expressions[0]).as<InstructionSequence>();
                    InstructionSequence rhsCode = this->visitExpression(expressions[1]).as<InstructionSequence>();

                    generated.insert(generated.end(), lhsCode.begin(), lhsCode.end());
                    generated.insert(generated.end(), rhsCode.begin(), rhsCode.end());

                    if (arithmetic->PLUS())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new AddInstruction()));
                    }
                    else if (arithmetic->MULTIPLY())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new MultiplyInstruction()));
                    }
                    else
                    {
                        throw std::runtime_error("Unhandled arithmetic type!");
                    }
                }
                else if (assign)
                {
                    std::vector<TorqueParser::ExpressionContext*> expressions = assign->expression();
                    InstructionSequence lhsCode = this->visitExpression(expressions[0]).as<InstructionSequence>();
                    InstructionSequence rhsCode = this->visitExpression(expressions[1]).as<InstructionSequence>();

                    generated.insert(generated.end(), lhsCode.begin(), lhsCode.end());
                    generated.insert(generated.end(), rhsCode.begin(), rhsCode.end());

                    if (assign->ASSIGN())
                    {
                        generated.push_back(std::shared_ptr<Instruction>(new AssignmentInstruction()));
                    }
                    else
                    {
                        throw std::runtime_error("Unhandled assignment type!");
                    }
                }
                else if (parentheses)
                {
                    InstructionSequence parenthesesCode = this->visitExpression(parentheses->expression()).as<InstructionSequence>();
                    generated.insert(generated.end(), parenthesesCode.begin(), parenthesesCode.end());
                }
                else
                {
                    throw std::runtime_error("Unhandled expression type!");
                }

                return generated;
            }

            virtual antlrcpp::Any visitCall(TorqueParser::CallContext* context) override
            {
                InstructionSequence generated;

                TorqueParser::Functioncall_expressionContext* callExpression = context->functioncall_expression();

                // Load function name
                std::string calledFunctionName = "";
                std::string calledFunctionNameSpace = "";

                std::vector<antlr4::tree::TerminalNode*> calledFunctionNameComponents = callExpression->LABEL();
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

                // Load function parameters
                InstructionSequence parameterCode;
                TorqueParser::Expression_listContext* expressionList = callExpression->expression_list();

                unsigned int parameterCount = 0;
                if (expressionList)
                {
                    std::vector<TorqueParser::ExpressionContext*> parameterExpressions = expressionList->expression();

                    parameterCount = parameterExpressions.size();
                    for (TorqueParser::ExpressionContext* parameterExpression : parameterExpressions)
                    {
                        InstructionSequence expressionCode = this->visitExpression(parameterExpression).as<InstructionSequence>();
                        parameterCode.insert(parameterCode.end(), expressionCode.begin(), expressionCode.end());
                    }
                }

                // Output code
                generated.insert(generated.end(), parameterCode.begin(), parameterCode.end());
                generated.push_back(std::shared_ptr<Instruction>(new CallFunctionInstruction(calledFunctionNameSpace, calledFunctionName, parameterCount)));

                return generated;
            }

        private:
            std::string mCurrentPackage;
    };

    /**
     *  @brief Primary compiler class. Here we use a listener to walk the tree generated by ANTLR in order
     *  to generate executable bytecode for the virtual machine.
     */
    class Compiler
    {
        public:
            /**
             *  @brief Ask the compiler to generate a codeblock from the provided stream.
             *  @param input The input to generate from.
             *  @return A CodeBlock representing the compiled output. If an error has occurred, nullptr is returned.
             */
            CodeBlock* compileStream(std::istream& input);

            /**
             *  @brief Ask the compiler to generate a codeblock from the provided string.
             *  @param input The string to generate from.
             */
            CodeBlock* compileString(const std::string& input);

            /**
             *  @brief Ask the compiler to generate a codeblock from the provided file.
             *  @param path The path to load from.
             */
            CodeBlock* compileFile(const std::string& path);
    };
}
