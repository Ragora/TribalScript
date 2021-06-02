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

grammar Torque;

program  : statement+ | <EOF> ;

/*
    Main Blocks
*/
function_declaration : FUNCTION LABEL '(' function_declaration_parameters? ')' '{' expression_statement* '}'
                     | FUNCTION LABEL '::' LABEL '(' function_declaration_parameters? ')' '{' expression_statement* '}' ;
function_declaration_parameters : localvariable (',' localvariable)* ;

package_declaration : PACKAGE LABEL '{' function_declaration+ '}' ';' ;

datablock_declaration : DATABLOCK LABEL '(' LABEL ')' (':' LABEL)? '{' field_assign+ '}' ;

field_assign : labelwithkeywords '=' expression ';'
             | labelwithkeywords '[' expression_list ']' '=' expression ';' ;
object_initialization : '{' field_assign* (object_declaration ';')* '}' ;

// NOTE: In T2 it's possible to pass multiple values in the type & name, what do they do exactly?
object_declaration : NEW LABEL '(' (name=expression)? ')' object_initialization?
                   | NEW '(' expression ')' '(' (name=expression)? ')' object_initialization? ;

/*
    Control blocks
*/
control_statements : '{' expression_statement* '}'
                   | expression_statement ;

while_control : WHILE '(' expression ')' control_statements ;

for_control : FOR '(' expression ';' expression ';' expression ')' control_statements ;

else_control : ELSE control_statements ;
elseif_control : ELSE IF '(' expression ')' control_statements ;
if_control : IF '(' expression ')' control_statements elseif_control* else_control? ;

default_control : DEFAULT ':' expression_statement* ;
case_control : CASE expression ('or' expression)* ':' expression_statement* ;
switch_control : SWITCH '$'? '(' expression ')' '{' case_control+ default_control? '}' ;

break_control : BREAK ;
continue_control : CONTINUE ;
return_control : RETURN expression? ;

/*
    Expressions and Statements
*/

expression_statement : primary_expression ';'
                     | while_control
                     | for_control
                     | if_control
                     | switch_control
                     | continue_control ';'
                     | break_control ';'
                     | return_control ';' ;

statement : function_declaration
          | package_declaration
          | expression_statement  ;

expression_list : expression (',' expression)* ;

functioncall_expression : LABEL '(' expression_list? ')'                        # call
                        | LABEL '::' LABEL '(' expression_list? ')'             # call
                        | (lvalue | rvalue) '.' LABEL '(' expression_list? ')'  # subcall ;

// Root level expression - because expressions like `1;` are not valid - it must be actionable
primary_expression : functioncall_expression                                       # callExpression
                   | primary_expression '.' primary_expression                     # primaryExpressionSubfield
                   | lvalue (op=ASSIGN
                            |op=PLUSASSIGN
                            |op=MINUSASSIGN
                            |op=MULTIPLYASSIGN
                            |op=DIVIDEASSIGN
                            |op=ORASSIGN
                            |op=MODULUSASSIGN
                            |op=ANDASSIGN) expression                               # assign
                   | lvalue '++'                                                    # increment
                   | lvalue '--'                                                    # decrement
                   | object_declaration                                             # objectDeclarationExpression
                   | datablock_declaration                                          # datablockDeclarationExpression ;

// Only valid on the right side of an assignment, however still valid on the left side of a '.'
rvalue : INT                                                                # value
       | HEXINT                                                             # value
       | STRING                                                             # value
       | TAGGEDSTRING                                                       # value
       | FLOAT                                                              # value
       | LABEL                                                              # value
       | TRUE                                                               # value
       | FALSE                                                              # value
       | object_declaration                                                 # objectDeclarationRValue ;

// Valid on both the left and right sides of an assignment
lvalue : (globalvariable | localvariable | LABEL) '[' expression_list ']'   # array
       | lvalue '.' LABEL '[' expression_list ']'                           # subarray
       | rvalue '.' LABEL '[' expression_list ']'                           # subarray
       | localvariable                                                      # localValue
       | globalvariable                                                     # globalValue
       | rvalue '.' LABEL                                                   # subfield
       | lvalue '.' LABEL                                                   # subfield ;

expression : (op=MINUS
             |op=NOT
             |op=TILDE) expression                                              # unary
           | primary_expression                                                 # primaryExpressionReference
           | expression '.' expression                                          # subfieldExpression
           | lvalue                                                             # lvalueExpression
           | '(' expression ')'                                                 # parentheses
           | expression '^' expression                                          # bitwise
           | expression '&' expression                                          # bitwise
           | expression '|' expression                                          # bitwise
           | expression (op=MODULUS|op=MULTIPLY|op=DIVIDE) expression           # arithmetic
           | expression (op=PLUS
                        |op=MINUS) expression                                   # arithmetic
           | expression '?' expression ':' expression                           # ternary
           | expression (op=LESSTHAN
                        |op=GREATERTHAN
                        |op=LESSTHANOREQUAL
                        |op=GREATERTHANOREQUAL) expression                      # relational
           | expression '||' expression                                         # logical
           | expression '<<' expression                                         # bitshift
           | expression '>>' expression                                         # bitshift
           | expression '&&' expression                                         # logical
           | expression (op=EQUALS
                        |op=NOTEQUAL
                        |op=STRINGEQUALS
                        |op=STRINGNOTEQUAL) expression                          # equality
           | expression (op=CONCAT
                        |op=TABCONCAT
                        |op=SPACECONCAT
                        |op=NEWLINECONCAT) expression                           # concat
           | rvalue                                                             # rvalueExpression ;

// For the grammar to work correctly, we need to explicitly allow these keywords to be used in variable names
labelwithkeywords : LABEL | PACKAGE | RETURN | WHILE | FALSE | TRUE | FUNCTION | ELSE | IF | DATABLOCK | CASE ;
localvariable : '%' labelwithkeywords ('::' labelwithkeywords)* ;
globalvariable : '$' labelwithkeywords ('::' labelwithkeywords)* ;

/*
    Lexer
*/
DATABLOCK : 'datablock' ;
PACKAGE : 'package' ;
FUNCTION : 'function' ;
IF : 'if';
ELSE : 'else' ;
SWITCH : 'switch';
CASE : 'case' ;
RETURN : 'return';
BREAK : 'break';
NEW : 'new' ;
WHILE : 'while' ;
FOR : 'for' ;
TRUE : 'true' ;
FALSE : 'false' ;
DEFAULT : 'default' ;
PLUS : '+' ;
MINUS : '-' ;
MULTIPLY : '*' ;
DIVIDE : '/' ;
MODULUS : '%' ;
ASSIGN : '=';
PLUSASSIGN : '+=' ;
MULTIPLYASSIGN : '*=' ;
ORASSIGN : '|=' ;
ANDASSIGN : '&=' ;
MODULUSASSIGN : '%=' ;
DIVIDEASSIGN : '/=' ;
MINUSASSIGN : '-=' ;
LESSTHAN : '<' ;
GREATERTHAN : '>' ;
LESSTHANOREQUAL : '<=' ;
GREATERTHANOREQUAL : '>=' ;
NOT : '!' ;
TILDE : '~' ;
EQUALS : '==' ;
STRINGEQUALS : '$=' ;
NOTEQUAL : '!=' ;
STRINGNOTEQUAL : '!$=' ;
CONCAT : '@' ;
SPACECONCAT : 'SPC' ;
TABCONCAT : 'TAB' ;
NEWLINECONCAT : 'NL' ;
CONTINUE : 'continue' ;

// Labels can contain numbers but not at the start
LABEL : [a-zA-Z_]+[a-zA-Z_0-9]* ;

INT :   DIGIT+ ;
HEXINT : '0x' HEXDIGIT+ ;
DIGIT:  '0'..'9' ;

HEXDIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

STRING : '"' ( ESC | ~[\\"] )*? '"' ;
TAGGEDSTRING : '\'' ( ESC | ~[\\'] )*? '\'' ;

fragment
ESC :   '\\' ([A-Za-z0-9|{}])
    |   HEX_ESCAPE
    |   COLOR_ESCAPE
    ;

fragment
HEX_ESCAPE
    :   '\\x' HEXDIGIT HEXDIGIT?
    ;

// NOTE: I'm uncertain as to what these \c escapes actually signify aside from what they do in the T2 chat and console
fragment
COLOR_ESCAPE
    :   '\\c' (DIGIT|'p'|'o') ;

EXP :   ('E' | 'e') ('+' | '-')? INT ;
FLOAT:  DIGIT+ '.' DIGIT+ EXP?
    |   DIGIT+ EXP?
    ;

// Ignore line comments
LineComment : '//' ~[\r\n]* -> skip ;

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
