// ast.hpp

#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include "operator_type.hpp"

// Declarações antecipadas
class Visitor;
class Value; // Certifique-se de incluir ou declarar a classe 'Value'

// Classe base para todos os nós do AST
class Node {
public:
    virtual ~Node() = default;
};

// Classe base para todas as declarações (statements)
class Statement : public Node {
public:
    virtual ~Statement() = default;
    virtual void accept(Visitor& visitor) = 0; // Retorno 'void'
};

// Classe base para todas as expressões
class Expression : public Node {
public:
    virtual ~Expression() = default;
    virtual Value accept(Visitor& visitor) = 0; // Retorno 'Value'
};

// Definição das classes de declaração com o método accept

// Representa um programa que contém uma lista de declarações
class Program : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void addStatement(std::unique_ptr<Statement> stmt) {
        statements.push_back(std::move(stmt));
    }

    void accept(Visitor& visitor) override;
};

class VariableDeclaration : public Statement {
public:
    std::string name;
    std::string type;
    std::unique_ptr<Expression> initializer;

    VariableDeclaration(const std::string& name, const std::string& type, std::unique_ptr<Expression> initializer = nullptr)
        : name(name), type(type), initializer(std::move(initializer)) {}

    void accept(Visitor& visitor) override;
};

class ArrayDeclaration : public Statement {
public:
    std::string name;
    std::string baseType;
    std::vector<std::pair<int, int>> dimensions;
    std::unique_ptr<Expression> initializer;

    ArrayDeclaration(const std::string& name, const std::string& baseType, const std::vector<std::pair<int, int>>& dimensions, std::unique_ptr<Expression> initializer = nullptr)
        : name(name), baseType(baseType), dimensions(dimensions), initializer(std::move(initializer)) {}

    void accept(Visitor& visitor) override;
};

class Assignment : public Statement {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    Assignment(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}

    void accept(Visitor& visitor) override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> value)
        : value(std::move(value)) {}

    void accept(Visitor& visitor) override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenBranch, std::unique_ptr<Statement> elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(Visitor& visitor) override;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(Visitor& visitor) override;
};

class ForStatement : public Statement {
public:
    std::unique_ptr<Assignment> initializer;
    std::unique_ptr<Expression> endCondition;
    std::unique_ptr<Statement> body;

    ForStatement(std::unique_ptr<Assignment> initializer, std::unique_ptr<Expression> endCondition, std::unique_ptr<Statement> body)
        : initializer(std::move(initializer)), endCondition(std::move(endCondition)), body(std::move(body)) {}

    void accept(Visitor& visitor) override;
};

class Function : public Statement {
public:
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;

    Function(const std::string& name) : name(name) {}

    void accept(Visitor& visitor) override;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}

    void accept(Visitor& visitor) override;
};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;

    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}

    void accept(Visitor& visitor) override;
};

// Definição das classes de expressão com o método accept

class Identifier : public Expression {
public:
    std::string name;

    Identifier(const std::string& name) : name(name) {}

    Value accept(Visitor& visitor) override;
};

class Number : public Expression {
public:
    double value;

    Number(double value) : value(value) {}

    Value accept(Visitor& visitor) override;
};

class BooleanLiteral : public Expression {
public:
    bool value;

    BooleanLiteral(bool value) : value(value) {}

    Value accept(Visitor& visitor) override;
};

class BinaryOperation : public Expression {
public:
    OperatorType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryOperation(OperatorType op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    Value accept(Visitor& visitor) override;
};

class UnaryOperation : public Expression {
public:
    OperatorType op;
    std::unique_ptr<Expression> operand;

    UnaryOperation(OperatorType op, std::unique_ptr<Expression> operand)
        : op(op), operand(std::move(operand)) {}

    Value accept(Visitor& visitor) override;
};

class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments)
        : functionName(functionName), arguments(std::move(arguments)) {}

    Value accept(Visitor& visitor) override;
};

class ArrayAccess : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::vector<std::unique_ptr<Expression>> indices;

    ArrayAccess(std::unique_ptr<Expression> array, std::vector<std::unique_ptr<Expression>> indices)
        : array(std::move(array)), indices(std::move(indices)) {}

    Value accept(Visitor& visitor) override;
};

// Definição da classe Visitor
class Visitor {
public:
    virtual ~Visitor() = default;

    // Métodos para 'Statement' retornam 'void'
    virtual void visitProgram(Program& program) = 0;
    virtual void visitVariableDeclaration(VariableDeclaration& varDecl) = 0;
    virtual void visitArrayDeclaration(ArrayDeclaration& arrayDecl) = 0;
    virtual void visitAssignment(Assignment& assignment) = 0;
    virtual void visitReturnStatement(ReturnStatement& returnStmt) = 0;
    virtual void visitIfStatement(IfStatement& ifStmt) = 0;
    virtual void visitWhileStatement(WhileStatement& whileStmt) = 0;
    virtual void visitForStatement(ForStatement& forStmt) = 0;
    virtual void visitFunction(Function& function) = 0;
    virtual void visitBlockStatement(BlockStatement& blockStmt) = 0;
    virtual void visitExpressionStatement(ExpressionStatement& exprStmt) = 0;

    // Métodos para 'Expression' retornam 'Value'
    virtual Value visitIdentifier(Identifier& identifier) = 0;
    virtual Value visitNumber(Number& number) = 0;
    virtual Value visitBooleanLiteral(BooleanLiteral& boolLit) = 0;
    virtual Value visitBinaryOperation(BinaryOperation& binOp) = 0;
    virtual Value visitUnaryOperation(UnaryOperation& unaryOp) = 0;
    virtual Value visitFunctionCall(FunctionCall& funcCall) = 0;
    virtual Value visitArrayAccess(ArrayAccess& arrayAccess) = 0;
};

#endif // AST_HPP
