#pragma once
//**************************************
// cCodeGen.h
//
// Code generation visitor that emits stackl assembly
//
// Author: Kaharra Wilcoxon
//

#include <string>
#include "cVisitor.h"
#include "emit.h"
#include "cProgramNode.h"
#include "cBlockNode.h"
#include "cDeclsNode.h"
#include "cDeclNode.h"
#include "cVarDeclNode.h"
#include "cFuncDeclNode.h"
#include "cFuncExprNode.h"
#include "cParamListNode.h"
#include "cStmtsNode.h"
#include "cAssignNode.h"
#include "cPrintNode.h"
#include "cPrintsNode.h"
#include "cIfNode.h"
#include "cWhileNode.h"
#include "cReturnNode.h"
#include "cIntExprNode.h"
#include "cBinaryExprNode.h"
#include "cVarExprNode.h"
#include "cOpNode.h"
#include "cSymbol.h"
#include "langparse.h"

class cCodeGen : public cVisitor
{
    public:
        cCodeGen() : cVisitor() {}

        virtual void VisitAllNodes(cAstNode *node) { node->Visit(this); }

        //------------------------------------------
        virtual void Visit(cProgramNode *node)
        {
            EmitString(".dataseg\n");
            EmitString(".codeseg\n");

            cBlockNode *block = node->GetBlock();
            int size = node->GetSize();

            // Allocate space for main program variables
            if (size > 0)
            {
                EmitString("ADJSP ");
                EmitInt(size);
                EmitString("\n");
            }

            // Visit the block's statements (skip function declarations)
            cStmtsNode *stmts = block->GetStmts();
            if (stmts != nullptr)
                stmts->Visit(this);

            EmitString("HALT\n");

            // Emit all function definitions
            cDeclsNode *decls = block->GetDecls();
            if (decls != nullptr)
            {
                for (int i = 0; i < decls->NumDecls(); i++)
                {
                    cFuncDeclNode *func =
                        dynamic_cast<cFuncDeclNode*>(decls->GetDecl(i));
                    if (func != nullptr && func->GetStmts() != nullptr)
                    {
                        func->Visit(this);
                    }
                }
            }
        }

        //------------------------------------------
        virtual void Visit(cBlockNode *node)
        {
            // Inner blocks don't emit ADJSP - space is allocated by
            // the enclosing program or function
            cStmtsNode *stmts = node->GetStmts();
            if (stmts != nullptr)
                stmts->Visit(this);
        }

        //------------------------------------------
        virtual void Visit(cStmtsNode *node)
        {
            node->VisitAllChildren(this);
        }

        //------------------------------------------
        virtual void Visit(cIntExprNode *node)
        {
            EmitString("PUSH ");
            EmitInt(node->GetValue());
            EmitString("\n");
        }

        //------------------------------------------
        virtual void Visit(cBinaryExprNode *node)
        {
            // Evaluate left operand
            node->GetLeft()->Visit(this);
            // Evaluate right operand
            node->GetRight()->Visit(this);
            // Emit the operator instruction
            EmitOperator(node->GetOp()->GetOp());
        }

        //------------------------------------------
        virtual void Visit(cVarExprNode *node)
        {
            // Default: emit as rvalue (push value)
            EmitVarExprRval(node);
        }

        //------------------------------------------
        virtual void Visit(cAssignNode *node)
        {
            cVarExprNode *lval =
                dynamic_cast<cVarExprNode*>(node->GetLval());
            cExprNode *rval = node->GetExpr();

            // Evaluate the rvalue expression (pushes value)
            rval->Visit(this);

            // Compute lvalue address and store
            EmitVarExprLval(lval);
        }

        //------------------------------------------
        virtual void Visit(cPrintNode *node)
        {
            // Evaluate expression to print
            node->GetExpr()->Visit(this);

            // Call the print function from io320.sl
            EmitString("CALL @print\n");

            // Cleanup: discard return value, then discard argument
            EmitString("POP\n");
            EmitString("POP\n");
        }

        //------------------------------------------
        virtual void Visit(cPrintsNode *node)
        {
            std::string label = GenerateLabel();

            EmitString("PUSH @" + label + "\n");
            EmitString(".dataseg\n");
            EmitString(label + ":\n");
            EmitString(".string \"" + node->GetValue() + "\"\n");
            EmitString(".codeseg\n");
            EmitString("OUTS\n");
        }

        //------------------------------------------
        virtual void Visit(cIfNode *node)
        {
            std::string elseLabel = GenerateLabel();
            std::string endLabel = GenerateLabel();

            // Evaluate condition
            node->GetCondition()->Visit(this);

            if (node->GetElseStmts() != nullptr)
            {
                // If-else: jump to else if condition is false (0)
                EmitString("JUMPE @" + elseLabel + "\n");
                node->GetIfStmts()->Visit(this);
                EmitString("JUMP @" + endLabel + "\n");
                EmitString(elseLabel + ":\n");
                node->GetElseStmts()->Visit(this);
                EmitString(endLabel + ":\n");
            }
            else
            {
                // If without else
                EmitString("JUMPE @" + endLabel + "\n");
                node->GetIfStmts()->Visit(this);
                EmitString(endLabel + ":\n");
            }
        }

        //------------------------------------------
        virtual void Visit(cWhileNode *node)
        {
            std::string startLabel = GenerateLabel();
            std::string endLabel = GenerateLabel();

            EmitString(startLabel + ":\n");
            node->GetCondition()->Visit(this);
            EmitString("JUMPE @" + endLabel + "\n");
            node->GetBody()->Visit(this);
            EmitString("JUMP @" + startLabel + "\n");
            EmitString(endLabel + ":\n");
        }

        //------------------------------------------
        virtual void Visit(cFuncDeclNode *node)
        {
            std::string name = node->GetName()->GetName();

            EmitString(".function " + name + "\n");
            EmitString(name + ":\n");

            int size = node->GetSize();
            if (size > 0)
            {
                EmitString("ADJSP ");
                EmitInt(size);
                EmitString("\n");
            }

            // Visit function body statements
            if (node->GetStmts() != nullptr)
                node->GetStmts()->Visit(this);

            // Default return in case no return statement was reached
            EmitString("PUSH 0\n");
            EmitString("RETURNV\n");
        }

        //------------------------------------------
        virtual void Visit(cFuncExprNode *node)
        {
            cParamListNode *params = node->GetParams();
            int numArgs = 0;

            if (params != nullptr)
            {
                numArgs = params->NumParams();

                // Push arguments in reverse order so that
                // first declared param ends up at FP-12
                for (int i = numArgs - 1; i >= 0; i--)
                {
                    params->GetParam(i)->Visit(this);
                }
            }

            // Call the function
            std::string name = node->GetName()->GetName();
            EmitString("CALL @" + name + "\n");

            // Clean up arguments while preserving return value
            for (int i = 0; i < numArgs; i++)
            {
                EmitString("SWAP\n");
                EmitString("POP\n");
            }
        }

        //------------------------------------------
        virtual void Visit(cReturnNode *node)
        {
            node->GetExpr()->Visit(this);
            EmitString("RETURNV\n");
        }

        // Use default visit (just visit children) for all other node types
        using cVisitor::Visit;

    private:
        //------------------------------------------
        // Emit the stackl instruction for an operator
        void EmitOperator(int op)
        {
            switch (op)
            {
                case '+':        EmitString("PLUS\n"); break;
                case '-':        EmitString("MINUS\n"); break;
                case '*':        EmitString("TIMES\n"); break;
                case '/':        EmitString("DIVIDE\n"); break;
                case '%':        EmitString("MOD\n"); break;
                case EQUALS:     EmitString("EQ\n"); break;
                case NOT_EQUALS: EmitString("NE\n"); break;
                case '<':        EmitString("LT\n"); break;
                case '>':        EmitString("GT\n"); break;
                case LE:         EmitString("LE\n"); break;
                case GE:         EmitString("GE\n"); break;
                case AND:        EmitString("AND\n"); break;
                case OR:         EmitString("OR\n"); break;
            }
        }

        //------------------------------------------
        // Emit code to push the value of a variable expression (rvalue)
        void EmitVarExprRval(cVarExprNode *node)
        {
            int size = node->GetSize();
            int offset = node->GetOffset();
            bool hasArrayIndex = (node->NumRowSizes() > 0);

            if (!hasArrayIndex)
            {
                // Simple variable access
                if (size == 1 && offset >= 0)
                {
                    // Local char variable: compute address, load byte
                    EmitString("PUSH ");
                    EmitInt(offset);
                    EmitString("\n");
                    EmitString("PUSHFP\n");
                    EmitString("PLUS\n");
                    EmitString("PUSHCVARIND\n");
                }
                else
                {
                    // Int/word variable or parameter: direct access
                    EmitString("PUSHVAR ");
                    EmitInt(offset);
                    EmitString("\n");
                }
            }
            else
            {
                // Array access: compute element address, then load
                EmitArrayAddress(node);

                if (size == 1)
                    EmitString("PUSHCVARIND\n");
                else
                    EmitString("PUSHVARIND\n");
            }
        }

        //------------------------------------------
        // Emit code to compute lvalue address and store
        // Assumes the value to store is already on the stack
        void EmitVarExprLval(cVarExprNode *node)
        {
            int size = node->GetSize();
            int offset = node->GetOffset();
            bool hasArrayIndex = (node->NumRowSizes() > 0);

            if (!hasArrayIndex)
            {
                // Simple variable: compute address
                EmitString("PUSH ");
                EmitInt(offset);
                EmitString("\n");
                EmitString("PUSHFP\n");
                EmitString("PLUS\n");
            }
            else
            {
                // Array element: compute address
                EmitArrayAddress(node);
            }

            // Store: POPCVARIND for local chars, POPVARIND otherwise
            if (size == 1 && offset >= 0)
                EmitString("POPCVARIND\n");
            else
                EmitString("POPVARIND\n");
        }

        //------------------------------------------
        // Emit code to compute the address of an array element
        // Result: address is pushed on the stack
        void EmitArrayAddress(cVarExprNode *node)
        {
            int offset = node->GetOffset();

            // Start with base address: FP + offset
            EmitString("PUSH ");
            EmitInt(offset);
            EmitString("\n");
            EmitString("PUSHFP\n");
            EmitString("PLUS\n");

            // Add each index * rowsize
            int rowSizeIdx = 0;
            for (int i = 0; i < node->NumElements(); i++)
            {
                cAstNode *element = node->GetElement(i);

                // Skip symbols (variable name / struct fields)
                cSymbol *sym = dynamic_cast<cSymbol*>(element);
                if (sym == nullptr)
                {
                    // This is an index expression
                    element->Visit(this);
                    EmitString("PUSH ");
                    EmitInt(node->GetRowSize(rowSizeIdx));
                    EmitString("\n");
                    EmitString("TIMES\n");
                    EmitString("PLUS\n");
                    rowSizeIdx++;
                }
            }
        }
};
