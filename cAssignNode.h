#pragma once
//**************************************
// cAssignNode.h
//
// Defines AST node for assignment statements
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include "cExprNode.h"

class cAssignNode : public cStmtNode
{
    public:
        // params are the lval (varref) and expression
        cAssignNode(cExprNode *lval, cExprNode *expr)
            : cStmtNode()
        {
            AddChild(lval);
            AddChild(expr);
        }

        virtual string NodeType() { return string("assign"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
