#pragma once
//**************************************
// cFuncExprNode.h
//
// Defines AST node for function calls (funcCall)
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbol.h"
#include "cParamListNode.h"
#include "cFuncDeclNode.h"

class cFuncExprNode : public cExprNode
{
    public:
        // Constructor with parameters
        cFuncExprNode(cSymbol *name, cParamListNode *params)
            : cExprNode()
        {
            AddChild(name);
            if (params != nullptr)
                AddChild(params);

            if (name->GetDecl() == nullptr)
            {
                SemanticParseError("Symbol " + name->GetName() +
                    " not defined");
            }
        }

        // Constructor without parameters
        cFuncExprNode(cSymbol *name)
            : cExprNode()
        {
            AddChild(name);

            if (name->GetDecl() == nullptr)
            {
                SemanticParseError("Symbol " + name->GetName() +
                    " not defined");
            }
        }

        cSymbol* GetName() { return static_cast<cSymbol*>(GetChild(0)); }
        cParamListNode* GetParams()
        {
            if (NumChildren() > 1)
                return static_cast<cParamListNode*>(GetChild(1));
            return nullptr;
        }

        virtual cDeclNode* GetType()
        {
            cSymbol *sym = GetName();
            if (sym == nullptr || sym->GetDecl() == nullptr) return nullptr;
            cDeclNode *decl = sym->GetDecl();
            if (decl->IsFunc())
            {
                cFuncDeclNode *fd = static_cast<cFuncDeclNode*>(decl);
                cSymbol *typeSym = fd->GetType();
                if (typeSym == nullptr || typeSym->GetDecl() == nullptr)
                    return nullptr;
                return typeSym->GetDecl();
            }
            return nullptr;
        }

        virtual string NodeType() { return string("funcCall"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
