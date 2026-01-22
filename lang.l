%{
//**************************************
// lang.l
//
// scanner definition file. flex uses this file to create the scanner
//
// Author: Kaharra Wilcoxon
//

#include "cSymbol.h"
#include "cSymbolTable.h"
#include "lex.h"
#include "tokens.h"

//#define DEBUG_OUTPUT
#ifdef DEBUG_OUTPUT
    #define DO_RETURN(a) { return Return(a); }
#else
    #define DO_RETURN(a) { return (a); }
#endif

int Return(int val);
static int ProcessID();

%}

%option noyywrap
%option noinput
%option nounput
%option yylineno

DIGIT           [0-9]
LETTER          [a-zA-Z]
IDENT           ({LETTER}|_)({LETTER}|{DIGIT}|_)*
INT_CONST       {DIGIT}+
FLOAT_CONST     {DIGIT}+\.{DIGIT}+
STRING          \"[^"]*\"
WHITESPACE      [ \t\n\r]+
COMMENT         \/\/.*

%%

{WHITESPACE}                {}
{COMMENT}                   {}

"!="                        DO_RETURN(NOT_EQUALS);
"=="                        DO_RETURN(EQUALS);
"&&"                        DO_RETURN(AND);
"||"                        DO_RETURN(OR);
">="                        DO_RETURN(GE);
"<="                        DO_RETURN(LE);

"program"                   DO_RETURN(PROGRAM);
"if"                        DO_RETURN(IF);
"else"                      DO_RETURN(ELSE);
"endif"                     DO_RETURN(ENDIF);
"while"                     DO_RETURN(WHILE);
"print"                     DO_RETURN(PRINT);
"prints"                    DO_RETURN(PRINTS);
"char"                      DO_RETURN(CHAR);
"int"                       DO_RETURN(INT);
"long"                      DO_RETURN(LONG);
"float"                     DO_RETURN(FLOAT);
"double"                    DO_RETURN(DOUBLE);
"struct"                    DO_RETURN(STRUCT);
"array"                     DO_RETURN(ARRAY);
"return"                    DO_RETURN(RETURN);

{FLOAT_CONST}               DO_RETURN(FLOAT_VAL);
{INT_CONST}                 DO_RETURN(INT_VAL);
{STRING}                    DO_RETURN(STRING_LIT);

{IDENT}                     DO_RETURN(ProcessID());

"("                         DO_RETURN('(');
")"                         DO_RETURN(')');
"{"                         { g_symbolTable.IncreaseScope(); DO_RETURN('{'); }
"}"                         { g_symbolTable.DecreaseScope(); DO_RETURN('}'); }
"["                         DO_RETURN('[');
"]"                         DO_RETURN(']');
";"                         DO_RETURN(';');
","                         DO_RETURN(',');
"."                         DO_RETURN('.');
"+"                         DO_RETURN('+');
"-"                         DO_RETURN('-');
"*"                         DO_RETURN('*');
"/"                         DO_RETURN('/');
"%"                         DO_RETURN('%');
"="                         DO_RETURN('=');
">"                         DO_RETURN('>');
"<"                         DO_RETURN('<');

.                           DO_RETURN(JUNK_TOKEN);

%%

int Return(int val)
{
    printf("Scanned '%s': %d\n", yytext, val);
    return val;
}

static int ProcessID()
{
    string name(yytext);
    cSymbol *sym;

    sym = g_symbolTable.FindLocal(name);
    if (sym == nullptr)
    {
        sym = new cSymbol(name);
        g_symbolTable.Insert(sym);
    }
    yylval.symbol = sym;

    return IDENTIFIER;
}
