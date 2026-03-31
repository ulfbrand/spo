grammar Lab1Grammar;

options
{
    ASTLabelType=pANTLR3_BASE_TREE;
    language=C;
    output=AST;
    backtrack=true;
}

tokens
{
    FUNC_DEF  ;
    ROOT_DEF  ;
    EMPTY_ROOT_DEF  ;
    FUNC_SIG  ;
    UNTYPE_FUNC_SIG ;
    ARG_ITEM  ;
    IN_ARR_DEF ;
    ARG_DEF   ;
    VAR_DEF   ;
    IF_DEF    ;
    ELSE_DEF  ;
    WHILE_DEF ;
    COND_DEF  ;
    DO_DEF    ;
    DO_STH_DEF ;
    UN_EXPR_DEF ;
    CALL_DEF ;
    INDEXER_DEF ;
    SOURCE  ;
    WORD_TOKEN  ;
    WORDNUM_TOKEN  ;
    DIGIT   ;
    IND ;
    ACCESS;
    CALL;
    ARG_CALL;
    POSTFIX;
    NAME;
    BLOCK_DEF;
    ASSIGNMENT;
    TYPE;
    DECLARE;
    EXPRESSION;
}

@header
{
   #define _empty NULL
}

bool_l  : BOOL
        ;
str     : STR
        ;
char_l  : CHAR
        ;
hex     : HEX
        ;
bits    : BITS
        ;
dec     : DEC
        ;
plus    : PLUS
        ;
minus   : MINUS
        ;
mult    : MULT
        ;
divOp   : DIV
        ;
and     : AND
        ;
or      : OR
        ;
shift_left  : SHIFT_LEFT
            ;
shift_right : SHIFT_RIGHT
            ;
less    : LESS
        ;
less_or_eq  : LESS_OR_EQ
            ;
more    : MORE
        ;
more_or_eq  : MORE_OR_EQ
        ;
mod     : MOD
        ;
not_equal   : NOT_EQUAL
            ;
equal   : EQUAL
        ;
assign  : ASSIGN
        ;
plus_assign : PLUS_ASSIGN
            ;
minus_assign: MINUS_ASSIGN
            ;
mult_assign : MULT_ASSIGN
            ;
div_assign  : DIV_ASSIGN
            ;
mod_assign  : MOD_ASSIGN
            ;
not         : NOT
            ;



source      : sourceItem* -> ^(SOURCE sourceItem*);

builtin     : boolK
            | byteK
            | intK
            | uintK
            | longK
            | ulongK
            | charK
            | stringK
            ;

boolK       : BOOL_KEYWORD;
byteK       : BYTE_KEYWORD;
intK        : INT_KEYWORD;
uintK       : UINT_KEYWORD;
longK       : LONG_KEYWORD;
ulongK      : ULONG_KEYWORD;
charK       : CHAR_KEYWORD;
stringK     : STRING_KEYWORD;

arrayType       : builtinOrIdentifier array+
                ;

array           : LEFT_SQBRACE COMMA* RIGHT_SQBRACE -> ^(IN_ARR_DEF COMMA*)
                ;

builtinOrIdentifier : builtin
                    | ID
                    ;

typeRef     : builtin -> ^(TYPE builtin)
            | ID -> ^(TYPE ID)
            | arrayType -> ^(TYPE arrayType)
            ;

argItem     : typeRef ID -> ^(ARG_ITEM typeRef ID)
            | ID -> ^(ARG_ITEM ID)
            ;

argDef      : (argItem (COMMA argItem)*)? -> ^(ARG_DEF argItem*)
            ;

funcSignature   : typeRef ID LEFT_BRACE argDef RIGHT_BRACE -> ^(FUNC_SIG typeRef ID argDef)
                | ID LEFT_BRACE argDef RIGHT_BRACE -> ^(UNTYPE_FUNC_SIG ID argDef)
                ;

sourceItem  : funcDef
            ;

funcDef     : funcSignature ( block_statement )  -> ^(FUNC_DEF funcSignature ^(ROOT_DEF block_statement))
            | funcSignature ( SEMICOLON )  -> ^(FUNC_DEF funcSignature ^(EMPTY_ROOT_DEF))
            ;

statement   : var_statement
            | if_statement
            | block_statement
            | while_statement
            | do_statement
            | break_statement
            | expr_statement
            ;

identifier : ID (ASSIGN expr)? -> ^(DECLARE ID ^(EXPRESSION ^(ASSIGNMENT ASSIGN ^(NAME ID) expr))?)
               ;

var_statement : typeRef identifier (COMMA identifier)* SEMICOLON -> ^(VAR_DEF typeRef identifier+)
              ;

else_statement   : ELSE_KEYWORD statement -> ^(ELSE_DEF statement)
              ;

if_statement  : IF_KEYWORD LEFT_BRACE expr RIGHT_BRACE statement (else_statement)?
                -> ^(IF_DEF ^(COND_DEF expr) statement else_statement)
              ;

block_statement : LEFT_FBRACE statement* RIGHT_FBRACE
                   -> ^(BLOCK_DEF statement*)
               ;

while_statement : WHILE_KEYWORD LEFT_BRACE expr RIGHT_BRACE statement
                   -> ^(WHILE_DEF ^(COND_DEF expr) statement)
                ;

do_statement    : DO_KEYWORD block_statement WHILE_KEYWORD LEFT_BRACE expr RIGHT_BRACE SEMICOLON
                   -> ^(DO_DEF ^(DO_STH_DEF block_statement) ^(COND_DEF expr))
                ;

break_statement : BREAK_KEYWORD SEMICOLON -> BREAK_KEYWORD
                ;

expr_statement  : expr SEMICOLON -> expr
                ;


                

expr            : assignmentExpr -> ^(EXPRESSION assignmentExpr)
                ;

assignmentExpr  : condExpr assignment assignmentExpr -> ^(ASSIGNMENT assignment condExpr assignmentExpr)
                | condExpr
                ;

condExpr        : orExpr
                ;

orExpr          : andExpr (or^ andExpr)*
                ;

andExpr         : eqExpr (and^ eqExpr)*
                ;

eqExpr          : relExpr (equality^ relExpr)*
                ;

relExpr         : shiftExpr (relational^ shiftExpr)*
                ;

shiftExpr       : additiveExpr (shift^ additiveExpr)*
                ;

additiveExpr    : multExpr (additive^ multExpr)*
                ;

multExpr        : unaryExpr (multiplicative^ unaryExpr)*
                ;

unaryExpr       : unOp? notrecursiveExpr
                ;


notrecursiveExpr: simpleExpr postfixOp+ -> ^(ACCESS simpleExpr postfixOp+)
                | simpleExpr 
                ;

postfixOp       : indexer | call
                ;


indexer
    : LEFT_SQBRACE (exprList)? RIGHT_SQBRACE -> ^(INDEXER_DEF (exprList)?)
    ;

call
    : LEFT_BRACE (exprList)? RIGHT_BRACE -> ^(ARG_CALL (exprList)?)
    ;

exprList
    : expr (COMMA expr)* -> (expr)+
    ;

simpleExpr
    : literal | place | braces
    ;

braces
    : LEFT_BRACE expr RIGHT_BRACE -> expr
    ;

place
    : ID -> ^(NAME ID)
    ;

literal
    : BOOL | STR | CHAR | HEX | BITS | DEC
    ;



assignment      : assign | plus_assign | minus_assign | mult_assign | div_assign | mod_assign
                ;

equality        : not_equal | equal
                ;

relational      : less | less_or_eq | more | more_or_eq
                ;

shift           : shift_left | shift_right
                ;

additive        : plus | minus 
                ;

multiplicative  : mult | divOp | mod
                ;

unOp            : not | minus
                ;

PLUS:    '+' ;
MINUS:   '-' ;
MULT:    '*' ;
DIV:     '/' ;
AND:     '&' ;
OR:      '|' ;
SHIFT_LEFT: '<<' ;
SHIFT_RIGHT: '>>' ;
LESS:    '<' ;
LESS_OR_EQ: '<=';
MORE:    '>' ;
MORE_OR_EQ: '>=';
MOD:     '%' ;
EQUAL:   '==' ;
ASSIGN:  '=' ;
NOT:     '!' ;
NOT_EQUAL: '!=' ;
PLUS_ASSIGN: '+=';
MINUS_ASSIGN: '-=';
MULT_ASSIGN: '*=';
DIV_ASSIGN: '/=';
MOD_ASSIGN: '%=';
COMMA:   ',' ;


LEFT_BRACE: '(';
RIGHT_BRACE: ')';
LEFT_SQBRACE: '[';
RIGHT_SQBRACE: ']';
LEFT_FBRACE: '{';
RIGHT_FBRACE: '}';
SEMICOLON: ';';

IF_KEYWORD: 'if';
ELSE_KEYWORD: 'else';
WHILE_KEYWORD: 'while';
DO_KEYWORD: 'do';
BREAK_KEYWORD: 'break';

BOOL_KEYWORD: 'bool';
BYTE_KEYWORD: 'byte';
INT_KEYWORD: 'int';
UINT_KEYWORD: 'uint';
LONG_KEYWORD: 'long';
ULONG_KEYWORD: 'ulong';
CHAR_KEYWORD: 'char';
STRING_KEYWORD: 'string';

BOOL        : ('true' | 'false')
            ;

ID          : WORD_TOKEN (WORDNUM_TOKEN)*
            ;

STR         : '\"' (ESC_SEQ | ~('\"' | '\\'))* '\"'
            ;

CHAR        : '\'' ~('\'') '\''
            ;

HEX         : '0' ('x' | 'X') (HEX_TOKEN)+
            ;

BITS        : '0' ('b' | 'B') ('0' | '1')+
            ;

DEC         : (DIGIT)+
            ;

WHITESPACE  : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+
              {
                 $channel = HIDDEN;
              }
            ;

fragment
ESC_SEQ : '\\' ( '"' | '\\' | 'n' | 'r' | 't' )
        ;

fragment
DIGIT       : '0'..'9'
            ;

fragment
WORDNUM_TOKEN  : WORD_TOKEN
                | DIGIT
                ;

fragment
HEX_TOKEN       : '0' ('x' | 'X') ('a'..'f' | 'A'..'F' | DIGIT)+
                ;

fragment
WORD_TOKEN  : 'a'..'z'
            | 'A' .. 'Z'
            | '_'
            ;
