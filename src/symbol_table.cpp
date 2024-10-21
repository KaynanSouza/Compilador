// symbol_table.cpp

#include "symbol_table.hpp"

void SymbolTable::enterScope() {
    scopes.emplace_back(); // Adiciona um novo escopo à pilha
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back(); // Remove o escopo atual da pilha
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, SymbolType symbolType) {
    if (scopes.empty()) {
        enterScope(); // Garante que exista pelo menos um escopo
    }
    auto& currentScope = scopes.back();
    auto result = currentScope.emplace(name, Symbol(name, type, symbolType));
    if (!result.second) {
        return false; // Já declarado neste escopo
    }
    return true;
}

Symbol* SymbolTable::resolve(const std::string& name) {
    // Procura o símbolo nos escopos, do mais interno ao mais externo
    for (auto scopeIt = scopes.rbegin(); scopeIt != scopes.rend(); ++scopeIt) {
        auto it = scopeIt->find(name);
        if (it != scopeIt->end()) {
            return &(it->second);
        }
    }
    return nullptr; // Não encontrado
}
