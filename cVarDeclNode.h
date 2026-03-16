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
                // Check for duplicate definition
                if (localSym->GetDecl() != nullptr &&
                    !localSym->GetDecl()->IsType())
                {
                    SemanticParseError("Symbol " + localSym->GetName() +
                        " already defined in current scope");
                    AddChild(localSym);
                }
                else
                {
                    AddChild(localSym);
                    localSym->SetDecl(this);
                }
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
                    newSym->SetDecl(this);
                }
                else
                {
                    // New name, insert it and use it
                    g_symbolTable.Insert(name);
                    AddChild(name);
                    name->SetDecl(this);
                }
            }
        }

        cSymbol* GetType()  { return static_cast<cSymbol*>(GetChild(0)); }
        cSymbol* GetName()  { return static_cast<cSymbol*>(GetChild(1)); }

        virtual bool IsVar() { return true; }

        virtual string NodeType() { return string("var_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
