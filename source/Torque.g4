/*
    MIT License
*/

grammar Torque;

program  : (outerblock | statement)+ | <EOF> ;

elseifcontrol : (ELSE IF '(' controlexpression ')' '{' statement* '}')
              | (ELSE IF '(' controlexpression ')' statement ) ;

elsecontrol : ELSE '{' statement* '}'
            | ELSE statement ;

ifcontrol : IF '(' controlexpression ')' statement elseifcontrol* elsecontrol?
          | IF '(' controlexpression ')' '{' statement* '}' elseifcontrol* elsecontrol? ;

whilecontrol : WHILE '(' controlexpression ')' '{' statement* '}'
             | WHILE '(' controlexpression ')' statement ;

// In Torque, all for components are required
forcontrol : FOR '(' controlexpression SEMICOLON controlexpression SEMICOLON controlexpression ')' statement
           | FOR '(' controlexpression SEMICOLON controlexpression SEMICOLON controlexpression ')' '{' statement* '}' ;

// In Torque, the case values are apparently expressions
switchcase : CASE expression switchcasealternative* COLON statement+ ;
switchcasealternative : (OR expression) ;

// Switch has two forms - switch and switch$ for string values
switchcontrol : SWITCH DOLLARSIGN? '(' expression ')' '{' switchcase* ('default' COLON statement+)? '}' ;

// Control structures
control : whilecontrol
        | ifcontrol
        | forcontrol
        | switchcontrol ;

// Object instantiation - we support two forms new TYPENAME(NAME) and new (TYPENAMEEXP)(NAME) both with optional initialization lists
objectinitialization : '{' (field | newobject SEMICOLON)* '}' ;
newobject : NEW LABEL '(' expression? ')' objectinitialization?
          | NEW '(' expression ')' '(' expression? ')' objectinitialization? ;

// Functions, datablocks, packages
paramlist : LOCALVARIABLE (',' LOCALVARIABLE )* ;
functiondeclaration : FUNCTION labelsingle '(' paramlist? ')' '{' statement* '}' ;
packagedeclaration : PACKAGE labelsingle '{' functiondeclaration* '}' SEMICOLON ;

// Datablock declaration requires at least one field
datablockdeclaration : DATABLOCK LABEL '(' LABEL ')' (COLON LABEL)? '{' field+ '}' SEMICOLON ;

outerblock : functiondeclaration
           | packagedeclaration
           | datablockdeclaration ;

actionstatement : expression SEMICOLON ;

statement : control
          | returncontrol SEMICOLON
          | breakcontrol SEMICOLON
          | actionstatement ;

// Used for setting field values in object instantiation & datablocks
field : LABEL ('[' expression ']')? '=' expression SEMICOLON ;

returncontrol : RETURN expression? ;
breakcontrol : BREAK ;

controlexpression : expression ;
expression : (op=NOT|op=MINUS) expression                                                        # unary
           | expression (op=PLUSPLUS|op=MINUSMINUS)                                              # unary
           | labelsingle '(' expression? (',' expression)* ')'                                   # call
           | (GLOBALVARIABLE | LOCALVARIABLE) '[' expression (',' expression)* ']'                                     # array
           | expression ('.' label)                                                              # subreference
           | '(' expression ')'                                                                  # parenthesis
           | expression (op=MULT|op=DIV|op=MODULUS) expression                                   # arithmetic
           | expression (op=PLUS|op=MINUS) expression                                            # arithmetic
           | expression (op=LEFTSHIFT|op=RIGHTSHIFT) expression                                  # arithmetic
           | expression (op=LESS|op=LESSEQ|op=BIGGER|op=BIGGEREQ) expression                     # relational
           | expression (op=EQUAL|op=NOTEQUAL|op=STRINGEQUAL|op=STRINGNOTEQUAL) expression       # equality
           | expression (op=BITWISEAND|op=EXCLUSIVEOR|op=BITWISEOR) expression                   # bitwise
           | expression (op=CONCAT|op=SPACE|op=NEWLINE|op=TAB) expression                        # concatenation
           | expression (op=LOGICALAND|op=LOGICALOR) expression                                                # logicalop
           | expression QUESTIONMARK controlexpression COLON controlexpression                              # ternary
           | expression (op=ASSIGN
                        |op=ADDASSIGN
                        |op=MULTASSIGN
                        |op=SUBASSIGN
                        |op=MODULUSASSIGN
                        |op=BITWISEORASSIGN
                        |op=BITWISEANDASSIGN
                        |op=EXLUSIVEORASSIGN
                        |op=LEFTSHIFTASSIGN
                        |op=RIGHTSHIFTASSIGN)  expression                                        # assignment
           | op=newobject                                                                        # objectInstantiation
           | op=INT                                                                              # value
           | op=FLOAT                                                                            # value
           | op=GLOBALVARIABLE                                                                   # value
           | op=LOCALVARIABLE                                                                    # value
           | op=LABEL                                                                            # value
           | op=TRUE                                                                             # value
           | op=FALSE                                                                            # value
           | op=STRING                                                                           # value ;

labelsingle : LABEL (sublabel)?;
label : LABEL (sublabel)*;
sublabel : '::' LABEL ;

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
LOGICALAND: '&&' ;
LOGICALOR: '||' ;
BITWISEAND: '&';
BITWISEOR: '|';
EXCLUSIVEOR: '^';
LEFTSHIFT: '<<';
RIGHTSHIFT: '>>';
CONCAT: '@' ;
SPACE: 'SPC' ;
NEWLINE: 'NL' ;
TAB: 'TAB' ;
NEW: 'new' ;
FUNCTION : 'function';
PACKAGE : 'package' ;
DATABLOCK : 'datablock' ;
RETURN : 'return' ;
BREAK : 'break' ;
FOR : 'for' ;
WHILE : 'while' ;
SEMICOLON : ';' ;
COLON : ':' ;
IF : 'if' ;
ELSE : 'else' ;
TRUE : 'true' ;
FALSE : 'false' ;
OR : 'or' ;
CASE : 'case' ;
MODULUS : '%' ;
SWITCH : 'switch' ;
DOLLARSIGN : '$' ;
LOCALVARIABLE: '%'LABEL ;
GLOBALVARIABLE: '$'LABEL ;
QUESTIONMARK : '?' ;

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
