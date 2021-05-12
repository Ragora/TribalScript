/*
    MIT License
*/

grammar Torque;

program  : (block | statement)* ;

ifcontrol : 'if' '(' expression ')' '{' statement* '}' 'else' '{' statement* '}'
          | 'if' '(' expression ')' statement 'else' statement
          | 'if' '(' expression ')' statement
          | 'if' '(' expression ')' '{' statement* '}' ('else if' '(' expression ')' '{' statement* '}')*
          | 'if' '(' expression ')' '{' statement* '}' ('else if' '(' expression ')' statement )* ;

whilecontrol : 'while' '(' expression ')' '{' statement* '}'
             | 'while' '(' expression ')' statement ;

forcontrol : 'for' '(' expression? ';' expression? ';' expression? ')' statement
           | 'for' '(' expression? ';' expression? ';' expression? ')' '{' statement* '}' ;

switchcontrol : 'switch' '$'? '(' expression ')' '{' ('case' expression ':' statement+)* ('default' ':' statement+)? '}' ;

// Control structures
control : whilecontrol
        | ifcontrol
        | forcontrol
        | switchcontrol ;

// Object instantiation
newobject : 'new' LABEL '(' expression? ')' '{' (field | newobject ';')* '}'
          | 'new' LABEL '(' expression? ')' ;

// Functions, datablocks, packages
paramlist : local (',' local)* ;
block : 'function' LABEL '(' paramlist? ')' '{' statement* '}'
      | 'package' LABEL '{' (block)* '}'
      | 'datablock' LABEL '(' LABEL ')' (':' LABEL)? '{' field* '}'
      | newobject ;

statement : (expression ';') | control ;

// Used for setting field values in object instantiation & datablocks
field : LABEL ('[' expression ']')? '=' expression ';' ;

expression : expression ('>'|'>='|'<'|'<='|'=='|'!='|'!$='|'$=') expression
           | expression ('++'|'--')
           | '!' expression
           | '~' expression
           | expression ('@'|'SPC'|'NL'|'TAB') expression
           | expression ('*'|'/') expression
           | expression ('+'|'-') expression
           | expression ('%') expression
           | expression ('&'|'&&') expression
           | expression ('|'|'||') expression
           | expression ('^') expression
           | expression '?' expression ':' expression
           | expression '=' expression
           | expression '[' expression (',' expression)* ']'
           | expression '(' (expression (',' expression)*)* ')'
           | 'return' expression
           | '(' expression ')'
           | control
           | newobject
           | expression ('.' expression)+
           | local
           | global
           | 'true'
           | 'false'
           | STRING
           | LABEL
           | INT
           | FLOAT ;


LABEL: ('0'..'9'|'a'..'z'|'A'..'Z'|'_')+ ('::'LABEL)* ;
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
INT :   '-'? DIGIT+ [Ll]? ;

EXP :   ('E' | 'e') ('+' | '-')? INT ;

FLOAT:  '-'? DIGIT+ '.' DIGIT* EXP? [Ll]?
    |   DIGIT+ EXP? [Ll]?
    |   '.' DIGIT+ EXP? [Ll]?
    ;

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
