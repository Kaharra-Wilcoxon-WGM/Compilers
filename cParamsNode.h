#pragma once
//**************************************
// cParamsNode.h
//
// Defines AST node for function parameter declarations (args)
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cDeclNode.h"

class cParamsNode : public cAstNode
{
    public:
        // param is the first parameter declaration
        cParamsNode(cDeclNode *param) : cAstNode()
        {
            AddChild(param);
        }

        // Add a parameter to the list
        void Insert(cDeclNode *param)
        {
            AddChild(param);
        }

        virtual string NodeType() { return string("args"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
