// scanner.cpp
#include "scanner.hpp"
#include <cctype>
#include <unordered_map>
#include <iostream> // Adicionado para logs

static std::unordered_map<std::string, TokenType> keywords = {
    {"VAR", TokenType::VAR},
    {"END_VAR", TokenType::END_VAR},
    {"VAR_INPUT", TokenType::VAR_INPUT},
    {"FUNCTION", TokenType::FUNCTION},
    {"END_FUNCTION", TokenType::END_FUNCTION},
    {"PROGRAM", TokenType::PROGRAM},
    {"END_PROGRAM", TokenType::END_PROGRAM},
    {"REAL", TokenType::REAL},
    {"INTEGER", TokenType::INTEGER},
    {"RETURN", TokenType::RETURN},
    // Adicione outras palavras-chave conforme necessário
};

Scanner::Scanner(const std::string& sourceCode)
    : source(sourceCode) {}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        skipWhitespace();
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::EOF_TOKEN, "");
    return tokens;
}

void Scanner::scanToken() {
    if (isAtEnd()) return;

    char c = advance();

    if (std::isalpha(c) || c == '_') {
        identifier();
    } else if (std::isdigit(c)) {
        number();
    } else {
        switch (c) {
            case ':':
                if (match('=')) {
                    addToken(TokenType::ASSIGNMENT);
                } else {
                    addToken(TokenType::COLON);
                }
                break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case ',': addToken(TokenType::COMMA); break;
            case '(': addToken(TokenType::LEFT_PAREN); break;
            case ')': addToken(TokenType::RIGHT_PAREN); break;
            case '+': addToken(TokenType::PLUS); break;
            case '-': addToken(TokenType::MINUS); break;
            case '*': addToken(TokenType::STAR); break;
            case '/':
                if (match('/')) {
                    skipComment();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            default:
                // Ignora caracteres não reconhecidos
                break;
        }
    }
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

void Scanner::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && peekNext() == '/') {
            skipComment();
        } else {
            break;
        }
    }
}

void Scanner::skipComment() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void Scanner::addToken(TokenType type) {
    addToken(type, source.substr(start, current - start));
}

void Scanner::addToken(TokenType type, const std::string& text) {
    tokens.emplace_back(type, text);

    // Log do token
    std::cout << "Scanner: Adicionado token '" << text << "' do tipo " << static_cast<int>(type) << std::endl;
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

void Scanner::identifier() {
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

void Scanner::number() {
    while (!isAtEnd() && std::isdigit(peek())) {
        advance();
    }
    if (!isAtEnd() && peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (!isAtEnd() && std::isdigit(peek())) {
            advance();
        }
    }
    std::string text = source.substr(start, current - start);
    addToken(TokenType::NUMBER, text);
}
