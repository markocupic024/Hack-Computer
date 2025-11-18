#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

enum class SymbolKind
{
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
};

struct Symbol
{
    std::string type;
    SymbolKind kind;
    int index;
};

class SymbolTable
{
public:
    SymbolTable();
    
    void startSubroutine();
    void define(const std::string &name, const std::string &type, SymbolKind kind);
    int varCount(SymbolKind kind) const;
    SymbolKind kindOf(const std::string &name) const;
    std::string typeOf(const std::string &name) const;
    int indexOf(const std::string &name) const;

private:
    std::unordered_map<std::string, Symbol> classTable;
    std::unordered_map<std::string, Symbol> subroutineTable;
    
    std::unordered_map<SymbolKind, int> indices;
};

#endif

