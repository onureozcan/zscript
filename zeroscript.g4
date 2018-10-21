grammar zeroscript;

expression:
       newObject
       | primaryExpresssion
       | expression bop ='.' expression
       | expression arrayIndexer = '[' expression ']'
       | expression methodCall = '(' expressionList? ')'
       | expression postfix=('++' | '--')
       | prefix=('+'|'-'|'++'|'--') expression
       | prefix=('~'|'!') expression
       | expression bop=('*'|'/'|'%') expression
       | expression bop=('+'|'-') expression
       | expression ('<' '<' | '>' '>' '>' | '>' '>') expression
       | expression bop=('<=' | '>=' | '>' | '<') expression
       | expression bop=('==' | '!=') expression
       | expression bop='and' expression
       | expression bop='or' expression
       | <assoc=right> expression
         bop=('=' | '+=' | '-=' | '*=' | '/=' | '%=')
         expression
       | anonymousFunction
       ;

expressionList
    : expression (',' expression)*
    ;

newObject :
    NEW expression
    ;


primaryExpresssion
    : '(' expression ')'
    | atom
    ;

statement :
    semicolon = ';'
    | body
    | var
    | function
    | expression ';'
    | forLoop
    | whileLoop
    | forInLoop
    | conditional
    | tryCatch
    | throw_ ';'
    | BREAK ';'
    | CONTINUE ';'
    | RET (expression) ';'
    ;

argumentsList:
    |
    identifier (',' identifier)*
    ;

bodyOrStatement:
    body
    | statement
    ;
bodyOrExpression:
    body
    | expression
    ;

body:
    '{' (statement)* '}'
    ;

anonymousFunction:
    FUNC '(' argumentsList ')' body
    | '(' argumentsList ')' '=>' bodyOrExpression
    | identifier '=>' bodyOrExpression
    ;

function:
    PRIVATE? STATIC? ASYNC? FUNC? functionName = identifier '(' argumentsList ')' body
    ;

var:
    PRIVATE? STATIC? VAR variableDeclarationPart  (',' variableDeclarationPart)*
    ;

variableDeclarationPart:
        variableName = identifier ('=' expression)?
    ;

forLoop:
    FOR '(' (expression|var)? ';' expression? ';' expression? ')' bodyOrStatement
    ;

forInLoop:
    FOR '(' VAR iterElement = IDENT? IN expression ')' bodyOrStatement
    ;

whileLoop:
    WHILE '(' expression? ')' bodyOrStatement
;

conditional:
    IF '(' expression ')' bodyOrStatement (ELSE bodyOrStatement)?
;

importStmt:
    IMPORT clsPath = STRING AS clsName = IDENT
    ;

classDeclaration:
    importStmt*
    CLS className = identifier ('(' argumentsList ')')? (EXTENDS extendedClassName = identifier)? body
 ;

tryCatch:
    TRY body CATCH '(' identifier ')' body (FINALLY body)?
    ;

throw_:
    THROW expression
    ;

jsonPair:
    key = expression ':' expression
    ;

jsonObject:
    '{' (jsonPair (',' jsonPair)*)? '}'
    ;
jsonArray:
    '[' (expression (',' expression )*)? ']'
    ;

json:
    jsonObject
    | jsonArray
;

string: STRING;
number: (INT|DECIMAL|FALSE_|TRUE_|NULL_);
identifier: (IDENT);

atom: (string|number|identifier|json);

BlockComment
    :   '/*' .*? '*/'
        -> skip
    ;

LineComment
    :   '//' ~[\r\n]*
        -> skip
    ;

// Whitespace
NEWLINE            : ['\r\n' | '\r' | '\n']+ ->skip ;
WS                 : [\t ]+ -> skip ;

// Keywords
NULL_               : 'null' ;
TRUE_              : 'true' ;
FALSE_              : 'false' ;
EXTENDS            : 'extends' ;
NEW                : 'new' ;
ELSE               : 'else' ;
IF                 : 'if' ;
RET                : 'return' ;
CONTINUE           : 'continue' ;
BREAK              : 'break' ;
WHILE              : 'while' ;
FOR                : 'for' ;
VAR                : 'var' ;
FUNC               : 'function' ;
CLS                : 'class' ;
THROW              : 'throw' ;
TRY                : 'try';
CATCH              : 'catch';
FINALLY            : 'finally';
SWITCH             : 'switch';
IN                 : 'in';
CASE               : 'case';
DEFAULT            : 'default';
STATIC             : 'static';
IMPORT             : 'import';
AS                 : 'as';
ASYNC              : 'async';
PRIVATE            : 'private';

// Literals
INT             : '0'|[1-9][0-9]* ;
DECIMAL         : [0-9][0-9]* '.' [0-9]+ ;
STRING
    : '"' SCharSequence? '"'
    ;

fragment
SCharSequence
    :   SChar+
    ;

fragment
SChar
    :   ~["\\\r\n]
    |   EscapeSequence
    |   '\\\n'   // Added line
    |   '\\\r\n' // Added line
;
fragment
EscapeSequence
    :   '\\' ['"?abfnrtv\\]
;

// Identifiers
IDENT                 : [_]*[A-Za-z0-9_]+ ;