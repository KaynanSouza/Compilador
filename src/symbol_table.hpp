// symbol_table.hpp
#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <string>

class SymbolTable {
public:
    void addSymbol(const std::string& name);
    bool symbolExists(const std::string& name) const;

private:
    std::unordered_map<std::string, bool> symbols;
};

#endif // SYMBOL_TABLE_HPP
