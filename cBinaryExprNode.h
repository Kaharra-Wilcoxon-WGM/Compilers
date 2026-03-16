#pragma once
//**************************************
// cBinaryExprNode.h
//
// Defines AST node for binary expressions (e.g., a + b)
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cOpNode.h"
#include "cSymbolTable.h"

class cBinaryExprNode : public cExprNode
{
    public:
        // params are the left expr, operator, and right expr
        cBinaryExprNode(cExprNode *left, cOpNode *op, cExprNode *right)
            : cExprNode()
        {
            AddChild(left);
            AddChild(op);
            AddChild(right);
        }

        cExprNode* GetLeft()   { return static_cast<cExprNode*>(GetChild(0)); }
        cOpNode* GetOp()       { return static_cast<cOpNode*>(GetChild(1)); }
        cExprNode* GetRight()  { return static_cast<cExprNode*>(GetChild(2)); }

        virtual cDeclNode* GetType()
        {
            int op = GetOp()->GetOp();
            if (op == EQUALS || op == NOT_EQUALS || op == '<' || op == '>' ||
                op == LE || op == GE || op == AND || op == OR)
            {
                return g_symbolTable.Find("int")->GetDecl();
            }
            cDeclNode *leftType = GetLeft()->GetType();
            cDeclNode *rightType = GetRight()->GetType();
            if (leftType == nullptr) return rightType;
            if (rightType == nullptr) return leftType;
            if (leftType->GetTypeRank() >= rightType->GetTypeRank())
                return leftType;
            return rightType;
        }

        virtual string NodeType() { return string("expr"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
