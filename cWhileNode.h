#pragma once
//**************************************
// cWhileNode.h
//
// Defines AST node for while statements
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include "cExprNode.h"

class cWhileNode : public cStmtNode
{
    public:
        // params are condition and body statement
        cWhileNode(cExprNode *cond, cStmtNode *stmt)
            : cStmtNode()
        {
            AddChild(cond);
            AddChild(stmt);
        }

        cExprNode* GetCondition() { return static_cast<cExprNode*>(GetChild(0)); }
        cStmtNode* GetBody() { return static_cast<cStmtNode*>(GetChild(1)); }

        virtual string NodeType() { return string("while"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
