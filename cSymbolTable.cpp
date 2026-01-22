//****************************************************
// cSymbolTable.cpp
// Implementation of the symbol table for the compiler
//
// Author: Kaharra Wilcoxon
//

#include "cSymbolTable.h"

// Construct an empty symbol table with one initial scope
cSymbolTable::cSymbolTable()
{
    // Create the initial (global) scope
    m_scopes.push_back(new symbolTable_t());
}

// Increase the scope: add a level to the nested symbol table
// Return value is the newly created scope
symbolTable_t *cSymbolTable::IncreaseScope()
{
    symbolTable_t *newScope = new symbolTable_t();
    m_scopes.push_back(newScope);
    return newScope;
}

// Decrease the scope: remove the outer-most scope.
// Returned value is the outer-most scope AFTER the pop.
// NOTE: do NOT clean up memory - AST nodes may still reference symbols
symbolTable_t *cSymbolTable::DecreaseScope()
{
    // Remove the innermost scope (don't delete - symbols may still be referenced)
    m_scopes.pop_back();

    // Return the new innermost scope, or nullptr if no scopes remain
    if (m_scopes.empty())
        return nullptr;

    return m_scopes.back();
}

// Insert a symbol into the current (innermost) scope
void cSymbolTable::Insert(cSymbol *sym)
{
    if (!m_scopes.empty())
    {
        (*m_scopes.back())[sym->GetName()] = sym;
    }
}

// Do a lookup in the nested table, starting from innermost scope
// and working outward until a match is found
cSymbol *cSymbolTable::Find(string name)
{
    // Search from innermost to outermost scope
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it)
    {
        auto found = (*it)->find(name);
        if (found != (*it)->end())
        {
            return found->second;
        }
    }
    return nullptr;
}

// Find a symbol only in the innermost scope
cSymbol *cSymbolTable::FindLocal(string name)
{
    if (m_scopes.empty())
        return nullptr;

    auto found = m_scopes.back()->find(name);
    if (found != m_scopes.back()->end())
    {
        return found->second;
    }
    return nullptr;
}
