// scanner.cpp

#include "scanner.hpp"
#include <cctype>
#include <unordered_map>
#include <stdexcept>

Scanner::Scanner(const std::string& source) : source(source) {}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line);
    return tokens;
}

void Scanner::scanToken() {
    skipWhitespaceAndComments();
    if (isAtEnd()) return;

    char c = advance();

    if (std::isalpha(c) || c == '_') {
        if (c == 'T' && peek() == '#') {
            advance(); // Consome '#'
            timeLiteral();
        } else {
            identifier();
        }
    } else if (std::isdigit(c)) {
        number();
    } else {
        switch (c) {
            case '+':
                addToken(TokenType::PLUS);
                break;
            case '-':
                addToken(TokenType::MINUS);
                break;
            case '*':
                addToken(TokenType::STAR);
                break;
            case '/':
                addToken(TokenType::SLASH);
                break;
            case '(':
                addToken(TokenType::LEFT_PAREN);
                break;
            case ')':
                addToken(TokenType::RIGHT_PAREN);
                break;
            case '{':
                addToken(TokenType::LEFT_BRACE);
                break;
            case '}':
                addToken(TokenType::RIGHT_BRACE);
                break;
            case '[':
                addToken(TokenType::LEFT_BRACKET);
                break;
            case ']':
                addToken(TokenType::RIGHT_BRACKET);
                break;
            case ',':
                addToken(TokenType::COMMA);
                break;
            case ';':
                addToken(TokenType::SEMICOLON);
                break;
            case ':':
                if (match('=')) {
                    addToken(TokenType::ASSIGNMENT);
                } else {
                    addToken(TokenType::COLON);
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
            case '=':
                addToken(TokenType::EQUAL_EQUAL); // Reconhece '=' como EQUAL_EQUAL
                break;
            case '!':
                if (match('=')) {
                    addToken(TokenType::NOT_EQUAL);
                } else {
                    throw std::runtime_error("Caractere inesperado '!' na linha " + std::to_string(line));
                }
                break;
            case '.':
                if (match('.')) {
                    addToken(TokenType::DOT_DOT);
                } else {
                    addToken(TokenType::DOT);
                }
                break;
            default:
                throw std::runtime_error("Caractere não reconhecido: '" + std::string(1, c) + "' na linha " + std::to_string(line));
        }
    }
}

void Scanner::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else if (c == '\n') {
            line++;
            advance();
        } else if (c == '(' && peekNext() == '*') {
            // Comentário de múltiplas linhas
            advance(); // Consome '('
            advance(); // Consome '*'
            while (!isAtEnd() && !(peek() == '*' && peekNext() == ')')) {
                if (peek() == '\n') line++;
                advance();
            }
            if (isAtEnd()) {
                throw std::runtime_error("Comentário não fechado antes do fim do arquivo.");
            }
            advance(); // Consome '*'
            advance(); // Consome ')'
        } else {
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

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Scanner::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Scanner::addToken(TokenType type) {
    addToken(type, "");
}

void Scanner::addToken(TokenType type, const std::string& text) {
    std::string lexeme = source.substr(start, current - start);
    tokens.emplace_back(type, lexeme, line);
}

void Scanner::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);

    // Converte para maiúsculas para tornar a linguagem case-insensitive
    std::string upperText;
    for (char c : text) {
        upperText += std::toupper(c);
    }

    static std::unordered_map<std::string, TokenType> keywords = {
        {"VAR", TokenType::VAR},
        {"VAR_INPUT", TokenType::VAR_INPUT},
        {"VAR_OUTPUT", TokenType::VAR_OUTPUT},
        {"END_VAR", TokenType::END_VAR},
        {"FUNCTION", TokenType::FUNCTION},
        {"END_FUNCTION", TokenType::END_FUNCTION},
        {"FUNCTION_BLOCK", TokenType::FUNCTION_BLOCK},
        {"END_FUNCTION_BLOCK", TokenType::END_FUNCTION_BLOCK},
        {"PROGRAM", TokenType::PROGRAM},
        {"END_PROGRAM", TokenType::END_PROGRAM},
        {"IF", TokenType::IF},
        {"THEN", TokenType::THEN},
        {"ELSE", TokenType::ELSE},
        {"ELSIF", TokenType::ELSIF},
        {"END_IF", TokenType::END_IF},
        {"WHILE", TokenType::WHILE},
        {"DO", TokenType::DO},
        {"END_WHILE", TokenType::END_WHILE},
        {"FOR", TokenType::FOR},
        {"TO", TokenType::TO},
        {"END_FOR", TokenType::END_FOR},
        {"RETURN", TokenType::RETURN},
        {"ARRAY", TokenType::ARRAY},
        {"OF", TokenType::OF},
        {"AND", TokenType::AND},
        {"OR", TokenType::OR},
        {"NOT", TokenType::NOT},
        {"TRUE", TokenType::TRUE},
        {"FALSE", TokenType::FALSE},
        // Tipos
        {"INTEGER", TokenType::INTEGER},
        {"REAL", TokenType::REAL},
        {"BOOLEAN", TokenType::BOOLEAN},
    };

    TokenType type = TokenType::IDENTIFIER;
    auto it = keywords.find(upperText);
    if (it != keywords.end()) {
        type = it->second;
    }
    addToken(type, text);
}

void Scanner::number() {
    while (std::isdigit(peek())) advance();

    // Verifica por parte fracionária
    if (peek() == '.' && std::isdigit(peekNext())) {
        advance(); // Consome o '.'
        while (std::isdigit(peek())) advance();
    }

    std::string text = source.substr(start, current - start);
    addToken(TokenType::NUMBER, text);
}

void Scanner::timeLiteral() {
    // Exemplo: T#5S

    // Captura o número após 'T#'
    std::string value;
    while (std::isdigit(peek())) {
        value += advance();
    }

    // Captura a unidade de tempo (S, M, H, etc.)
    std::string unit;
    while (std::isalpha(peek())) {
        unit += advance();
    }

    if (value.empty() || unit.empty()) {
        throw std::runtime_error("Literal de tempo inválido na linha " + std::to_string(line));
    }

    std::string lexeme = "T#" + value + unit;
    addToken(TokenType::TIME_LITERAL, lexeme);
}
