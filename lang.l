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
#include "langparse.h"

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
"char"                      DO_RETURN(ProcessID());
"int"                       DO_RETURN(ProcessID());
"long"                      DO_RETURN(ProcessID());
"float"                     DO_RETURN(ProcessID());
"double"                    DO_RETURN(ProcessID());
"struct"                    DO_RETURN(STRUCT);
"array"                     DO_RETURN(ARRAY);
"return"                    DO_RETURN(RETURN);

{FLOAT_CONST}               {yylval.float_val = atof(yytext); DO_RETURN(FLOAT_VAL);}
{INT_CONST}                 {yylval.int_val = atoi(yytext); DO_RETURN(INT_VAL);}
{STRING}                    {yylval.str_val = new std::string(yytext); DO_RETURN(STRING_LIT);}

{IDENT}                     DO_RETURN(ProcessID());

"("                         DO_RETURN('(');
")"                         DO_RETURN(')');
"{"                         DO_RETURN('{');
"}"                         DO_RETURN('}');
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

    // First check if the symbol exists in any scope
    sym = g_symbolTable.Find(name);
    if (sym != nullptr)
    {
        yylval.symbol = sym;
        // Check if it's a type - return TYPE_ID if so
        if (sym->IsType())
            return TYPE_ID;
        return IDENTIFIER;
    }

    // Symbol doesn't exist - create new one in local scope
    sym = new cSymbol(name);
    g_symbolTable.Insert(sym);
    yylval.symbol = sym;

    return IDENTIFIER;
}
