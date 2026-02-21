#pragma once
//**************************************
// cDeclNode
//
// Defines base class for all declarations.
//
// Author: Phil Howard
// phil.howard@oit.edu
//

#include "cAstNode.h"

class cDeclNode : public cAstNode
{
    public:
        cDeclNode() : cAstNode(), m_size(0), m_offset(0) {}

        int GetSize()    { return m_size; }
        void SetSize(int size) { m_size = size; }
        int GetOffset()  { return m_offset; }
        void SetOffset(int offset) { m_offset = offset; }

        virtual string AttributesToString()
        {
            if (m_size == 0 && m_offset == 0) return string("");
            string result("");
            result += " size=\"" + std::to_string(m_size) + "\"";
            result += " offset=\"" + std::to_string(m_offset) + "\"";
            return result;
        }
    protected:
        int m_size;
        int m_offset;
};
