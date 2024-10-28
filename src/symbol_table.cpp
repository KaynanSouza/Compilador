// symbol_table.cpp

#include "symbol_table.hpp"

void SymbolTable::enterScope() {
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    scopes.pop_back();
}

void SymbolTable::define(const std::string& name, const std::string& type, SymbolType symbolType) {
    Symbol symbol(name, type, symbolType);
    scopes.back()[name] = symbol;
}

Symbol* SymbolTable::resolve(const std::string& name) {
    for (auto scopeIt = scopes.rbegin(); scopeIt != scopes.rend(); ++scopeIt) {
        auto it = scopeIt->find(name);
        if (it != scopeIt->end()) {
            return &(it->second);
        }
    }
    return nullptr;
}

std::unordered_map<std::string, Symbol>& SymbolTable::currentScope() {
    return scopes.back();
}
