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

// define global variables
cSymbolTable g_symbolTable;
long long cSymbol::nextId;

// Helper function to insert a type symbol into the symbol table
static void InsertType(const char* name)
{
    cSymbol* sym = new cSymbol(name);
    sym->SetIsType(true);
    g_symbolTable.Insert(sym);
}

// takes two string args: input_file, and output_file
int main(int argc, char **argv)
{
    std::cout << "Philip Howard" << std::endl;

    // Insert standard types into the symbol table (in the order specified)
    InsertType("char");
    InsertType("int");
    InsertType("float");
    InsertType("long");
    InsertType("double");

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

    // Setup the output. If empty, use stdout (which may be redirected)
    if (argc > 2)
    {
        outfile_name = argv[2];

        FILE *output = fopen(outfile_name, "w");
        if (output == nullptr)
        {
            std::cerr << "Unable to open output file " << outfile_name << "\n";
            exit(-1);
        }

        // redirect stdout to the output file
        int output_fd = fileno(output);
        if (dup2(output_fd, 1) != 1)
        {
            std::cerr << "Unable configure output stream\n";
            exit(-1);
        }
    }

    result = yyparse();
    if (yyast_root != nullptr)
    {
        if (result == 0)
        {
            cComputeSize sizeVisitor;
            sizeVisitor.VisitAllNodes(yyast_root);

            std::cout << yyast_root->ToString();
        } else {
            std::cout << " Errors in compile\n";
        }
    }

    if (result == 0 && yylex() != 0)
    {
        std::cout << "Junk at end of program\n";
    }

    return result;
}
