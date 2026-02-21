#pragma once
//**************************************
// cFuncDeclNode.h
//
// Defines AST node for function declarations
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"
#include "cParamsNode.h"
#include "cDeclsNode.h"
#include "cStmtsNode.h"
#include "cSymbolTable.h"

class cFuncDeclNode : public cDeclNode
{
    public:
        // Constructor for function declaration (forward declaration)
        cFuncDeclNode(cSymbol *type, cSymbol *name)
            : cDeclNode()
        {
            AddChild(type);
            AddChild(HandleSymbol(name));
            m_params = nullptr;
            m_decls = nullptr;
            m_stmts = nullptr;
        }

        // Set the parameters
        void SetParams(cParamsNode *params)
        {
            m_params = params;
            AddChild(params);
        }

        // Set the local declarations
        void SetDecls(cDeclsNode *decls)
        {
            m_decls = decls;
            AddChild(decls);
        }

        // Set the statements
        void SetStmts(cStmtsNode *stmts)
        {
            m_stmts = stmts;
            AddChild(stmts);
        }

        cSymbol* GetType()       { return static_cast<cSymbol*>(GetChild(0)); }
        cSymbol* GetName()       { return static_cast<cSymbol*>(GetChild(1)); }
        cParamsNode* GetParams() { return m_params; }
        cDeclsNode* GetDecls()   { return m_decls; }
        cStmtsNode* GetStmts()   { return m_stmts; }

        virtual string NodeType() { return string("func"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        cSymbol* HandleSymbol(cSymbol *name)
        {
            // Check if name exists locally
            cSymbol *localSym = g_symbolTable.FindLocal(name->GetName());
            if (localSym != nullptr)
            {
                return localSym;
            }
            else
            {
                // Check for shadowing
                cSymbol *outerSym = g_symbolTable.Find(name->GetName());
                if (outerSym != nullptr)
                {
                    // Create new symbol to shadow
                    cSymbol *newSym = new cSymbol(name->GetName());
                    g_symbolTable.Insert(newSym);
                    return newSym;
                }
                else
                {
                    // New name, insert it
                    g_symbolTable.Insert(name);
                    return name;
                }
            }
        }

        cParamsNode *m_params;
        cDeclsNode *m_decls;
        cStmtsNode *m_stmts;
};
