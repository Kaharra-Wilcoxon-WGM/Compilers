#pragma once
//**************************************
// cVarExprNode.h
//
// Defines AST node for variable references
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbol.h"

class cVarExprNode : public cExprNode
{
    public:
        // param is the first symbol in the variable reference
        cVarExprNode(cSymbol *sym) : cExprNode()
        {
            AddChild(sym);
        }

        // Add another symbol (for struct field access like a.b.c)
        void AddSymbol(cSymbol *sym)
        {
            AddChild(sym);
        }

        // Add an index expression (for array access like a[0])
        void AddIndex(cExprNode *index)
        {
            AddChild(index);
        }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
