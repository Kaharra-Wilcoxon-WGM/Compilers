#pragma once
//**************************************
// cVarDeclNode.h
//
// Defines AST node for variable declarations
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cVarDeclNode : public cDeclNode
{
    public:
        // params are the type symbol and name symbol
        cVarDeclNode(cSymbol *type, cSymbol *name)
            : cDeclNode()
        {
            AddChild(type);

            // Check if name already exists in local scope
            cSymbol *localSym = g_symbolTable.FindLocal(name->GetName());
            if (localSym != nullptr)
            {
                // Name exists in local scope - use the local symbol
                AddChild(localSym);
            }
            else
            {
                // Name doesn't exist locally - check if it exists in outer scope
                cSymbol *outerSym = g_symbolTable.Find(name->GetName());
                if (outerSym != nullptr)
                {
                    // Name exists in outer scope - create a new local symbol to shadow it
                    cSymbol *newSym = new cSymbol(name->GetName());
                    g_symbolTable.Insert(newSym);
                    AddChild(newSym);
                }
                else
                {
                    // New name, insert it and use it
                    g_symbolTable.Insert(name);
                    AddChild(name);
                }
            }
        }

        virtual string NodeType() { return string("var_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
