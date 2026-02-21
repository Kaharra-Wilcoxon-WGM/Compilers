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
        cParamsNode(cDeclNode *param) : cAstNode(), m_size(0)
        {
            AddChild(param);
        }

        // Add a parameter to the list
        void Insert(cDeclNode *param)
        {
            AddChild(param);
        }

        int GetSize() { return m_size; }
        void SetSize(int size) { m_size = size; }

        cDeclNode* GetParam(int index) { return static_cast<cDeclNode*>(GetChild(index)); }
        int NumParams() { return NumChildren(); }

        virtual string AttributesToString()
        {
            if (m_size == 0) return string("");
            return " size=\"" + std::to_string(m_size) + "\"";
        }

        virtual string NodeType() { return string("args"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        int m_size;
};
