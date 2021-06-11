#include <torquescript/interpreter.hpp>
#include <torquescript/libraries/libraries.hpp>

int main(int argc, char* argv[])
{
    // Initialize interpreter configuration - this is optional but can be used to set settings
    TorqueScript::InterpreterConfiguration config;
    config.mMaxRecursionDepth = 1024;

    // Initialize interpreter
    TorqueScript::Interpreter interpreter(config);
    TorqueScript::registerAllLibraries(&interpreter);

    // Ask interpreter to evaluate a statement
    interpreter.evaluate("echo(\"Hi\");");

    return 0;
}
