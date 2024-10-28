// semantic_analyzer.hpp

#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ast.hpp"
#include "symbol_table.hpp"
#include <string>

class SemanticAnalyzer {
public:
    void analyze(Program* program);

    // Debug flag
    bool debug = false;

private:
    SymbolTable symbolTable;

    void visitProgram(Program* program);
    void visitFunction(Function* function);
    void visitVariableDeclaration(VariableDeclaration* varDecl);
    void visitArrayDeclaration(ArrayDeclaration* arrayDecl);
    void visitAssignment(Assignment* assignment);
    void visitReturnStatement(ReturnStatement* returnStmt);
    void visitIfStatement(IfStatement* ifStmt);
    void visitWhileStatement(WhileStatement* whileStmt);
    void visitForStatement(ForStatement* forStmt);
    void visitStatement(Statement* stmt);
    void visitExpression(Expression* expr);
    std::string visitExpressionAndGetType(Expression* expr);

    std::string currentFunctionReturnType;
};

#endif // SEMANTIC_ANALYZER_HPP
