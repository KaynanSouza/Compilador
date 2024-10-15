#include "parser.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens) {}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        auto declaration = parseDeclaration();
        if (declaration) {
            program->addStatement(std::move(declaration));
        }
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseDeclaration() {
    if (match({TokenType::FUNCTION, TokenType::PROGRAM})) {
        return parseFunction();
    } else {
        return parseStatement();
    }
}

std::unique_ptr<Function> Parser::parseFunction() {
    TokenType funcType = previous().type;
    std::string name = consume(TokenType::IDENTIFIER, "Expected function or program name").lexeme;

    auto function = std::make_unique<Function>(name);

    if (funcType == TokenType::FUNCTION && match(TokenType::COLON)) {
        consume({TokenType::REAL, TokenType::INTEGER}, "Expected return type after ':'");
    }

    while (!isAtEnd() && !check(TokenType::END_FUNCTION) && !check(TokenType::END_PROGRAM)) {
        auto stmt = parseStatement();
        if (stmt) {
            function->body.push_back(std::move(stmt));
        }
    }

    if (funcType == TokenType::FUNCTION) {
        consume(TokenType::END_FUNCTION, "Expected END_FUNCTION");
    } else {
        consume(TokenType::END_PROGRAM, "Expected END_PROGRAM");
    }

    return function;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (match({TokenType::VAR, TokenType::VAR_INPUT})) {
        return parseVariableDeclaration();
    } else if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    } else if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    } else if (match(TokenType::SEMICOLON)) {
        // Empty statement, skip
        return nullptr;
    } else {
        throw std::runtime_error("Unexpected token: " + peek().lexeme);
    }
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    std::vector<std::unique_ptr<VariableDeclaration>> declarations;

    while (!isAtEnd() && !check(TokenType::END_VAR)) {
        std::string name = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
        consume(TokenType::COLON, "Expected ':' after variable name");
        std::string type = consume({TokenType::REAL, TokenType::INTEGER}, "Expected type after ':'").lexeme;
        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

        declarations.push_back(std::make_unique<VariableDeclaration>(name, type));
    }

    consume(TokenType::END_VAR, "Expected END_VAR");

    // Wrap declarations in a block statement
    std::vector<std::unique_ptr<Statement>> stmtDeclarations;
    for (auto& decl : declarations) {
        stmtDeclarations.push_back(std::move(decl));
    }

    return std::make_unique<BlockStatement>(std::move(stmtDeclarations));
}

std::unique_ptr<Statement> Parser::parseAssignment() {
    std::string name = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
    consume(TokenType::ASSIGNMENT, "Expected ':=' in assignment");
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    return std::make_unique<Assignment>(
        std::make_unique<Identifier>(name),
        std::move(value)
    );
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    auto expr = parseTerm();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        char op = previous().lexeme[0];
        auto right = parseTerm();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match({TokenType::STAR, TokenType::SLASH})) {
        char op = previous().lexeme[0];
        auto right = parseFactor();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    if (match(TokenType::NUMBER)) {
        double value = std::stod(previous().lexeme);
        return std::make_unique<Number>(value);
    } else if (match(TokenType::IDENTIFIER)) {
        std::string name = previous().lexeme;
        if (match(TokenType::LEFT_PAREN)) {
            std::vector<std::unique_ptr<Expression>> arguments;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RIGHT_PAREN, "Expected ')' after function arguments");
            return std::make_unique<FunctionCall>(name, std::move(arguments));
        } else {
            return std::make_unique<Identifier>(name);
        }
    } else if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
        return expr;
    } else {
        throw std::runtime_error("Expected expression");
    }
}

// Utility methods

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " at '" + peek().lexeme + "'");
}

Token Parser::consume(std::initializer_list<TokenType> types, const std::string& message) {
    for (TokenType type : types) {
        if (check(type)) {
            return advance();
        }
    }
    throw std::runtime_error(message + " at '" + peek().lexeme + "'");
}
