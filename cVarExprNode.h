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
#include "cVarDeclNode.h"

class cVarExprNode : public cExprNode
{
    public:
        // param is the first symbol in the variable reference
        cVarExprNode(cSymbol *sym) : cExprNode(), m_size(0), m_offset(0)
        {
            AddChild(sym);

            // Check if the symbol is defined
            if (sym->GetDecl() == nullptr)
            {
                SemanticParseError("Symbol " + sym->GetName() +
                    " not defined");
            }
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
        int GetRowSize(int i) { return m_rowsizes[i]; }
        int NumRowSizes() { return (int)m_rowsizes.size(); }

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

        virtual cDeclNode* GetType()
        {
            cSymbol *sym = static_cast<cSymbol*>(GetChild(0));
            if (sym == nullptr || sym->GetDecl() == nullptr) return nullptr;
            cDeclNode *decl = sym->GetDecl();
            if (decl->IsFunc()) return nullptr;
            if (decl->IsVar())
            {
                cVarDeclNode *vd = static_cast<cVarDeclNode*>(decl);
                cSymbol *typeSym = vd->GetType();
                if (typeSym == nullptr || typeSym->GetDecl() == nullptr)
                    return nullptr;
                return typeSym->GetDecl();
            }
            return nullptr;
        }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        int m_size;
        int m_offset;
        std::vector<int> m_rowsizes;
};
