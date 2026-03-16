//**************************************
// main.cpp
//
// Main function for lang compiler
//
// Author: Phil Howard
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "cSymbolTable.h"
#include "lex.h"
#include "astnodes.h"
#include "langparse.h"
#include "emit.h"
#include "cBaseTypeNode.h"
#include "cSemantics.h"

// Define SemanticParseError (declared in cAstNode.h)
void SemanticParseError(std::string error)
{
    std::cout << "ERROR: " << error << " near line " << yylineno << "\n";
    yynerrs++;
}

// Define which lab features to enable
// Uncomment the appropriate line for the lab being graded
//#define LAB5B
//#define LAB6
#define LAB7

// define global variables
cSymbolTable g_symbolTable;
long long cSymbol::nextId;

// Helper function to insert a type symbol into the symbol table
static void InsertType(const char* name, int size, bool isFloat)
{
    cSymbol* sym = new cSymbol(name);
    sym->SetIsType(true);
    g_symbolTable.Insert(sym);
    cBaseTypeNode *node = new cBaseTypeNode(name, size, isFloat);
    sym->SetDecl(node);
}

// takes two string args: input_file, and output_file
int main(int argc, char **argv)
{
    std::cout << "Philip Howard" << std::endl;

    // Insert standard types into the symbol table (in the order specified)
    InsertType("char", 1, false);
    InsertType("int", 4, false);
    InsertType("float", 8, true);
    InsertType("long", 8, false);
    InsertType("double", 8, true);

    const char *outfile_name;
    int result = 0;

    if (argc > 1)
    {
        yyin = fopen(argv[1], "r");
        if (yyin == nullptr)
        {
            std::cerr << "ERROR: Unable to open file " << argv[1] << "\n";
            exit(-1);
        }
    }

    if (argc > 2)
    {
        outfile_name = argv[2];
    } else {
        outfile_name = "/dev/tty";
    }

#ifndef LAB7
    // For Labs 4/5/6: redirect stdout to output file for XML output
    FILE *output = fopen(outfile_name, "w");
    if (output == nullptr)
    {
        std::cerr << "Unable to open output file " << outfile_name << "\n";
        exit(-1);
    }

    int output_fd = fileno(output);
    if (dup2(output_fd, 1) != 1)
    {
        std::cerr << "Unable to configure output stream\n";
        exit(-1);
    }
#endif

    result = yyparse();
    if (yyast_root != nullptr && result == 0)
    {
#ifdef LAB5B
        cSemantics semantics;
        semantics.VisitAllNodes(yyast_root);
#endif

        result += yynerrs;
        if (result == 0)
        {
#if defined(LAB6) || defined(LAB7)
            cComputeSize sizeVisitor;
            sizeVisitor.VisitAllNodes(yyast_root);
#endif

#ifdef LAB7
            // Generate stackl assembly output
            if (argc > 2)
            {
                string outputFile = string(argv[2]) + ".sl";
                InitOutput(outputFile);

                cCodeGen codeGen;
                codeGen.VisitAllNodes(yyast_root);

                FinalizeOutput();
            }
#else
            // Output XML representation for Labs 4/5/6
            std::cout << yyast_root->ToString() << std::endl;
#endif
        }
    }

    if (yynerrs != 0)
    {
        std::cout << yynerrs << " Errors in compile\n";
    }

    if (result == 0 && yylex() != 0)
    {
        std::cerr << "Junk at end of program\n";
    }

    return result;
}
