#pragma once
//**************************************
// cVarExprNode.h
//
// Defines AST node for variable references
//
// Author: Kaharra Wilcoxon
//

#include <vector>
#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbol.h"

class cVarExprNode : public cExprNode
{
    public:
        // param is the first symbol in the variable reference
        cVarExprNode(cSymbol *sym) : cExprNode(), m_size(0), m_offset(0)
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

        int GetSize() { return m_size; }
        void SetSize(int size) { m_size = size; }
        int GetOffset() { return m_offset; }
        void SetOffset(int offset) { m_offset = offset; }

        void AddRowSize(int rs) { m_rowsizes.push_back(rs); }

        cAstNode* GetElement(int index) { return GetChild(index); }
        int NumElements() { return NumChildren(); }

        virtual string AttributesToString()
        {
            if (m_size == 0 && m_offset == 0) return string("");
            string result("");
            result += " size=\"" + std::to_string(m_size) + "\"";
            result += " offset=\"" + std::to_string(m_offset) + "\"";
            if (!m_rowsizes.empty())
            {
                result += " rowsizes=\"";
                for (int i = 0; i < (int)m_rowsizes.size(); i++)
                {
                    if (i > 0) result += " ";
                    result += std::to_string(m_rowsizes[i]);
                }
                result += "\"";
            }
            return result;
        }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        int m_size;
        int m_offset;
        std::vector<int> m_rowsizes;
};
