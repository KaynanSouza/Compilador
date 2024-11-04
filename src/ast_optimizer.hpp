// ast_optimizer.hpp

#ifndef AST_OPTIMIZER_HPP
#define AST_OPTIMIZER_HPP

#include "ast.hpp"

class ASTOptimizer {
public:
    void optimize(Program* program);

private:
    void optimizeExpression(std::unique_ptr<Expression>& expr);

    void optimizeStatement(std::unique_ptr<Statement>& stmt);

    // Métodos de otimização para cada tipo de nó
    void optimizeProgram(Program* program);
    void optimizeVariableDeclaration(VariableDeclaration* varDecl);
    void optimizeArrayDeclaration(ArrayDeclaration* arrayDecl);
    void optimizeAssignment(Assignment* assignment);
    void optimizeReturnStatement(ReturnStatement* returnStmt);
    void optimizeIfStatement(IfStatement* ifStmt);
    void optimizeWhileStatement(WhileStatement* whileStmt);
    void optimizeForStatement(ForStatement* forStmt);
    void optimizeFunction(Function* function);
    void optimizeBlockStatement(BlockStatement* blockStmt);
    void optimizeExpressionStatement(ExpressionStatement* exprStmt);
};

#endif // AST_OPTIMIZER_HPP
