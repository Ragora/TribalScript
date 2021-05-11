/*
    MIT License
*/

grammar Torque;

program  : (block | statement)* ;

paramlist : local (',' local)* ;

expression_or_assign_or_function :
                     expression '=' expression_or_assign_or_function
                     | expression;

// Control structures
control : 'while' '(' expression ')' '{' statement* '}'
        | 'if' '(' expression ')' '{' statement* '}' 'else' '{' statement* '}'
        | 'if' '(' expression ')' '{' statement* '}' ;

// Functions, datablocks, packages
block : 'function' LABEL '(' paramlist? ')' '{' statement* '}'
      | 'package' LABEL '{' (block)* '};' ;

statement : (expression ';') | control ;

expression : expression ('>'|'>='|'<'|'<='|'=='|'!=') expression
           | '!' expression
           | '~' expression
           | expression ('+'|'-') expression
           | expression ('*'|'/') expression
           | expression '=' expression
           | expression '(' expression ')'
           | control
           | local
           | global
           | 'true'
           | 'false'
           | STRING
           | LABEL
           | INT
           | FLOAT ;


LABEL: [A-z]+ ('::'LABEL)* ;
local: '%'LABEL ;
global: '$'LABEL ;

// Pulled from C Grammar
LineComment
    :   '//' ~[\r\n]*
        -> skip
    ;

// Pulled from the R grammar
HEXDIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

STRING
    :   '"' ( ESC | ~[\\"] )*? '"'
    |   '\'' ( ESC | ~[\\'] )*? '\''
    ;

fragment
ESC :   '\\' ([abtnfrv]|'"'|'\'')
    |   UNICODE_ESCAPE
    |   HEX_ESCAPE
    |   OCTAL_ESCAPE
    ;

fragment
UNICODE_ESCAPE
    :   '\\' 'u' HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT
    |   '\\' 'u' '{' HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT '}'
    ;

fragment
OCTAL_ESCAPE
    :   '\\' [0-3] [0-7] [0-7]
    |   '\\' [0-7] [0-7]
    |   '\\' [0-7]
    ;

fragment
HEX_ESCAPE
    :   '\\' HEXDIGIT HEXDIGIT?
    ;

DIGIT:  '0'..'9' ;
INT :   DIGIT+ [Ll]? ;

EXP :   ('E' | 'e') ('+' | '-')? INT ;

FLOAT:  DIGIT+ '.' DIGIT* EXP? [Ll]?
    |   DIGIT+ EXP? [Ll]?
    |   '.' DIGIT+ EXP? [Ll]?
    ;

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
