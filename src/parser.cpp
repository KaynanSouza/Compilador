// parser.cpp

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
    std::string name = consume(TokenType::IDENTIFIER, "Esperado nome da função ou programa").lexeme;

    auto function = std::make_unique<Function>(name);

    if (funcType == TokenType::FUNCTION && match(TokenType::COLON)) {
        Token returnTypeToken = consume({TokenType::REAL, TokenType::INTEGER}, "Esperado tipo de retorno após ':'");
        function->returnType = returnTypeToken.lexeme;
    }

    while (!isAtEnd() && !check(TokenType::END_FUNCTION) && !check(TokenType::END_PROGRAM)) {
        auto stmt = parseStatement();
        if (stmt) {
            function->body.push_back(std::move(stmt));
        }
    }

    if (funcType == TokenType::FUNCTION) {
        consume(TokenType::END_FUNCTION, "Esperado END_FUNCTION");
    } else {
        consume(TokenType::END_PROGRAM, "Esperado END_PROGRAM");
    }

    return function;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (match({TokenType::VAR, TokenType::VAR_INPUT})) {
        return parseVariableDeclaration();
    } else if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    } else if (match(TokenType::IF)) {
        return parseIfStatement();
    } else if (match(TokenType::WHILE)) {
        return parseWhileStatement();
    } else if (match(TokenType::FOR)) {
        return parseForStatement();
    } else if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    } else if (match(TokenType::SEMICOLON)) {
        // Declaração vazia, ignora
        return nullptr;
    } else {
        throw std::runtime_error("Token inesperado: " + peek().lexeme);
    }
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    std::vector<std::unique_ptr<VariableDeclaration>> declarations;

    while (!isAtEnd() && !check(TokenType::END_VAR)) {
        std::string name = consume(TokenType::IDENTIFIER, "Esperado nome da variável").lexeme;
        consume(TokenType::COLON, "Esperado ':' após o nome da variável");
        std::string type = consume({TokenType::REAL, TokenType::INTEGER}, "Esperado tipo após ':'").lexeme;
        consume(TokenType::SEMICOLON, "Esperado ';' após a declaração da variável");

        declarations.push_back(std::make_unique<VariableDeclaration>(name, type));
    }

    consume(TokenType::END_VAR, "Esperado END_VAR");

    // Envolve as declarações em um bloco
    std::vector<std::unique_ptr<Statement>> stmtDeclarations;
    for (auto& decl : declarations) {
        stmtDeclarations.push_back(std::move(decl));
    }

    return std::make_unique<BlockStatement>(std::move(stmtDeclarations));
}

std::unique_ptr<Statement> Parser::parseAssignment() {
    std::string name = consume(TokenType::IDENTIFIER, "Esperado nome da variável").lexeme;
    consume(TokenType::ASSIGNMENT, "Esperado ':=' na atribuição");
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Esperado ';' após a atribuição");
    return std::make_unique<Assignment>(
        std::make_unique<Identifier>(name),
        std::move(value)
    );
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Esperado ';' após o retorno");
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    consume(TokenType::LEFT_PAREN, "Esperado '(' após 'IF'");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Esperado ')' após a condição");
    consume(TokenType::THEN, "Esperado 'THEN' após a condição");
    auto thenBranch = parseStatement();
    std::unique_ptr<Statement> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = parseStatement();
    }
    consume(TokenType::END_IF, "Esperado 'END_IF'");
    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::parseWhileStatement() {
    consume(TokenType::LEFT_PAREN, "Esperado '(' após 'WHILE'");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Esperado ')' após a condição");
    consume(TokenType::DO, "Esperado 'DO' após a condição");
    auto body = parseStatement();
    consume(TokenType::END_WHILE, "Esperado 'END_WHILE'");
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::parseForStatement() {
    std::string varName = consume(TokenType::IDENTIFIER, "Esperado nome da variável de loop").lexeme;
    consume(TokenType::ASSIGNMENT, "Esperado ':=' na inicialização do loop");
    auto initValue = parseExpression();
    auto initializer = std::make_unique<Assignment>(std::make_unique<Identifier>(varName), std::move(initValue));

    consume(TokenType::TO, "Esperado 'TO' após a inicialização do loop");
    auto endCondition = parseExpression();

    consume(TokenType::DO, "Esperado 'DO' após a condição do loop");

    auto body = parseStatement();

    consume(TokenType::END_FOR, "Esperado 'END_FOR'");

    // Chamando o construtor com 3 argumentos
    return std::make_unique<ForStatement>(std::move(initializer), std::move(endCondition), std::move(body));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseLogicalOr();
}

std::unique_ptr<Expression> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::OR)) {
        std::string op = previous().lexeme;
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AND)) {
        std::string op = previous().lexeme;
        auto right = parseEquality();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();
    while (match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
        std::string op = previous().lexeme;
        auto right = parseComparison();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseTerm();
    while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL})) {
        std::string op = previous().lexeme;
        auto right = parseTerm();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        auto right = parseFactor();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto expr = parseUnary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        std::string op = previous().lexeme;
        auto right = parseUnary();
        expr = std::make_unique<BinaryOperation>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        auto right = parseUnary();
        return std::make_unique<UnaryOperation>(op, std::move(right));
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
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
            consume(TokenType::RIGHT_PAREN, "Esperado ')' após os argumentos da função");
            return std::make_unique<FunctionCall>(name, std::move(arguments));
        } else {
            return std::make_unique<Identifier>(name);
        }
    } else if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Esperado ')'");
        return expr;
    } else {
        throw std::runtime_error("Esperado expressão em '" + peek().lexeme + "'");
    }
}

// Métodos utilitários

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
    throw std::runtime_error(message + " em '" + peek().lexeme + "'");
}

Token Parser::consume(std::initializer_list<TokenType> types, const std::string& message) {
    for (TokenType type : types) {
        if (check(type)) {
            return advance();
        }
    }
    throw std::runtime_error(message + " em '" + peek().lexeme + "'");
}
