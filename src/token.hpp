// token.hpp

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    // Tokens de um caractere
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    COMMA,          // ,
    DOT,            // .
    SEMICOLON,      // ;
    COLON,          // :
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    ASSIGNMENT,     // :=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    AND,            // AND
    OR,             // OR
    NOT,            // NOT
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    DOT_DOT,        // ..
    // Literais
    IDENTIFIER,
    STRING,
    NUMBER,
    // Palavras-chave
    VAR,
    VAR_INPUT,
    VAR_OUTPUT,
    VAR_GLOBAL,
    END_VAR,
    FUNCTION,
    END_FUNCTION,
    FUNCTION_BLOCK,
    END_FUNCTION_BLOCK,
    PROGRAM,
    END_PROGRAM,
    IF,
    THEN,
    ELSE,
    ELSIF,
    END_IF,
    WHILE,
    DO,
    END_WHILE,
    FOR,
    TO,
    END_FOR,
    RETURN,
    ARRAY,
    OF,
    TRUE,
    FALSE,
    // Tipos
    INTEGER,
    REAL,
    BOOLEAN,
    // Outros
    EOF_TOKEN,
    TIME_LITERAL
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, const std::string& lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}
};

#endif // TOKEN_HPP
