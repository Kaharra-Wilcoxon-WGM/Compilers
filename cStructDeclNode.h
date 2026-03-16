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
                    localSym->SetIsType(true);
                    AddChild(localSym);
                    localSym->SetDecl(this);
                }
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
                    newSym->SetDecl(this);
                }
                else
                {
                    // New name, insert and mark as type
                    name->SetIsType(true);
                    g_symbolTable.Insert(name);
                    AddChild(name);
                    name->SetDecl(this);
                }
            }
        }

        cDeclsNode* GetDecls() { return static_cast<cDeclsNode*>(GetChild(0)); }
        cSymbol* GetName()     { return static_cast<cSymbol*>(GetChild(1)); }

        virtual bool IsStruct() { return true; }
        virtual bool IsType()   { return true; }

        virtual string NodeType() { return string("struct_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
