// semantic_analyzer.hpp

#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ast.hpp"
#include "symbol_table.hpp"
#include "value.hpp" // Adicionado

class SemanticAnalyzer : public Visitor {
public:
    SemanticAnalyzer();

    void analyze(Program* program);

    // Métodos do Visitor para Statements
    void visitProgram(Program& program) override;
    void visitVariableDeclaration(VariableDeclaration& varDecl) override;
    void visitArrayDeclaration(ArrayDeclaration& arrayDecl) override;
    void visitAssignment(Assignment& assignment) override;
    void visitReturnStatement(ReturnStatement& returnStmt) override;
    void visitIfStatement(IfStatement& ifStmt) override;
    void visitWhileStatement(WhileStatement& whileStmt) override;
    void visitForStatement(ForStatement& forStmt) override;
    void visitFunction(Function& function) override;
    void visitBlockStatement(BlockStatement& blockStmt) override;
    void visitExpressionStatement(ExpressionStatement& exprStmt) override;

    // Métodos do Visitor para Expressions
    Value visitIdentifier(Identifier& identifier) override;
    Value visitNumber(Number& number) override;
    Value visitBooleanLiteral(BooleanLiteral& boolLit) override;
    Value visitBinaryOperation(BinaryOperation& binOp) override;
    Value visitUnaryOperation(UnaryOperation& unaryOp) override;
    Value visitFunctionCall(FunctionCall& funcCall) override;
    Value visitArrayAccess(ArrayAccess& arrayAccess) override;

    // Método para definir o modo de depuração
    void setDebug(bool value);

private:
    SymbolTable symbolTable;
    std::string currentFunctionReturnType;
    bool debug = false;

    // Método auxiliar para obter o tipo a partir de um Value
    std::string getTypeFromValue(const Value& value);
};

#endif // SEMANTIC_ANALYZER_HPP
