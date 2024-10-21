// scanner.cpp

#include "scanner.hpp"
#include <cctype>
#include <unordered_map>
#include <stdexcept>

// Mapeamento das palavras-chave para seus respectivos tipos de token.
static std::unordered_map<std::string, TokenType> keywords = {
    {"VAR", TokenType::VAR},
    {"VAR_INPUT", TokenType::VAR_INPUT},
    {"END_VAR", TokenType::END_VAR},
    {"PROGRAM", TokenType::PROGRAM},
    {"END_PROGRAM", TokenType::END_PROGRAM},
    {"FUNCTION", TokenType::FUNCTION},
    {"END_FUNCTION", TokenType::END_FUNCTION},
    {"IF", TokenType::IF},
    {"THEN", TokenType::THEN},
    {"ELSE", TokenType::ELSE},
    {"END_IF", TokenType::END_IF},
    {"WHILE", TokenType::WHILE},
    {"DO", TokenType::DO},
    {"END_WHILE", TokenType::END_WHILE},
    {"FOR", TokenType::FOR},
    {"TO", TokenType::TO},
    {"END_FOR", TokenType::END_FOR},
    {"RETURN", TokenType::RETURN},
    {"TRUE", TokenType::TRUE},
    {"FALSE", TokenType::FALSE},
    {"INTEGER", TokenType::INTEGER},
    {"REAL", TokenType::REAL},
    {"BOOLEAN", TokenType::BOOLEAN},
    {"NOT", TokenType::NOT},
    {"AND", TokenType::AND},
    {"OR", TokenType::OR},
};

Scanner::Scanner(const std::string& source)
    : source(source) {}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::EOF_TOKEN, ""); // Adiciona token de fim de arquivo
    return tokens;
}

bool Scanner::isAtEnd() const {
    return current >= source.length();
}

char Scanner::advance() {
    return source[current++];
}

char Scanner::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Scanner::addToken(TokenType type, const std::string& lexeme) {
    tokens.emplace_back(type, lexeme);
}

void Scanner::scanToken() {
    char c = advance();
    if (isspace(static_cast<unsigned char>(c))) {
        // Ignora espaços em branco e controla a contagem de linhas
        if (c == '\n') {
            line++;
        }
        return;
    }
    switch (c) {
        case '+':
            addToken(TokenType::PLUS, "+");
            break;
        case '-':
            addToken(TokenType::MINUS, "-");
            break;
        case '*':
            addToken(TokenType::STAR, "*");
            break;
        case '/':
            if (peek() == '/') {
                // Comentário de linha única, ignora até o fim da linha
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
            } else {
                addToken(TokenType::SLASH, "/");
            }
            break;
        case ':':
            if (peek() == '=') {
                advance();
                addToken(TokenType::ASSIGNMENT, ":=");
            } else {
                addToken(TokenType::COLON, ":");
            }
            break;
        case '=':
            if (peek() == '=') {
                advance();
                addToken(TokenType::EQUAL_EQUAL, "==");
            } else {
                throw std::runtime_error("Caractere não reconhecido: '=' na linha " + std::to_string(line));
            }
            break;
        case '!':
            if (peek() == '=') {
                advance();
                addToken(TokenType::NOT_EQUAL, "!=");
            } else {
                throw std::runtime_error("Caractere não reconhecido: '!' na linha " + std::to_string(line));
            }
            break;
        case '<':
            if (peek() == '=') {
                advance();
                addToken(TokenType::LESS_EQUAL, "<=");
            } else {
                addToken(TokenType::LESS, "<");
            }
            break;
        case '>':
            if (peek() == '=') {
                advance();
                addToken(TokenType::GREATER_EQUAL, ">=");
            } else {
                addToken(TokenType::GREATER, ">");
            }
            break;
        case '(':
            addToken(TokenType::LEFT_PAREN, "(");
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN, ")");
            break;
        case ';':
            addToken(TokenType::SEMICOLON, ";");
            break;
        case ',':
            addToken(TokenType::COMMA, ",");
            break;
        default:
            if (isdigit(static_cast<unsigned char>(c))) {
                // Número (inteiro ou real)
                current--;
                number();
            } else if (isalpha(static_cast<unsigned char>(c))) {
                // Identificador ou palavra-chave
                current--;
                identifier();
            } else {
                // Caractere não reconhecido
                int code = static_cast<unsigned char>(c);
                throw std::runtime_error("Caractere não reconhecido: código ASCII " + std::to_string(code) + " na linha " + std::to_string(line));
            }
            break;
    }
}

void Scanner::number() {
    std::string numStr;
    while (isdigit(peek())) {
        numStr += advance();
    }

    // Verifica se é um número real
    if (peek() == '.' && isdigit(peekNext())) {
        numStr += advance(); // Adiciona o ponto decimal
        while (isdigit(peek())) {
            numStr += advance();
        }
    }

    addToken(TokenType::NUMBER, numStr);
}

void Scanner::identifier() {
    std::string identifier;
    while (isalnum(peek()) || peek() == '_') {
        identifier += advance();
    }

    // Verifica se o identificador é uma palavra-chave
    auto keywordIt = keywords.find(identifier);
    if (keywordIt != keywords.end()) {
        addToken(keywordIt->second, identifier);
    } else {
        addToken(TokenType::IDENTIFIER, identifier);
    }
}
