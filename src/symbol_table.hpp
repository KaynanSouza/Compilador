// symbol_table.hpp

#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

enum class SymbolType {
    VARIABLE,
    FUNCTION,
    ARRAY,
    // Adicione outros tipos, se necessário
};

struct Symbol {
    std::string name;
    std::string type;
    SymbolType symbolType;
    std::vector<std::pair<int, int>> dimensions; // Para arrays

    // Construtor padrão
    Symbol() = default;

    Symbol(const std::string& name, const std::string& type, SymbolType symbolType)
        : name(name), type(type), symbolType(symbolType) {}
};

class SymbolTable {
public:
    void enterScope();
    void exitScope();
    void define(const std::string& name, const std::string& type, SymbolType symbolType);
    Symbol* resolve(const std::string& name);

    std::unordered_map<std::string, Symbol>& currentScope();

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes = {{}};
};

#endif // SYMBOL_TABLE_HPP
