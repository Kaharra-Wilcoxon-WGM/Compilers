#pragma once
//**************************************
// cStructDeclNode.h
//
// Defines AST node for struct declarations
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cDeclsNode.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cStructDeclNode : public cDeclNode
{
    public:
        // params are the declarations inside the struct and the name
        cStructDeclNode(cDeclsNode *decls, cSymbol *name)
            : cDeclNode()
        {
            AddChild(decls);

            // Check if name exists locally
            cSymbol *localSym = g_symbolTable.FindLocal(name->GetName());
            if (localSym != nullptr)
            {
                // Use local symbol and mark as type
                localSym->SetIsType(true);
                AddChild(localSym);
            }
            else
            {
                // Check for shadowing
                cSymbol *outerSym = g_symbolTable.Find(name->GetName());
                if (outerSym != nullptr)
                {
                    // Create new symbol to shadow
                    cSymbol *newSym = new cSymbol(name->GetName());
                    newSym->SetIsType(true);
                    g_symbolTable.Insert(newSym);
                    AddChild(newSym);
                }
                else
                {
                    // New name, insert and mark as type
                    name->SetIsType(true);
                    g_symbolTable.Insert(name);
                    AddChild(name);
                }
            }
        }

        cDeclsNode* GetDecls() { return static_cast<cDeclsNode*>(GetChild(0)); }
        cSymbol* GetName()     { return static_cast<cSymbol*>(GetChild(1)); }

        virtual string NodeType() { return string("struct_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
