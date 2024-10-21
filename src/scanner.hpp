// scanner.hpp

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "token.hpp"
#include <vector>
#include <string>

// Classe responsável por analisar o código fonte e gerar a lista de tokens.
class Scanner {
public:
    explicit Scanner(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string source;    // Código fonte a ser analisado
    std::vector<Token> tokens;   // Lista de tokens gerados
    size_t start = 0;            // Início do lexema atual
    size_t current = 0;          // Posição atual no código fonte
    size_t line = 1;             // Linha atual (para mensagens de erro)

    // Métodos auxiliares
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    void addToken(TokenType type, const std::string& lexeme);
    void scanToken();
    void number();
    void identifier();
};

#endif // SCANNER_HPP
