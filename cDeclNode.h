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

        virtual bool IsArray()  { return false; }
        virtual bool IsStruct() { return false; }
        virtual bool IsType()   { return false; }
        virtual bool IsFunc()   { return false; }
        virtual bool IsVar()    { return false; }
        virtual bool IsFloat()  { return false; }
        virtual bool IsInt()    { return false; }
        virtual bool IsChar()   { return false; }

        virtual int GetTypeRank() { return 0; }

        virtual int GetSize()    { return m_size; }
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
