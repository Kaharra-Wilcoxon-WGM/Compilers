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
        cParamListNode(cExprNode *param) : cAstNode(), m_size(0)
        {
            AddChild(param);
        }

        // Add a parameter to the list
        void Insert(cExprNode *param)
        {
            AddChild(param);
        }

        int GetSize() { return m_size; }
        void SetSize(int size) { m_size = size; }

        cExprNode* GetParam(int index) { return static_cast<cExprNode*>(GetChild(index)); }
        int NumParams() { return NumChildren(); }

        virtual string AttributesToString()
        {
            if (m_size == 0) return string("");
            return " size=\"" + std::to_string(m_size) + "\"";
        }

        virtual string NodeType() { return string("params"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        int m_size;
};
