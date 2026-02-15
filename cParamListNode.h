#pragma once
//**************************************
// cParamListNode.h
//
// Defines AST node for function call parameters (params)
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cExprNode.h"

class cParamListNode : public cAstNode
{
    public:
        // param is the first parameter expression
        cParamListNode(cExprNode *param) : cAstNode()
        {
            AddChild(param);
        }

        // Add a parameter to the list
        void Insert(cExprNode *param)
        {
            AddChild(param);
        }

        virtual string NodeType() { return string("params"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
