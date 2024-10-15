#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <vector>
#include <string>
#include "token.hpp"

class Scanner {
public:
    explicit Scanner(const std::string& sourceCode);
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    size_t current = 0;
    size_t start = 0;

    std::vector<Token> tokens;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    void skipWhitespace();
    void skipComment();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& text);
    void scanToken();
    void identifier();
    void number();
    bool match(char expected);
};

#endif // SCANNER_HPP
