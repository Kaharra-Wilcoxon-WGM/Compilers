#pragma once
//**************************************
// cPrintsNode.h
//
// Defines AST node for prints statements (print string)
//
// Author: Kaharra Wilcoxon
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include <string>

class cPrintsNode : public cStmtNode
{
    public:
        // param is the string literal (including quotes)
        cPrintsNode(std::string *str) : cStmtNode()
        {
            // Remove surrounding quotes from the string
            if (str->length() >= 2)
            {
                m_value = str->substr(1, str->length() - 2);
            }
            else
            {
                m_value = *str;
            }
            delete str;  // Free the string allocated by the scanner
        }

        virtual string AttributesToString()
        {
            return " value=\"" + m_value + "\"";
        }

        virtual string NodeType() { return string("prints"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    private:
        string m_value;
};
