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
    IF,
    THEN,
    ELSE,
    END_IF,
    WHILE,
    DO,
    END_WHILE,
    FOR,
    TO,
    END_FOR,
    AND,
    OR,
    NOT,
    LESS,          // <
    LESS_EQUAL,    // <=
    GREATER,       // >
    GREATER_EQUAL, // >=
    EQUAL_EQUAL,   // ==
    NOT_EQUAL,     // !=
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
