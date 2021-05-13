#pragma once

#include "antlr4-runtime.h"
#include <TorqueBaseListener.h>

#include <torquescript/compiler/codeblock.h>

class Compiler : public TorqueBaseListener
{
    public:
        virtual void enterFunctiondeclaration(TorqueParser::FunctiondeclarationContext * /*ctx*/) override;
        virtual void exitFunctiondeclaration(TorqueParser::FunctiondeclarationContext * /*ctx*/) override;
};
