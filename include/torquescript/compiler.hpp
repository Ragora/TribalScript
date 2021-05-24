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
            virtual antlrcpp::Any defaultResult() override
            {
                return GeneratedInstructions();
            }

            virtual antlrcpp::Any visitChildren(antlr4::tree::ParseTree *node) override
            {
                GeneratedInstructions result = GeneratedInstructions();

                size_t n = node->children.size();
                for (size_t i = 0; i < n; i++)
                {
                    GeneratedInstructions childResult = node->children[i]->accept(this).as<GeneratedInstructions>();
                    result.insert(result.end(), childResult.begin(), childResult.end());
                }
                return result;
            }

            virtual antlrcpp::Any visitPackage_declaration(TorqueParser::Package_declarationContext* context) override
            {
                GeneratedInstructions generated;

                mCurrentPackage = context->LABEL()->getText();
                this->visitChildren(context);
                mCurrentPackage = "";
                return generated;
            }

            virtual antlrcpp::Any visitIncrement(TorqueParser::IncrementContext* context) override
            {
                InstructionSequence out = this->collapseInstructions(this->visitChildren(context).as<GeneratedInstructions>());

                out.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(1)));
                out.push_back(std::shared_ptr<Instruction>(new AddAssignmentInstruction()));

                GeneratedInstructions generated;
                generated.push_back(out);
                return generated;
            }

            virtual antlrcpp::Any visitWhile_control(TorqueParser::While_controlContext* context) override
            {
                GeneratedInstructions generated = this->visitChildren(context).as<GeneratedInstructions>();

                InstructionSequence whileExpression = generated[0];
                generated.erase(generated.begin());

                InstructionSequence whileBody = this->collapseInstructions(generated);

                // Modify expression to be conditional - the +2 is to cover the NOP and Jump back in body
                whileExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(whileBody.size() + 2)));

                // Modify body to jump back
                const unsigned int jumpTarget = whileExpression.size() + whileBody.size();
                whileBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(-jumpTarget)));
                whileBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

                GeneratedInstructions result;
                result.push_back(whileExpression);
                result.push_back(whileBody);

                return result;
            }

            virtual antlrcpp::Any visitSwitch_control(TorqueParser::Switch_controlContext* context) override
            {
                GeneratedInstructions switchCode = this->visitChildren(context).as<GeneratedInstructions>();

                InstructionSequence defaultCaseInstructions;
                if (context->default_control())
                {
                    TorqueParser::Default_controlContext* defaultCase = context->default_control();

                    // Load instructions for the default case
                    std::vector<TorqueParser::Expression_statementContext*> defaultStatements = defaultCase->expression_statement();
                    for (auto statement : defaultStatements)
                    {
                        InstructionSequence currentFrame = switchCode.back();
                        defaultCaseInstructions.insert(defaultCaseInstructions.begin(), currentFrame.begin(), currentFrame.end());
                        switchCode.pop_back();
                    }
                }

                // The switch structure is a little more complex - a set of expressions is associated with it so we need to track a list of expressions
                // and the body code
                struct SwitchCaseData
                {
                    std::vector<std::shared_ptr<Instruction>> mCaseBody;
                    std::vector<std::vector<std::shared_ptr<Instruction>>> mExpressions;
                };

                // Now enumerate all case statements
                std::vector<SwitchCaseData> caseData;
                std::vector<TorqueParser::Case_controlContext*> switchCases = context->case_control();
                for (auto iterator = switchCases.rbegin(); iterator != switchCases.rend(); ++iterator)
                {
                    caseData.push_back(SwitchCaseData());
                    SwitchCaseData& currentCaseData = caseData.back();

                    TorqueParser::Case_controlContext* caseContext = *iterator;

                    // Load all statements from the case
                    std::vector<TorqueParser::Expression_statementContext*> caseStatements = caseContext->expression_statement();
                    for (auto statementIterator = caseStatements.begin(); statementIterator != caseStatements.end(); ++statementIterator)
                    {
                        InstructionSequence currentFrame = switchCode.back();
                        currentCaseData.mCaseBody.insert(currentCaseData.mCaseBody.begin(), currentFrame.begin(), currentFrame.end());
                        switchCode.pop_back();
                    }

                    // Load all expressions
                    std::vector<TorqueParser::ExpressionContext*> caseExpressions = caseContext->expression();
                    for (auto expressionIterator = caseExpressions.begin(); expressionIterator != caseExpressions.end(); ++expressionIterator)
                    {
                        currentCaseData.mExpressions.push_back(std::vector<std::shared_ptr<Instruction>>());
                        std::vector<std::shared_ptr<Instruction>>& currentExpression = currentCaseData.mExpressions.back();

                        InstructionSequence currentFrame = switchCode.back();
                        currentExpression.insert(currentExpression.begin(), currentFrame.begin(), currentFrame.end());
                        switchCode.pop_back();
                    }
                }

                // Finally load the expression to switch on
                InstructionSequence switchExpression = switchCode.back();
                switchCode.pop_back();

                // Once we know our expression, we need to cycle through our stored case data and generate handlers for each one
                for (SwitchCaseData& currentCaseData : caseData)
                {
                    std::vector<std::shared_ptr<Instruction>> generatedConditions;

                    // For each sub expression we push our expression
                    const unsigned int expressionCount = currentCaseData.mExpressions.size();
                    for (auto expressionIteration = currentCaseData.mExpressions.begin(); expressionIteration != currentCaseData.mExpressions.end(); ++expressionIteration)
                    {
                        std::vector<std::shared_ptr<Instruction>>& expression = *expressionIteration;

                        // Check if the expression is true - if so, jump to body immediately unless we're the last check then we jump false over the body
                        if (expressionIteration == currentCaseData.mExpressions.begin())
                        {
                            generatedConditions.insert(generatedConditions.begin(), std::shared_ptr<Instruction>(new JumpFalseInstruction(currentCaseData.mCaseBody.size() + 2)));
                        }
                        else
                        {
                            generatedConditions.insert(generatedConditions.begin(), std::shared_ptr<Instruction>(new JumpTrueInstruction(generatedConditions.size() + 1)));
                        }

                        generatedConditions.insert(generatedConditions.begin(), std::shared_ptr<Instruction>(new EqualsInstruction()));
                        generatedConditions.insert(generatedConditions.begin(), switchExpression.begin(), switchExpression.end());
                        generatedConditions.insert(generatedConditions.begin(), expression.begin(), expression.end());

                        generatedConditions[0]->mComment = "Begin Case";
                        generatedConditions[generatedConditions.size() - 1]->mComment = "End Case";
                    }
                    currentCaseData.mCaseBody.insert(currentCaseData.mCaseBody.begin(), generatedConditions.begin(), generatedConditions.end());
                }

                // Stick the default case entry at the end
                caseData.insert(caseData.begin(), SwitchCaseData());
                SwitchCaseData& defaultCaseData = caseData.front();
                defaultCaseData.mCaseBody.insert(defaultCaseData.mCaseBody.begin(), defaultCaseInstructions.begin(), defaultCaseInstructions.end());
                defaultCaseData.mCaseBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));
                defaultCaseData.mCaseBody[0]->mComment = "Begin Default Case";
                defaultCaseData.mCaseBody[defaultCaseData.mCaseBody.size() - 1]->mComment = "End Default Case";

                InstructionSequence currentFrame = InstructionSequence();

                // Output code and insert jumps for true branches
                for (auto iterator = caseData.begin(); iterator != caseData.end(); ++iterator)
                {
                    SwitchCaseData& currentCaseData = *iterator;

                    // Generate a jump to end for the true case
                    if (currentFrame.size() != 0)
                    {
                        currentCaseData.mCaseBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(currentFrame.size())));
                    }
                    else
                    {
                        currentCaseData.mCaseBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));
                    }

                    currentFrame.insert(currentFrame.begin(), currentCaseData.mCaseBody.begin(), currentCaseData.mCaseBody.end());
                }

                GeneratedInstructions result;
                result.push_back(currentFrame);
                return result;
            }

            virtual antlrcpp::Any visitFunction_declaration(TorqueParser::Function_declarationContext* context) override
            {
                GeneratedInstructions generated;
                InstructionSequence out;

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
                    GeneratedInstructions statementCode = this->visitExpression_statement(statement).as<GeneratedInstructions>();
                    InstructionSequence statementSequence = this->collapseInstructions(statementCode);
                    functionBody.insert(functionBody.end(), statementSequence.begin(), statementSequence.end());
                }

                // Generate final declaration
                out.push_back(std::shared_ptr<Instruction>(new FunctionDeclarationInstruction(mCurrentPackage, functionNameSpace, functionName, parameterNames, functionBody)));
                generated.push_back(out);
                return generated;
            }

            virtual antlrcpp::Any visitEquality(TorqueParser::EqualityContext* context) override
            {
                GeneratedInstructions generated;

                InstructionSequence sequence = this->collapseInstructions(this->visitChildren(context).as<GeneratedInstructions>());
                if (context->EQUALS())
                {
                    sequence.push_back(std::shared_ptr<Instruction>(new EqualsInstruction()));
                }
                else
                {
                    throw std::runtime_error("Unknown equality type!");
                }

                generated.push_back(sequence);
                return generated;
            }

            virtual antlrcpp::Any visitArithmetic(TorqueParser::ArithmeticContext* context) override
            {
                GeneratedInstructions generated;

                InstructionSequence sequence = this->collapseInstructions(this->visitChildren(context).as<GeneratedInstructions>());
                if (context->PLUS())
                {
                    sequence.push_back(std::shared_ptr<Instruction>(new AddInstruction()));
                }
                else if (context->MULTIPLY())
                {
                    sequence.push_back(std::shared_ptr<Instruction>(new MultiplyInstruction()));
                }
                else
                {
                    throw std::runtime_error("Unhandled arithmetic type!");
                }

                generated.push_back(sequence);
                return generated;
            }

            virtual antlrcpp::Any visitFor_control(TorqueParser::For_controlContext* context) override
            {
                InstructionSequence generated;

                GeneratedInstructions forInstructions = this->visitChildren(context).as<GeneratedInstructions>();

                // Load for body instructions
                InstructionSequence forBody;
                TorqueParser::Control_statementsContext* controlStatements = context->control_statements();
                if (controlStatements)
                {
                    std::vector<TorqueParser::Expression_statementContext*> statements = controlStatements->expression_statement();

                    for (TorqueParser::Expression_statementContext* statement : statements)
                    {
                        InstructionSequence expressionInstructions = forInstructions.back();
                        forBody.insert(forBody.end(), expressionInstructions.begin(), expressionInstructions.end());
                        forInstructions.pop_back();
                    }
                }

                // Remaining 3 should be the for components
                assert(forInstructions.size() == 3);

                InstructionSequence forInitializer = forInstructions[0];
                InstructionSequence forCondition = forInstructions[1];
                InstructionSequence forAdvance = forInstructions[2];

                // At the end of our loop, run the advance expression
                forBody.insert(forBody.end(), forAdvance.begin(), forAdvance.end());

                // Our body should return to the expression
                const unsigned int jumpTarget = forCondition.size() + forBody.size();
                forBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(-jumpTarget)));
                forBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

                // Check if our expression is false
                forCondition.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(forBody.size())));

                // Output final code
                generated.insert(generated.end(), forInitializer.begin(), forInitializer.end());
                generated.insert(generated.end(), forCondition.begin(), forCondition.end());
                generated.insert(generated.end(), forBody.begin(), forBody.end());

                GeneratedInstructions out;
                out.push_back(generated);
                return out;
            }

            virtual antlrcpp::Any visitIf_control(TorqueParser::If_controlContext* context) override
            {
                InstructionSequence generated;

                // Here we intentionally process in reverse order due to the nature of the jumps needing to know how to far to jump over existing code
                GeneratedInstructions ifInstructions = this->visitChildren(context).as<GeneratedInstructions>();

                // Handle an else if present
                InstructionSequence elseBody;
                if (context->else_control())
                {
                    std::vector<TorqueParser::Expression_statementContext*>  elseControlStatements = context->else_control()->control_statements()->expression_statement();

                    for (unsigned int iteration = 0; iteration < elseControlStatements.size(); ++iteration)
                    {
                        InstructionSequence elseInstructions = ifInstructions.back();
                        ifInstructions.pop_back();

                        elseBody.insert(elseBody.end(), elseInstructions.begin(), elseInstructions.end());
                    }
                }
                elseBody.push_back(std::shared_ptr<Instruction>(new NOPInstruction())); // Add a NOP for jump targets
                generated.insert(generated.end(), elseBody.begin(), elseBody.end());

                // Handle all else if's
                std::vector<TorqueParser::Elseif_controlContext*> elseIfs = context->elseif_control();
                for (TorqueParser::Elseif_controlContext* elseIf : elseIfs)
                {
                    InstructionSequence elseIfBody;

                    TorqueParser::Control_statementsContext* elseIfControlStatements = elseIf->control_statements();
                    if (elseIfControlStatements)
                    {
                        std::vector<TorqueParser::Expression_statementContext*> elseIfStatements = elseIfControlStatements->expression_statement();

                        for (TorqueParser::Expression_statementContext* elseIfStatement : elseIfStatements)
                        {
                            InstructionSequence elseIfInstructions = ifInstructions.back();
                            ifInstructions.pop_back();

                            elseIfBody.insert(elseIfBody.end(), elseIfInstructions.begin(), elseIfInstructions.end());
                        }
                    }

                    InstructionSequence elseIfExpression = ifInstructions.back();
                    ifInstructions.pop_back();

                    // The expression must jump over our body if false
                    elseIfExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(elseIfBody.size() + 2)));

                    // The body, when done, must jump over the remaining code
                    elseIfBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(generated.size())));

                    generated.insert(generated.begin(), elseIfBody.begin(), elseIfBody.end());
                    generated.insert(generated.begin(), elseIfExpression.begin(), elseIfExpression.end());
                }

                // Finally handle the if
                InstructionSequence ifBody;
                if (context->control_statements())
                {
                    std::vector<TorqueParser::Expression_statementContext*> ifStatements = context->control_statements()->expression_statement();

                    for (unsigned int iteration = 0; iteration < ifStatements.size(); ++iteration)
                    {
                        InstructionSequence sequence = ifInstructions.back();
                        ifInstructions.pop_back();

                        ifBody.insert(ifBody.end(), sequence.begin(), sequence.end());
                    }
                }

                InstructionSequence ifExpression = ifInstructions.back();
                ifInstructions.pop_back();

                // Output final code

                // The expression must jump over our body if false
                ifExpression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(ifBody.size() + 2)));

                // The body, when done, must jump over the remaining code
                ifBody.push_back(std::shared_ptr<Instruction>(new JumpInstruction(generated.size())));

                generated.insert(generated.begin(), ifBody.begin(), ifBody.end());
                generated.insert(generated.begin(), ifExpression.begin(), ifExpression.end());

                GeneratedInstructions out;
                out.push_back(generated);
                return out;
            }

            virtual antlrcpp::Any visitAssign(TorqueParser::AssignContext* context) override
            {
                InstructionSequence sequence = this->collapseInstructions(this->visitChildren(context).as<GeneratedInstructions>());
                if (context->ASSIGN())
                {
                    sequence.push_back(std::shared_ptr<Instruction>(new AssignmentInstruction()));
                }
                else
                {
                    throw std::runtime_error("Unhandled assignment type!");
                }

                GeneratedInstructions generated;
                generated.push_back(sequence);
                return generated;
            }

            virtual antlrcpp::Any visitRelational(TorqueParser::RelationalContext* context) override
            {
                InstructionSequence sequence = this->collapseInstructions(this->visitChildren(context).as<GeneratedInstructions>());
                if (context->LESSTHAN())
                {
                    sequence.push_back(std::shared_ptr<Instruction>(new LessThanInstruction()));
                }
                else
                {
                    throw std::runtime_error("Unhandled Relational Type!");
                }

                GeneratedInstructions generated;
                generated.push_back(sequence);
                return generated;
            }

            virtual antlrcpp::Any visitTernary(TorqueParser::TernaryContext* context) override
            {
                GeneratedInstructions generated;
                GeneratedInstructions expressions = this->visitChildren(context).as<GeneratedInstructions>();

                assert(expressions.size() == 3);
                InstructionSequence expression = expressions[0];
                InstructionSequence trueCode = expressions[1];
                InstructionSequence falseCode = expressions[2];

                // We add a NOP to the false expressions for a target to jump to
                falseCode.push_back(std::shared_ptr<Instruction>(new NOPInstruction()));

                // In the true expression we need to jump over the false expression
                trueCode.push_back(std::shared_ptr<Instruction>(new JumpInstruction(falseCode.size())));

                // Jump to the false expression if our expression is false
                expression.push_back(std::shared_ptr<Instruction>(new JumpFalseInstruction(falseCode.size() + 1)));

                // Push generated instructions back
                generated.push_back(expression);
                generated.push_back(trueCode);
                generated.push_back(falseCode);

                return generated;
            }

            virtual antlrcpp::Any visitValue(TorqueParser::ValueContext* context) override
            {
                InstructionSequence out;
                GeneratedInstructions generated;

                if (context->INT())
                {
                    out.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoi(context->INT()->getText()))));
                }
                else if (context->FLOAT())
                {
                    out.push_back(std::shared_ptr<Instruction>(new PushFloatInstruction(std::stof(context->FLOAT()->getText()))));
                }
                else if (context->STRING())
                {
                    // FIXME: Is there a way to utilize the grammar to extract this instead? We don't want the enclosing quotations
                    const std::string rawString = context->STRING()->getText();
                    const std::string stringContent = rawString.substr(1, rawString.size() - 2);
                    out.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(stringContent)));
                }
                else if (context->LABEL())
                {
                    out.push_back(std::shared_ptr<Instruction>(new PushStringInstruction(context->LABEL()->getText())));
                }
                else if (context->TRUE())
                {
                    out.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(1)));
                }
                else if (context->FALSE())
                {
                    out.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(0)));
                }
                else if (context->HEXINT())
                {
                    out.push_back(std::shared_ptr<Instruction>(new PushIntegerInstruction(std::stoul(context->HEXINT()->getText(), nullptr, 16))));
                }
                else
                {
                    throw std::runtime_error("Unhandled expression value type!");
                }

                generated.push_back(out);
                return generated;
            }

            virtual antlrcpp::Any visitGlobalvariable(TorqueParser::GlobalvariableContext* context) override
            {
                InstructionSequence out;
                GeneratedInstructions generated;

                std::string variableName = "";
                std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = context->labelwithkeywords();

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
                out.push_back(std::shared_ptr<Instruction>(new PushGlobalReferenceInstruction(variableName)));

                generated.push_back(out);
                return generated;
            }

            virtual antlrcpp::Any visitLocalvariable(TorqueParser::LocalvariableContext* context) override
            {
                InstructionSequence out;
                GeneratedInstructions generated;

                std::string variableName = "";
                std::vector<TorqueParser::LabelwithkeywordsContext*> variableNameComponents = context->labelwithkeywords();

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
                out.push_back(std::shared_ptr<Instruction>(new PushLocalReferenceInstruction(variableName)));

                generated.push_back(out);
                return generated;
            }

            virtual antlrcpp::Any visitFunctioncall_expression(TorqueParser::Functioncall_expressionContext* context) override
            {
                InstructionSequence out;
                GeneratedInstructions generated;

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

                // Load function parameters
                GeneratedInstructions callParameters = this->visitChildren(context).as<GeneratedInstructions>();

                // Output code
                InstructionSequence collapsedParameters = this->collapseInstructions(callParameters);
                out.insert(out.end(), collapsedParameters.begin(), collapsedParameters.end());
                out.push_back(std::shared_ptr<Instruction>(new CallFunctionInstruction(calledFunctionNameSpace, calledFunctionName, callParameters.size())));

                generated.push_back(out);
                return generated;
            }

            InstructionSequence collapseInstructions(GeneratedInstructions instructions)
            {
                InstructionSequence result;

                for (InstructionSequence sequence : instructions)
                {
                    result.insert(result.end(), sequence.begin(), sequence.end());
                }
                return result;
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
