%{
//**************************************
// lang.y
//
// Parser definition file. bison uses this file to generate the parser.
//
// Author: Phil Howard
//

#include <iostream>
#include "lex.h"
#include "astnodes.h"
#include "cSymbolTable.h"

%}

%locations

 /* union defines the type for lexical values */
%union{
    int             int_val;
    double          float_val;
    std::string*    str_val;
    cAstNode*       ast_node;
    cProgramNode*   program_node;
    cBlockNode*     block_node;
    cStmtsNode*     stmts_node;
    cStmtNode*      stmt_node;
    cDeclsNode*     decls_node;
    cDeclNode*      decl_node;
    cExprNode*      expr_node;
    cIntExprNode*   int_node;
    cSymbol*        symbol;
    }

%{
    int yyerror(const char *msg);

    cAstNode *yyast_root;
%}

%start  program

%token <symbol>    IDENTIFIER
%token <symbol>    TYPE_ID
%token <int_val>   CHAR_VAL
%token <int_val>   INT_VAL
%token <float_val> FLOAT_VAL
%token <int_val>   LE
%token <int_val>   GE
%token <int_val>   AND
%token <int_val>   OR
%token <int_val>   EQUALS
%token <int_val>   NOT_EQUALS
%token <str_val>   STRING_LIT

%token  PROGRAM
%token  PRINT PRINTS
%token  WHILE IF ELSE ENDIF
%token  STRUCT ARRAY
%token  RETURN
%token  CHAR INT LONG FLOAT DOUBLE
%token  JUNK_TOKEN

%type <program_node> program
%type <block_node> block
%type <ast_node> open
%type <ast_node> close
%type <decls_node> decls
%type <decl_node> decl
%type <decl_node> var_decl
%type <ast_node> struct_decl
%type <ast_node> array_decl
%type <decl_node> func_decl
%type <decl_node> func_header
%type <decl_node> func_prefix
%type <expr_node> func_call
%type <ast_node> paramsspec
%type <decl_node> paramspec
%type <stmts_node> stmts
%type <stmt_node> stmt
%type <expr_node> lval
%type <ast_node> params
%type <expr_node> param
%type <expr_node> expr
%type <expr_node> addit
%type <expr_node> term
%type <expr_node> fact
%type <expr_node> varref
%type <symbol> varpart

%%

program: PROGRAM block
                                { $$ = new cProgramNode($2);
                                  yyast_root = $$;
                                  if (yynerrs == 0) 
                                      YYACCEPT;
                                  else
                                      YYABORT;
                                }
block:  open decls stmts close
                                { $$ = new cBlockNode($2, $3); }
    |   open stmts close
                                { $$ = new cBlockNode(nullptr, $2); }

open:   '{'
                                { g_symbolTable.IncreaseScope(); }

close:  '}'
                                { g_symbolTable.DecreaseScope(); }

decls:      decls decl
                                { $$ = $1; $$->Insert($2); }
        |   decl
                                { $$ = new cDeclsNode($1); }
decl:       var_decl ';'
                                { $$ = $1; }
        |   array_decl ';'
                            {  }
        |   struct_decl ';'
                            {  }
        |   func_decl
                            {  }
        |   error ';'
                            {  }

var_decl:   TYPE_ID IDENTIFIER
                                    { $$ = new cVarDeclNode($1, $2); }
struct_decl:  STRUCT open decls close IDENTIFIER
                                { $$ = new cStructDeclNode($3, $5); }
array_decl:   ARRAY TYPE_ID '[' INT_VAL ']' IDENTIFIER
                                { $$ = new cArrayDeclNode($2, $4, $6); }

func_decl:  func_header ';'
                                { $$ = $1; g_symbolTable.DecreaseScope(); }
        |   func_header  '{' decls stmts '}'
                                { $$ = $1; static_cast<cFuncDeclNode*>($$)->SetDecls($3); static_cast<cFuncDeclNode*>($$)->SetStmts($4); g_symbolTable.DecreaseScope(); }
        |   func_header  '{' stmts '}'
                                { $$ = $1; static_cast<cFuncDeclNode*>($$)->SetStmts($3); g_symbolTable.DecreaseScope(); }
func_header: func_prefix paramsspec ')'
                                { $$ = $1; static_cast<cFuncDeclNode*>($$)->SetParams(static_cast<cParamsNode*>($2)); }
        |    func_prefix ')'
                            { $$ = $1; }
func_prefix: TYPE_ID IDENTIFIER '('
                                { $$ = new cFuncDeclNode($1, $2); g_symbolTable.IncreaseScope(); }
paramsspec:  paramsspec ',' paramspec
                                { $$ = $1; static_cast<cParamsNode*>($$)->Insert($3); }
        |   paramspec
                            { $$ = new cParamsNode($1); }

paramspec:  var_decl
                                    { $$ = $1; }

stmts:      stmts stmt
                                { $$ = $1; $$->Insert($2); }
        |   stmt
                            { $$ = new cStmtsNode($1); }

stmt:       IF '(' expr ')' stmts ENDIF ';'
                                { $$ = new cIfNode($3, $5); }
        |   IF '(' expr ')' stmts ELSE stmts ENDIF ';'
                                { $$ = new cIfNode($3, $5, $7); }
        |   WHILE '(' expr ')' stmt
                                { $$ = new cWhileNode($3, $5); }
        |   PRINT '(' expr ')' ';'
                                { $$ = new cPrintNode($3); }
        |   PRINTS '(' STRING_LIT ')' ';'
                                { $$ = new cPrintsNode($3); }
        |   lval '=' expr ';'
                            { $$ = new cAssignNode($1, $3); }
        |   func_call ';'
                            { $$ = $1; }
        |   block
                            { $$ = $1; }
        |   RETURN expr ';'
                            { $$ = new cReturnNode($2); }
        |   error ';'
                            {}

func_call:  IDENTIFIER '(' params ')'
                                    { $$ = new cFuncExprNode($1, static_cast<cParamListNode*>($3)); }
        |   IDENTIFIER '(' ')'
                            { $$ = new cFuncExprNode($1); }

varref:   varref '.' varpart
                                { $$ = $1; static_cast<cVarExprNode*>($$)->AddSymbol($3); }
        | varref '[' expr ']'
                            { $$ = $1; static_cast<cVarExprNode*>($$)->AddIndex($3); }
        | varpart
                            { $$ = new cVarExprNode($1); }

varpart:  IDENTIFIER
                                { $$ = $1; }

lval:     varref
                                { $$ = $1; }

params:   params ',' param
                                { $$ = $1; static_cast<cParamListNode*>($$)->Insert($3); }
        |   param
                            { $$ = new cParamListNode($1); }

param:      expr
                                { $$ = $1; }

expr:       expr EQUALS addit
                                { $$ = new cBinaryExprNode($1, new cOpNode(EQUALS), $3); }
        |   addit
                            { $$ = $1; }

addit:      addit '+' term
                                { $$ = new cBinaryExprNode($1, new cOpNode('+'), $3); }
        |   addit '-' term
                            { $$ = new cBinaryExprNode($1, new cOpNode('-'), $3); }
        |   term
                            { $$ = $1; }

term:       term '*' fact
                                { $$ = new cBinaryExprNode($1, new cOpNode('*'), $3); }
        |   term '/' fact
                            { $$ = new cBinaryExprNode($1, new cOpNode('/'), $3); }
        |   term '%' fact
                            { $$ = new cBinaryExprNode($1, new cOpNode('%'), $3); }
        |   fact
                            { $$ = $1; }

fact:       '(' expr ')'
                                { $$ = $2; }
        |   INT_VAL
                            { $$ = new cIntExprNode($1); }
        |   FLOAT_VAL
                            { $$ = new cFloatExprNode($1); }
        |   varref
                            { $$ = $1; }
        |   func_call
                            { $$ = $1; }

%%

// Function to format error messages
int yyerror(const char *msg)
{
    std::cerr << "ERROR: " << msg << " at symbol "
        << yytext << " on line " << yylineno << "\n";

    return 0;
}
