#pragma once
//**************************************
// cIfNode.h
//
// Defines AST node for if statements
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include "cExprNode.h"
#include "cStmtsNode.h"

class cIfNode : public cStmtNode
{
    public:
        // params are condition, if-stmts, and optional else-stmts
        cIfNode(cExprNode *cond, cStmtsNode *ifStmts, cStmtsNode *elseStmts = nullptr)
            : cStmtNode()
        {
            AddChild(cond);
            AddChild(ifStmts);
            if (elseStmts != nullptr)
                AddChild(elseStmts);
        }

        cExprNode* GetCondition() { return static_cast<cExprNode*>(GetChild(0)); }
        cStmtsNode* GetIfStmts() { return static_cast<cStmtsNode*>(GetChild(1)); }
        cStmtsNode* GetElseStmts()
        {
            if (NumChildren() > 2)
                return static_cast<cStmtsNode*>(GetChild(2));
            return nullptr;
        }

        virtual string NodeType() { return string("if"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
