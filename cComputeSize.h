#pragma once
//**************************************
// cComputeSize.h
//
// Visitor that computes size and offset for AST nodes
//
// Author: Kaharra Wilcoxon
//

#include "cVisitor.h"
#include "cProgramNode.h"
#include "cBlockNode.h"
#include "cDeclsNode.h"
#include "cDeclNode.h"
#include "cVarDeclNode.h"
#include "cArrayDeclNode.h"
#include "cStructDeclNode.h"
#include "cFuncDeclNode.h"
#include "cParamsNode.h"
#include "cParamListNode.h"
#include "cVarExprNode.h"
#include "cSymbol.h"

class cComputeSize : public cVisitor
{
    public:
        cComputeSize() : cVisitor(), m_offset(0), m_highWater(0) {}

        virtual void VisitAllNodes(cAstNode *node) { node->Visit(this); }

        //------------------------------------------
        virtual void Visit(cProgramNode *node)
        {
            node->VisitAllChildren(this);
            cBlockNode *block = node->GetBlock();
            if (block != nullptr)
            {
                node->SetSize(RoundUp(block->GetSize(), WORD_SIZE));
            }
        }

        //------------------------------------------
        virtual void Visit(cBlockNode *node)
        {
            int oldOffset = m_offset;
            int oldHighWater = m_highWater;

            // Reset high water to current offset so this block
            // computes its own size independently of sibling blocks
            m_highWater = m_offset;

            node->VisitAllChildren(this);

            node->SetSize(m_highWater - oldOffset);

            m_offset = oldOffset;
            // Propagate the max high water mark to outer scope
            if (oldHighWater > m_highWater)
                m_highWater = oldHighWater;
        }

        //------------------------------------------
        virtual void Visit(cDeclsNode *node)
        {
            int startOffset = m_offset;

            node->VisitAllChildren(this);

            node->SetSize(m_highWater - startOffset);
        }

        //------------------------------------------
        virtual void Visit(cVarDeclNode *node)
        {
            cSymbol *typeSym = node->GetType();
            int typeSize = GetTypeSize(typeSym);

            // Align offset if size > 1
            m_offset = Align(m_offset, typeSize);

            node->SetOffset(m_offset);
            node->SetSize(typeSize);

            // Link this symbol's name to its declaration
            node->GetName()->SetDecl(node);

            m_offset += typeSize;
            if (m_offset > m_highWater)
                m_highWater = m_offset;
        }

        //------------------------------------------
        virtual void Visit(cStructDeclNode *node)
        {
            int oldOffset = m_offset;
            int oldHighWater = m_highWater;

            m_offset = 0;
            m_highWater = 0;

            node->VisitAllChildren(this);

            node->SetSize(m_offset);
            node->SetOffset(0);

            // Link symbol name to this struct declaration
            node->GetName()->SetDecl(node);

            // Restore parent scope
            m_offset = oldOffset;
            m_highWater = oldHighWater;
        }

        //------------------------------------------
        virtual void Visit(cArrayDeclNode *node)
        {
            // Array type declarations don't occupy space
            // Just link the symbol to this declaration
            node->GetName()->SetDecl(node);

            // Visit children so nested types are processed
            node->VisitAllChildren(this);
        }

        //------------------------------------------
        virtual void Visit(cFuncDeclNode *node)
        {
            int oldOffset = m_offset;
            int oldHighWater = m_highWater;

            m_offset = 0;
            m_highWater = 0;

            // Link symbol name to this func declaration
            node->GetName()->SetDecl(node);

            // Process parameters with negative offsets
            if (node->GetParams() != nullptr)
            {
                node->GetParams()->Visit(this);
            }

            // Process local declarations
            if (node->GetDecls() != nullptr)
            {
                node->GetDecls()->Visit(this);
            }

            // Process statements (may contain inner blocks)
            if (node->GetStmts() != nullptr)
            {
                node->GetStmts()->Visit(this);
            }

            node->SetSize(RoundUp(m_highWater, WORD_SIZE));
            node->SetOffset(0);

            // Restore parent scope
            m_offset = oldOffset;
            m_highWater = oldHighWater;
        }

        //------------------------------------------
        virtual void Visit(cParamsNode *node)
        {
            int paramOffset = -12;
            int numParams = node->NumParams();

            for (int i = 0; i < numParams; i++)
            {
                cDeclNode *param = node->GetParam(i);
                // Get size of this parameter's type
                cVarDeclNode *varParam = dynamic_cast<cVarDeclNode*>(param);
                if (varParam != nullptr)
                {
                    cSymbol *typeSym = varParam->GetType();
                    int typeSize = GetTypeSize(typeSym);
                    param->SetSize(typeSize);
                    // Link symbol
                    varParam->GetName()->SetDecl(varParam);
                }

                param->SetOffset(paramOffset);
                paramOffset -= WORD_SIZE;
            }

            node->SetSize(numParams * WORD_SIZE);
        }

        //------------------------------------------
        virtual void Visit(cParamListNode *node)
        {
            node->VisitAllChildren(this);
            node->SetSize(node->NumParams() * WORD_SIZE);
        }

        //------------------------------------------
        virtual void Visit(cVarExprNode *node)
        {
            node->VisitAllChildren(this);

            if (node->NumElements() == 0) return;

            // First element is always a symbol
            cSymbol *baseSym = dynamic_cast<cSymbol*>(node->GetElement(0));
            if (baseSym == nullptr) return;

            cDeclNode *baseDecl = baseSym->GetDecl();
            if (baseDecl == nullptr) return;

            int offset = baseDecl->GetOffset();
            int size = baseDecl->GetSize();
            cDeclNode *currentDecl = baseDecl;

            // Walk through remaining elements
            for (int i = 1; i < node->NumElements(); i++)
            {
                cAstNode *element = node->GetElement(i);
                cSymbol *fieldSym = dynamic_cast<cSymbol*>(element);

                if (fieldSym != nullptr)
                {
                    // Struct field access — resolve by name in the struct's field list
                    cVarDeclNode *currentVar = dynamic_cast<cVarDeclNode*>(currentDecl);
                    if (currentVar != nullptr)
                    {
                        cSymbol *typeSym = currentVar->GetType();
                        cDeclNode *typeDecl = typeSym->GetDecl();
                        cStructDeclNode *structDecl = dynamic_cast<cStructDeclNode*>(typeDecl);
                        if (structDecl != nullptr)
                        {
                            cDeclsNode *fields = structDecl->GetDecls();
                            for (int j = 0; j < fields->NumDecls(); j++)
                            {
                                cVarDeclNode *field = dynamic_cast<cVarDeclNode*>(fields->GetDecl(j));
                                if (field != nullptr && field->GetName()->GetName() == fieldSym->GetName())
                                {
                                    offset += field->GetOffset();
                                    size = field->GetSize();
                                    currentDecl = field;
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    // Array index — compute rowsize and walk down the type chain
                    int rowSize = GetArrayElementSize(currentDecl);
                    node->AddRowSize(rowSize);

                    // Update currentDecl to represent one level deeper for multi-dim arrays
                    cVarDeclNode *varDecl = dynamic_cast<cVarDeclNode*>(currentDecl);
                    if (varDecl != nullptr)
                    {
                        cSymbol *typeSym = varDecl->GetType();
                        cDeclNode *typeDecl = typeSym->GetDecl();
                        cArrayDeclNode *arrayDecl = dynamic_cast<cArrayDeclNode*>(typeDecl);
                        if (arrayDecl != nullptr)
                        {
                            cSymbol *elemTypeSym = arrayDecl->GetType();
                            cDeclNode *elemTypeDecl = elemTypeSym->GetDecl();
                            if (elemTypeDecl != nullptr)
                            {
                                currentDecl = elemTypeDecl;
                            }
                        }
                    }
                }
            }

            node->SetSize(size);
            node->SetOffset(offset);
        }

        // Use default visit (just visit children) for all other node types
        using cVisitor::Visit;

    private:
        int m_offset;
        int m_highWater;

        static const int WORD_SIZE = 4;

        static int RoundUp(int value, int alignment)
        {
            if (value % alignment == 0) return value;
            return value + (alignment - value % alignment);
        }

        static int Align(int offset, int size)
        {
            if (size > 1) return RoundUp(offset, WORD_SIZE);
            return offset;
        }

        int GetTypeSize(cSymbol *typeSym)
        {
            string name = typeSym->GetName();
            if (name == "char")   return 1;
            if (name == "int")    return 4;
            if (name == "float")  return 4;

            // For user-defined types, look up the declaration
            cDeclNode *decl = typeSym->GetDecl();
            if (decl != nullptr)
            {
                // Check if it's an array type declaration
                cArrayDeclNode *arrayDecl = dynamic_cast<cArrayDeclNode*>(decl);
                if (arrayDecl != nullptr)
                {
                    // Array variable size = count * base element size
                    return arrayDecl->GetCount() * GetTypeSize(arrayDecl->GetType());
                }

                return decl->GetSize();
            }
            return 0;
        }

        // Get the element size for array indexing (for rowsizes)
        int GetArrayElementSize(cDeclNode *decl)
        {
            // The decl may be a var_decl whose type is an array
            cVarDeclNode *varDecl = dynamic_cast<cVarDeclNode*>(decl);
            if (varDecl != nullptr)
            {
                cSymbol *typeSym = varDecl->GetType();
                cDeclNode *typeDecl = typeSym->GetDecl();
                cArrayDeclNode *arrayDecl = dynamic_cast<cArrayDeclNode*>(typeDecl);
                if (arrayDecl != nullptr)
                {
                    return GetTypeSize(arrayDecl->GetType());
                }
            }

            // After walking down the type chain, decl may be an array decl directly
            cArrayDeclNode *arrayDecl = dynamic_cast<cArrayDeclNode*>(decl);
            if (arrayDecl != nullptr)
            {
                return GetTypeSize(arrayDecl->GetType());
            }
            return 0;
        }
};
