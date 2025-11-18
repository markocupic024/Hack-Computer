#include "symbol_table.h"

SymbolTable::SymbolTable()
{
    indices[SymbolKind::STATIC] = 0;
    indices[SymbolKind::FIELD] = 0;
    indices[SymbolKind::ARG] = 0;
    indices[SymbolKind::VAR] = 0;
}

void SymbolTable::startSubroutine()
{
    subroutineTable.clear();
    indices[SymbolKind::ARG] = 0;
    indices[SymbolKind::VAR] = 0;
}

void SymbolTable::define(const std::string &name, const std::string &type, SymbolKind kind)
{
    Symbol symbol{type, kind, indices[kind]++};
    
    if (kind == SymbolKind::STATIC || kind == SymbolKind::FIELD)
    {
        classTable[name] = symbol;
    }
    else
    {
        subroutineTable[name] = symbol;
    }
}

int SymbolTable::varCount(SymbolKind kind) const
{
    return indices.at(kind);
}

SymbolKind SymbolTable::kindOf(const std::string &name) const
{
    if (subroutineTable.count(name))
    {
        return subroutineTable.at(name).kind;
    }
    else if (classTable.count(name))
    {
        return classTable.at(name).kind;
    }
    return SymbolKind::NONE;
}

std::string SymbolTable::typeOf(const std::string &name) const
{
    if (subroutineTable.count(name))
    {
        return subroutineTable.at(name).type;
    }
    else if (classTable.count(name))
    {
        return classTable.at(name).type;
    }
    return "";
}

int SymbolTable::indexOf(const std::string &name) const
{
    if (subroutineTable.count(name))
    {
        return subroutineTable.at(name).index;
    }
    else if (classTable.count(name))
    {
        return classTable.at(name).index;
    }
    return -1;
}

