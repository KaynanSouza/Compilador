// token.hpp

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// Enumeração dos tipos de tokens que podem ser reconhecidos pelo scanner.
enum class TokenType {
    // Literais
    IDENTIFIER,   // Identificadores (nomes de variáveis, funções, etc.)
    NUMBER,       // Números (inteiros e reais)
    TRUE,         // Literal booleano TRUE
    FALSE,        // Literal booleano FALSE

    // Palavras-chave
    VAR,          // 'VAR'
    VAR_INPUT,    // 'VAR_INPUT'
    END_VAR,      // 'END_VAR'
    PROGRAM,      // 'PROGRAM'
    END_PROGRAM,  // 'END_PROGRAM'
    FUNCTION,     // 'FUNCTION'
    END_FUNCTION, // 'END_FUNCTION'
    IF,           // 'IF'
    THEN,         // 'THEN'
    ELSE,         // 'ELSE'
    END_IF,       // 'END_IF'
    WHILE,        // 'WHILE'
    DO,           // 'DO'
    END_WHILE,    // 'END_WHILE'
    FOR,          // 'FOR'
    TO,           // 'TO'
    END_FOR,      // 'END_FOR'
    RETURN,       // 'RETURN'
    BOOLEAN,      // 'BOOLEAN'

    // Tipos
    INTEGER,      // 'INTEGER'
    REAL,         // 'REAL'

    // Operadores
    PLUS,           // '+'
    MINUS,          // '-'
    STAR,           // '*'
    SLASH,          // '/'
    ASSIGNMENT,     // ':='
    EQUAL_EQUAL,    // '=='
    NOT_EQUAL,      // '!='
    LESS,           // '<'
    LESS_EQUAL,     // '<='
    GREATER,        // '>'
    GREATER_EQUAL,  // '>='
    NOT,            // 'NOT'
    AND,            // 'AND'
    OR,             // 'OR'

    // Delimitadores
    LEFT_PAREN,     // '('
    RIGHT_PAREN,    // ')'
    COLON,          // ':'
    SEMICOLON,      // ';'
    COMMA,          // ','

    // Especial
    EOF_TOKEN       // Fim do arquivo
};

// Estrutura que representa um token, contendo seu tipo e lexema.
struct Token {
    TokenType type;       // Tipo do token
    std::string lexeme;   // Texto original do token

    Token(TokenType type, const std::string& lexeme)
        : type(type), lexeme(lexeme) {}
};

#endif // TOKEN_HPP
