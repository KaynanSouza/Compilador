// symbol_table.cpp
#include "symbol_table.hpp"

void SymbolTable::addSymbol(const std::string& name) {
    symbols[name] = true;
}

bool SymbolTable::symbolExists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}
