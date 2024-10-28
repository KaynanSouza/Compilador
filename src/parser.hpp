// parser.hpp

#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include "token.hpp"
#include "ast.hpp"
#include "operator_type.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    std::unique_ptr<Program> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Métodos auxiliares
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    Token consume(std::initializer_list<TokenType> types, const std::string& message);

    // Métodos de parsing
    std::unique_ptr<Statement> parseDeclaration();
    std::unique_ptr<Function> parseFunction();
    std::unique_ptr<Statement> parseStatement();
    std::vector<std::unique_ptr<Statement>> parseVariableDeclaration();

    std::unique_ptr<Statement> parseAssignment();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<Statement> parseIfStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<Statement> parseForStatement();

    // Métodos de parsing de expressões
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseLogicalOr();
    std::unique_ptr<Expression> parseLogicalAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();

    // Auxiliar para operadores
    OperatorType getOperatorType(TokenType type);
};

#endif // PARSER_HPP
