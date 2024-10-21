// ast_optimizer.hpp

#ifndef AST_OPTIMIZER_HPP
#define AST_OPTIMIZER_HPP

#include "ast.hpp"
#include <memory>

// Classe responsável por otimizar a AST (ex: constant folding, dead code elimination)
class ASTOptimizer {
public:
    void optimize(Program* program);

private:
    // Métodos para otimizar diferentes tipos de nós
    std::unique_ptr<Statement> optimizeStatement(Statement* stmt);
    std::unique_ptr<Expression> optimizeExpression(Expression* expr);

    // Métodos específicos para otimizações
    std::unique_ptr<Expression> foldConstants(BinaryOperation* binOp);
    std::unique_ptr<Expression> simplifyTrivialOperations(BinaryOperation* binOp);
    std::unique_ptr<Statement> eliminateDeadCode(IfStatement* ifStmt);
    std::unique_ptr<Statement> eliminateDeadCode(WhileStatement* whileStmt);
};

#endif // AST_OPTIMIZER_HPP
