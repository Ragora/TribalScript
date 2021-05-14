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

// In Torque, all for components are required
forcontrol : 'for' '(' expression ';' expression ';' expression ')' statement
           | 'for' '(' expression ';' expression ';' expression ')' '{' statement* '}' ;

// In Torque, the case values are apparently expressions
switchcase : 'case' expression switchcasealternative* ':' statement+ ;
switchcasealternative : ('or' expression) ;

// Switch has two forms - switch and switch$ for string values
switchcontrol : 'switch' '$'? '(' expression ')' '{' switchcase* ('default' ':' statement+)? '}' ;

// Control structures
control : whilecontrol
        | ifcontrol
        | forcontrol
        | switchcontrol ;

// Object instantiation - we support two forms new TYPENAME(NAME) and new (TYPENAMEEXP)(NAME) both with optional initialization lists
objectinitialization : '{' (field | newobject ';')* '}' ;
newobject : 'new' LABEL '(' expression? ')' objectinitialization?
          | 'new' '(' expression ')' '(' expression? ')' objectinitialization? ;

// Functions, datablocks, packages
paramlist : localvariable (',' localvariable)* ;
functiondeclaration : 'function' LABELNAMESPACESINGLE '(' paramlist? ')' '{' statement* '}' ;
packagedeclaration : 'package' LABEL '{' functiondeclaration* '}' ';' ;

// Datablock declaration requires at least one field
datablockdeclaration : 'datablock' LABEL '(' LABEL ')' (':' LABEL)? '{' field+ '}' ';' ;

outerblock : functiondeclaration
           | packagedeclaration
           | datablockdeclaration ;

statement : control
          | returncontrol ';'
          | breakcontrol ';'
          | expression ';' ;

// Used for setting field values in object instantiation & datablocks
field : LABEL ('[' expression ']')? '=' expression ';' ;

returncontrol : 'return' expression? ;
breakcontrol : 'break' ;
trueliteral: 'true' ;
falseliteral : 'false' ;

lvalue : localvariable
       | globalvariable ;

expression : (op=NOT|op=MINUS) expression                                                                                                                                                                           # unary
           | expression (op=PLUSPLUS|op=MINUSMINUS)                                                                                                                                                                 # unary
           | expression '(' expression? (',' expression)* ')'                                                                                                                                                       # call
           | (localvariable | globalvariable) '[' expression (',' expression)* ']'                                                                                                                                  # array
           | expression ('.' LABEL)+                                                                                                                                                                                # subreference
           | '(' expression ')'                                                                                                                                                                                     # parenthesis
           | expression (op=MULT|op=DIV|op=MODULUS) expression                                                                                                                                                      # arithmetic
           | expression (op=PLUS|op=MINUS) expression                                                                                                                                                               # arithmetic
           | expression (op=LEFTSHIFT|op=RIGHTSHIFT) expression                                                                                                                                                     # arithmetic
           | expression (op=LESS|op=LESSEQ|op=BIGGER|op=BIGGEREQ) expression                                                                                                                                        # relational
           | expression (op=EQUAL|op=NOTEQUAL|op=STRINGEQUAL|op=STRINGNOTEQUAL) expression                                                                                                                          # equality
           | expression (op=BITWISEAND|op=EXCLUSIVEOR|op=BITWISEOR) expression                                                                                                                                      # bitwiseAnd
           | expression (op=CONCAT|op=SPACE|op=NEWLINE|op=TAB) expression                                                                                                                                           # concatenation
           | expression (op=AND|op=OR) expression                                                                                                                                                                   # logicalop
           | expression '?' expression ':' expression                                                                                                                                                               # ternary
           | expression (op=ASSIGN|op=ADDASSIGN|op=MULTASSIGN|op=SUBASSIGN|op=MODULUSASSIGN|op=BITWISEORASSIGN|op=BITWISEANDASSIGN|op=EXLUSIVEORASSIGN|op=LEFTSHIFTASSIGN|op=RIGHTSHIFTASSIGN)  expression          # assignment
           | op=newobject                                                                                                                                                                                           # objectInstantiation
           | op=lvalue                                                                                                                                                                                              # expressionLValue
           | op=INT                                                                                                                                                                                                 # value
           | op=FLOAT                                                                                                                                                                                               # value
           | op=LABELNAMESPACESINGLE                                                                                                                                                                                # labelReference
           | op=BOOLEAN                                                                                                                                                                                             # value
           | op=STRING                                                                                                                                                                                              # value ;

// Function declarations can be namespaced but only a single deep ie. function one::two() {}
LABELNAMESPACESINGLE : LABEL ('::' LABEL)? ;

// Some cases like variables can go infinitely deep with namespacing
LABELNAMESPACE : LABEL ('::' LABEL)* ;

localvariable: '%'LABELNAMESPACE ;
globalvariable: '$'LABELNAMESPACE ;

// Lexer
PLUS: '+' ;
PLUSPLUS: '++' ;
MINUSMINUS: '--' ;
MINUS: '-' ;
MULT: '*' ;
DIV: '/' ;
NOT: '!';
EQUAL: '==' ;
ASSIGN: '=' ;
NOTEQUAL: '!=';
ADDASSIGN: '+=';
MULTASSIGN: '*=';
SUBASSIGN: '-=';
MODULUSASSIGN: '%=' ;
STRINGNOTEQUAL: '!$=' ;
LEFTSHIFTASSIGN: '<<=' ;
RIGHTSHIFTASSIGN: '>>=' ;
BITWISEORASSIGN: '|=' ;
BITWISEANDASSIGN: '&=' ;
EXLUSIVEORASSIGN: '^=' ;
STRINGEQUAL: '$=' ;
LESS: '<' ;
LESSEQ: '<=' ;
BIGGER: '>' ;
BIGGEREQ: '>=' ;
AND: '&&' ;
OR: '||' ;
BITWISEAND: '&';
BITWISEOR: '|';
EXCLUSIVEOR: '^';
LEFTSHIFT: '<<';
RIGHTSHIFT: '>>';
CONCAT: '@' ;
SPACE: 'SPC' ;
NEWLINE: 'NL' ;
TAB: 'TAB' ;

BOOLEAN: TRUE | FALSE ;
TRUE: 'true' ;
FALSE: 'false' ;

MODULUS: '%' ;

// Labels can contain numbers but not at the start
LABEL : [a-zA-Z_]+[a-zA-Z_0-9]* ;

INT :   DIGIT+ ;
DIGIT:  '0'..'9' ;

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

// NOTE: I'm uncertain as to what these \c escapes actually signify aside from what they do in the T2 chat and console
fragment
COLOR_ESCAPE
    :   '\\c' (DIGIT|'p'|'o') ;

EXP :   ('E' | 'e') ('+' | '-')? INT ;
FLOAT:  DIGIT+ '.' DIGIT* EXP? [Ll]?
    |   DIGIT+ EXP? [Ll]?
    |   '.' DIGIT+ EXP? [Ll]?
    ;

// Ignore line comments
LineComment : '//' ~[\r\n]* -> skip ;

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
