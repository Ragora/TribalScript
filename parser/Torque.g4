/*
    MIT License
*/

grammar Torque;

program  : (outerblock | statement)* ;

elseifcontrol : ('else if' '(' expression ')' '{' statement* '}')
              | ('else if' '(' expression ')' statement ) ;

elsecontrol : 'else' '{' statement* '}'
            | 'else' statement ;

ifcontrol : 'if' '(' expression ')' statement elseifcontrol* elsecontrol?
          | 'if' '(' expression ')' '{' statement* '}' elseifcontrol* elsecontrol? ;

whilecontrol : 'while' '(' expression ')' '{' statement* '}'
             | 'while' '(' expression ')' statement ;

forcontrol : 'for' '(' expression? ';' expression? ';' expression? ')' statement
           | 'for' '(' expression? ';' expression? ';' expression? ')' '{' statement* '}' ;

// In Torque, the case values are apparently expressions
switchcase : 'case' expression switchcasealternative* ':' statement+ ;
switchcasealternative : ('or' expression) ;

// Switch has two forms - switch and switch$ for string values
switchcontrol : 'switch' '$'? '(' expression ')' '{' switchcase* ('default' ':' statement+)? '}' ;

// Control structures
control : whilecontrol
        | ifcontrol
        | forcontrol
        | returncontrol
        | switchcontrol ;

// Object instantiation - we support two forms new TYPENAME(NAME) and new (TYPENAMEEXP)(NAME) both with optional initialization lists
objectinitialization : '{' (field | newobject ';')* '}' ;
newobject : 'new' LABEL '(' expression? ')' objectinitialization?
          | 'new' '(' expression ')' '(' expression? ')' objectinitialization? ;

// Functions, datablocks, packages
paramlist : localvariable (',' localvariable)* ;
functiondeclaration : 'function' labelnamespace '(' paramlist? ')' '{' statement* '}' ;
packagedeclaration : 'package' LABEL '{' functiondeclaration* '}' ';' ;

// Datablock declaration requires at least one field
datablockdeclaration : 'datablock' LABEL '(' LABEL ')' (':' LABEL)? '{' field+ '}' ';' ;

outerblock : functiondeclaration
           | packagedeclaration
           | datablockdeclaration ;

statement : (expression ';') | control ;

// Used for setting field values in object instantiation & datablocks
field : LABEL ('[' expression ']')? '=' expression ';' ;

unary : '!' expression
      | '-' expression
      | '~' expression ;

returncontrol : 'return' expression? ;

expression : expression ('>'|'>='|'<'|'<=') expression
           | expression ('++'|'--')
           | expression ('=='|'!='|'!$='|'$=') expression
           | unary
           | expression ('@'|'SPC'|'NL'|'TAB') expression
           | expression ('*'|'/'|'%') expression
           | expression ('+'|'-') expression
           | expression ('||'|'&&') expression
           | expression ('&'|'|') expression
           | expression ('^') expression
           | expression '?' expression ':' expression
           | expression ('='|'+='|'*='|'-='|'%=') expression
           | expression '[' expression (',' expression)* ']'
           | expression '(' (expression (',' expression)*)* ')'
           | control
           | newobject
           | expression ('.' expression)+
           | localvariable
           | globalvariable
           | 'true'
           | 'false'
           | STRING
           | labelnamespacesingle
           | labelnamespace
           | label
           | INT
           | '(' expression ')'
           | FLOAT ;

// Function declarations can be namespaced but only a single deep ie. function one::two() {}
labelnamespacesingle : LABEL ('::' LABEL)? ;

// Some cases like variables can go infinitely deep with namespacing
labelnamespace : LABEL ('::' LABEL)* ;

// Finally some cases like object names are just a label with no possibility of namespacing
label: LABEL ;

LABEL : ('0'..'9'|'a'..'z'|'A'..'Z'|'_')+ ;

localvariable: '%'labelnamespace ;
globalvariable: '$'labelnamespace ;

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
    |   HEX_ESCAPE
    |   COLOR_ESCAPE
    ;

fragment
HEX_ESCAPE
    :   '\\x' HEXDIGIT HEXDIGIT?
    ;

// NOTE: I'm uncertain as to what these \c escapes actually signify aside from what they do in the T2 chat
fragment
COLOR_ESCAPE
    :   '\\c' (DIGIT|'p'|'o') ;

DIGIT:  '0'..'9' ;
INT :   '-'? DIGIT+ [Ll]? ;

EXP :   ('E' | 'e') ('+' | '-')? INT ;

FLOAT:  '-'? DIGIT+ '.' DIGIT* EXP? [Ll]?
    |   DIGIT+ EXP? [Ll]?
    |   '.' DIGIT+ EXP? [Ll]?
    ;

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
