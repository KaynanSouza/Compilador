#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    ASSIGNMENT,    // :=
    COLON,         // :
    COMMA,         // ,
    LEFT_PAREN,    // (
    RIGHT_PAREN,   // )
    SEMICOLON,     // ;
    VAR,
    END_VAR,
    VAR_INPUT,
    FUNCTION,
    END_FUNCTION,
    PROGRAM,
    END_PROGRAM,
    REAL,
    INTEGER,
    RETURN,
    PLUS,          // +
    MINUS,         // -
    STAR,          // *
    SLASH,         // /
    EOF_TOKEN,     // End of file
    // Add other token types as needed
};

class Token {
public:
    TokenType type;
    std::string lexeme;

    Token(TokenType type, std::string lexeme)
        : type(type), lexeme(std::move(lexeme)) {}
};

#endif // TOKEN_HPP
