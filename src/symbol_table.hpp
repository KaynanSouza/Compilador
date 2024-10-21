// symbol_table.hpp

#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

// Enumeração para o tipo de símbolo (variável ou função)
enum class SymbolType {
    VARIABLE,
    FUNCTION
};

// Estrutura que representa um símbolo na tabela de símbolos
struct Symbol {
    std::string name;       // Nome do símbolo
    std::string type;       // Tipo do símbolo (ex: INTEGER, REAL)
    SymbolType symbolType;  // Tipo do símbolo (variável ou função)

    Symbol(const std::string& name, const std::string& type, SymbolType symbolType)
        : name(name), type(type), symbolType(symbolType) {}
};

// Classe que implementa a tabela de símbolos com suporte a escopos
class SymbolTable {
public:
    void enterScope();  // Entra em um novo escopo
    void exitScope();   // Sai do escopo atual
    bool declare(const std::string& name, const std::string& type, SymbolType symbolType); // Declara um novo símbolo
    Symbol* resolve(const std::string& name); // Resolve um símbolo pelo nome

private:
    // Pilha de escopos, cada escopo é um mapa de símbolos
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

#endif // SYMBOL_TABLE_HPP
