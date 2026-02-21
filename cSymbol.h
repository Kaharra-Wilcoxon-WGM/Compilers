#pragma once
//**************************************
// cSymbol.h
//
// Defines class used to represent symbols.
// Later labs will add features to this class.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include <string>

using std::string;

#include "cAstNode.h"

class cDeclNode;

class cSymbol : public cAstNode
{
    public:
        // param is name of symbol
        cSymbol(string name) : cAstNode()
        {
            m_id = ++nextId;        // get next available ID
            m_name = name;
            m_isType = false;
            m_decl = nullptr;
        }

        // return name of symbol
        string GetName() { return m_name; }

        // Check if this symbol represents a type
        bool IsType() { return m_isType; }

        // Set whether this symbol represents a type
        void SetIsType(bool isType) { m_isType = isType; }

        // Get/Set the declaration node associated with this symbol
        cDeclNode* GetDecl() { return m_decl; }
        void SetDecl(cDeclNode* decl) { m_decl = decl; }

        virtual string AttributesToString()
        {
            string result(" id=\"");
            result += std::to_string(m_id);
            result += "\" name=\"" + m_name + "\"";
            return result;
        }
        virtual string NodeType() { return string("sym"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    protected:
        static long long nextId;        // Next avail symbol ID
        long long m_id;                 // Unique ID for this symbol
        string m_name;                  // name of symbol
        bool m_isType;                  // true if this symbol is a type
        cDeclNode *m_decl;              // declaration associated with this symbol
};
