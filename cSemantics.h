#pragma once
//**************************************
// cSemantics.h
//
// Semantic checking visitor
//
// Author: Kaharra Wilcoxon
//

#include "cVisitor.h"
#include "astnodes.h"

class cSemantics : public cVisitor
{
    public:
        cSemantics() : cVisitor() {}

        virtual void VisitAllNodes(cAstNode *node) { node->Visit(this); }

        // Use default visit (visit all children) for all node types
        using cVisitor::Visit;

        virtual void Visit(cAssignNode *node)
        {
            // Visit children first
            node->VisitAllChildren(this);

            cDeclNode *lvalType = node->GetLval()->GetType();
            cDeclNode *rvalType = node->GetExpr()->GetType();

            if (lvalType != nullptr && rvalType != nullptr)
            {
                if (rvalType->GetTypeRank() > lvalType->GetTypeRank())
                {
                    cBaseTypeNode *src =
                        dynamic_cast<cBaseTypeNode*>(rvalType);
                    cBaseTypeNode *dest =
                        dynamic_cast<cBaseTypeNode*>(lvalType);
                    if (src != nullptr && dest != nullptr)
                    {
                        node->SemanticError("Cannot assign " +
                            src->GetName() + " to " + dest->GetName());
                    }
                }
            }
        }

        virtual void Visit(cVarExprNode *node)
        {
            // Visit children first
            node->VisitAllChildren(this);

            cSymbol *sym = static_cast<cSymbol*>(node->GetElement(0));
            if (sym == nullptr || sym->GetDecl() == nullptr) return;

            cDeclNode *decl = sym->GetDecl();

            // Check if symbol is a function used as a variable
            if (decl->IsFunc())
            {
                node->SemanticError("Symbol " + sym->GetName() +
                    " is a function, not a variable");
                return;
            }

            // Check for array-related errors
            // Look through children for index expressions (non-symbol children)
            for (int i = 1; i < node->NumElements(); i++)
            {
                cAstNode *child = node->GetElement(i);
                // If the child is not a cSymbol, it's an index expression
                cSymbol *childSym = dynamic_cast<cSymbol*>(child);
                if (childSym == nullptr)
                {
                    // Check if the variable's type is an array
                    if (decl->IsVar())
                    {
                        cVarDeclNode *vd =
                            static_cast<cVarDeclNode*>(decl);
                        cSymbol *typeSym = vd->GetType();
                        if (typeSym != nullptr &&
                            typeSym->GetDecl() != nullptr &&
                            !typeSym->GetDecl()->IsArray())
                        {
                            node->SemanticError(sym->GetName() +
                                " is not an array");
                            return;
                        }
                    }

                    // Check if the index is an int type
                    cExprNode *indexExpr =
                        dynamic_cast<cExprNode*>(child);
                    if (indexExpr != nullptr)
                    {
                        cDeclNode *indexType = indexExpr->GetType();
                        if (indexType != nullptr && !indexType->IsInt())
                        {
                            node->SemanticError("Index of " +
                                sym->GetName() + " is not an int");
                            return;
                        }
                    }
                }
            }
        }

        virtual void Visit(cFuncExprNode *node)
        {
            // Visit children first
            node->VisitAllChildren(this);

            cSymbol *sym = node->GetName();
            if (sym == nullptr || sym->GetDecl() == nullptr) return;
            if (!sym->GetDecl()->IsFunc()) return;

            cFuncDeclNode *funcDecl =
                static_cast<cFuncDeclNode*>(sym->GetDecl());

            // Check if function is fully defined
            if (!funcDecl->HasBody())
            {
                node->SemanticError("Function " + sym->GetName() +
                    " is not fully defined");
                return;
            }

            // Check argument count
            int paramCount = 0;
            int argCount = 0;

            if (funcDecl->GetParams() != nullptr)
                paramCount = funcDecl->GetParams()->NumParams();
            if (node->GetParams() != nullptr)
                argCount = node->GetParams()->NumParams();

            if (paramCount != argCount)
            {
                node->SemanticError(sym->GetName() +
                    " called with wrong number of arguments");
                return;
            }

            // Check argument type compatibility
            if (node->GetParams() != nullptr && funcDecl->GetParams() != nullptr)
            {
                for (int i = 0; i < argCount; i++)
                {
                    cDeclNode *argType =
                        node->GetParams()->GetParam(i)->GetType();
                    cVarDeclNode *paramDecl =
                        static_cast<cVarDeclNode*>(
                            funcDecl->GetParams()->GetParam(i));
                    cDeclNode *paramType = nullptr;
                    if (paramDecl->GetType() != nullptr &&
                        paramDecl->GetType()->GetDecl() != nullptr)
                    {
                        paramType = paramDecl->GetType()->GetDecl();
                    }

                    if (argType != nullptr && paramType != nullptr)
                    {
                        if (argType->GetTypeRank() >
                            paramType->GetTypeRank())
                        {
                            node->SemanticError(sym->GetName() +
                                " called with incompatible argument");
                            return;
                        }
                    }
                }
            }
        }
};
