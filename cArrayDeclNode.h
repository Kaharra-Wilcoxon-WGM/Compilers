#pragma once
//**************************************
// cArrayDeclNode.h
//
// Defines AST node for array declarations
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cArrayDeclNode : public cDeclNode
{
    public:
        // params are base type, count, and name
        cArrayDeclNode(cSymbol *type, int count, cSymbol *name)
            : cDeclNode(), m_count(count)
        {
            AddChild(type);

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

        cSymbol* GetType()  { return static_cast<cSymbol*>(GetChild(0)); }
        cSymbol* GetName()  { return static_cast<cSymbol*>(GetChild(1)); }
        int GetCount()      { return m_count; }

        virtual string AttributesToString()
        {
            return " count=\"" + std::to_string(m_count) + "\"";
        }

        virtual string NodeType() { return string("array_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        int m_count;
};
