// parser.hpp

#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include "token.hpp"
#include "ast.hpp"

// Classe responsável por analisar a lista de tokens e gerar a AST.
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();

private:
    const std::vector<Token>& tokens;  // Lista de tokens a serem analisados
    size_t current = 0;                // Posição atual na lista de tokens

    // Métodos auxiliares de navegação nos tokens
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    Token consume(std::initializer_list<TokenType> types, const std::string& message);

    // Métodos de análise sintática
    std::unique_ptr<Statement> parseDeclaration();
    std::unique_ptr<Function> parseFunction();
    std::unique_ptr<Statement> parseStatement();
    std::vector<std::unique_ptr<Statement>> parseVariableDeclaration();
    std::unique_ptr<Statement> parseAssignment();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<Statement> parseIfStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<Statement> parseForStatement();

    // Métodos de análise de expressões
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseLogicalOr();
    std::unique_ptr<Expression> parseLogicalAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();
};

#endif // PARSER_HPP
