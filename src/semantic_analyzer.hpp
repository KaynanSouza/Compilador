// semantic_analyzer.hpp

#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ast.hpp"
#include "symbol_table.hpp"
#include <string>

// Classe responsável por realizar a análise semântica da AST.
class SemanticAnalyzer {
public:
    void analyze(Program* program);

private:
    SymbolTable symbolTable;  // Tabela de símbolos para rastrear variáveis e funções

    // Métodos para visitar diferentes tipos de nós na AST
    void visitProgram(Program* program);
    void visitFunction(Function* function);
    void visitBlockStatement(BlockStatement* block);
    void visitVariableDeclaration(VariableDeclaration* varDecl);
    void visitAssignment(Assignment* assignment);
    void visitReturnStatement(ReturnStatement* returnStmt);
    void visitIfStatement(IfStatement* ifStmt);
    void visitWhileStatement(WhileStatement* whileStmt);
    void visitForStatement(ForStatement* forStmt);
    void visitExpression(Expression* expr);
    std::string visitExpressionAndGetType(Expression* expr);

    void visitStatement(Statement* stmt);

    // Variável para armazenar o tipo de retorno da função atual
    std::string currentFunctionReturnType;
};

#endif // SEMANTIC_ANALYZER_HPP
