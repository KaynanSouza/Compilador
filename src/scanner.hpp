// scanner.hpp

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <vector>
#include "token.hpp"

class Scanner {
public:
    Scanner(const std::string& source);

    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;

    bool isAtEnd() const;
    void scanToken();
    char advance();
    bool match(char expected);
    char peek() const;
    char peekNext() const;
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& text);
    void identifier();
    void number();
    void skipWhitespaceAndComments();
};

#endif // SCANNER_HPP
