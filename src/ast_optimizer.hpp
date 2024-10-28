// ast_optimizer.hpp

#ifndef AST_OPTIMIZER_HPP
#define AST_OPTIMIZER_HPP

#include "ast.hpp"
#include <memory>

// Otimizador de AST (Árvore Sintática Abstrata)
class ASTOptimizer {
public:
    // Otimiza o programa inteiro
    void optimize(Program* program);

private:
    // Otimiza uma declaração
    std::unique_ptr<Statement> optimizeStatement(Statement* stmt);

    // Otimiza uma expressão
    std::unique_ptr<Expression> optimizeExpression(Expression* expr);
};

#endif // AST_OPTIMIZER_HPP
