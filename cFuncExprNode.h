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
        }

        // Constructor without parameters
        cFuncExprNode(cSymbol *name)
            : cExprNode()
        {
            AddChild(name);
        }

        cSymbol* GetName() { return static_cast<cSymbol*>(GetChild(0)); }
        cParamListNode* GetParams()
        {
            if (NumChildren() > 1)
                return static_cast<cParamListNode*>(GetChild(1));
            return nullptr;
        }

        virtual string NodeType() { return string("funcCall"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
