#include "scanner.hpp"
#include <cctype>
#include <unordered_map>
#include <iostream>

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
    {"AND", TokenType::AND},
    {"OR", TokenType::OR},
    {"NOT", TokenType::NOT},
    // Add other keywords as needed
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
            case '=':
                if (match('=')) {
                    addToken(TokenType::EQUAL_EQUAL);
                } else {
                    // Single '=' may not be valid; handle accordingly
                    // addToken(TokenType::EQUAL); // If you have an EQUAL token
                    throw std::runtime_error("Unexpected character '='");
                }
                break;
            case '!':
                if (match('=')) {
                    addToken(TokenType::NOT_EQUAL);
                } else {
                    addToken(TokenType::NOT); // If '!' is valid as NOT operator
                }
                break;
            case '<':
                if (match('=')) {
                    addToken(TokenType::LESS_EQUAL);
                } else {
                    addToken(TokenType::LESS);
                }
                break;
            case '>':
                if (match('=')) {
                    addToken(TokenType::GREATER_EQUAL);
                } else {
                    addToken(TokenType::GREATER);
                }
                break;
            default:
                // Ignore unrecognized characters or throw an error
                // throw std::runtime_error(std::string("Unexpected character '") + c + "'");
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
