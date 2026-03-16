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
        // Constructor for function declaration
        cFuncDeclNode(cSymbol *type, cSymbol *name)
            : cDeclNode(), m_params(nullptr), m_decls(nullptr),
              m_stmts(nullptr), m_prevFunc(nullptr)
        {
            AddChild(type);

            // Handle symbol table insertion and duplicate checking
            cSymbol *localSym = g_symbolTable.FindLocal(name->GetName());
            if (localSym != nullptr)
            {
                if (localSym->GetDecl() != nullptr &&
                    !localSym->GetDecl()->IsType())
                {
                    if (localSym->GetDecl()->IsFunc())
                    {
                        // Previous declaration was also a function
                        m_prevFunc = static_cast<cFuncDeclNode*>(
                            localSym->GetDecl());

                        // Check return type match
                        cSymbol *prevType = m_prevFunc->GetType();
                        if (prevType->GetName() != type->GetName())
                        {
                            SemanticParseError(name->GetName() +
                                " previously declared with different "
                                "return type");
                        }
                    }
                    else
                    {
                        // Previous declaration was not a function
                        SemanticParseError("Symbol " + name->GetName() +
                            " already defined in current scope");
                    }
                }
                AddChild(localSym);
                localSym->SetDecl(this);
            }
            else
            {
                // Check for shadowing from outer scope
                cSymbol *outerSym = g_symbolTable.Find(name->GetName());
                if (outerSym != nullptr)
                {
                    cSymbol *newSym = new cSymbol(name->GetName());
                    g_symbolTable.Insert(newSym);
                    AddChild(newSym);
                    newSym->SetDecl(this);
                }
                else
                {
                    g_symbolTable.Insert(name);
                    AddChild(name);
                    name->SetDecl(this);
                }
            }
        }

        // Set the parameters
        void SetParams(cParamsNode *params)
        {
            m_params = params;
            AddChild(params);

            // Check parameter count matches previous declaration
            if (m_prevFunc != nullptr)
            {
                int prevCount = 0;
                int newCount = 0;
                if (m_prevFunc->GetParams() != nullptr)
                    prevCount = m_prevFunc->GetParams()->NumParams();
                if (params != nullptr)
                    newCount = params->NumParams();

                if (prevCount != newCount)
                {
                    SemanticParseError(GetName()->GetName() +
                        " redeclared with a different number of "
                        "parameters");
                }
            }
        }

        // Set the local declarations
        void SetDecls(cDeclsNode *decls)
        {
            m_decls = decls;
            AddChild(decls);
        }

        // Set the statements (function body)
        void SetStmts(cStmtsNode *stmts)
        {
            m_stmts = stmts;
            AddChild(stmts);

            // Check if previous definition already had a body
            if (m_prevFunc != nullptr && m_prevFunc->HasBody())
            {
                SemanticParseError(GetName()->GetName() +
                    " already has a definition");
            }
        }

        // Copy body from previous declaration (for forward decls after defs)
        void CopyFromPrev()
        {
            if (m_prevFunc != nullptr)
            {
                if (m_stmts == nullptr && m_prevFunc->m_stmts != nullptr)
                {
                    m_stmts = m_prevFunc->m_stmts;
                    AddChild(m_stmts);
                }
                if (m_params == nullptr && m_prevFunc->m_params != nullptr)
                {
                    m_params = m_prevFunc->m_params;
                    AddChild(m_params);
                }
                if (m_decls == nullptr && m_prevFunc->m_decls != nullptr)
                {
                    m_decls = m_prevFunc->m_decls;
                    AddChild(m_decls);
                }
            }
        }

        bool HasBody() { return m_stmts != nullptr; }

        cSymbol* GetType()       { return static_cast<cSymbol*>(GetChild(0)); }
        cSymbol* GetName()       { return static_cast<cSymbol*>(GetChild(1)); }
        cParamsNode* GetParams() { return m_params; }
        cDeclsNode* GetDecls()   { return m_decls; }
        cStmtsNode* GetStmts()   { return m_stmts; }

        virtual bool IsFunc() { return true; }

        virtual string NodeType() { return string("func"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        cParamsNode *m_params;
        cDeclsNode *m_decls;
        cStmtsNode *m_stmts;
        cFuncDeclNode *m_prevFunc;
};
