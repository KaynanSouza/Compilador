#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <memory>
#include <vector>

class Identifier;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Base class for statements
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

// Base class for expressions
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

// Variable declaration statement
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::string type;

    VariableDeclaration(const std::string& name, const std::string& type)
        : name(name), type(type) {}
};

// Assignment statement
class Assignment : public Statement {
public:
    std::unique_ptr<Identifier> left;
    std::unique_ptr<Expression> right;

    Assignment(std::unique_ptr<Identifier> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}
};

// Return statement
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> value)
        : value(std::move(value)) {}
};

// Block of statements
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}
};

// Function or program
class Function : public Statement {
public:
    std::string name;
    std::string returnType; // Added return type
    std::vector<std::unique_ptr<Statement>> body;

    Function(const std::string& name)
        : name(name) {}
};

// Program (root of AST)
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void addStatement(std::unique_ptr<Statement> statement) {
        statements.push_back(std::move(statement));
    }
};

// Expressions

// Identifier
class Identifier : public Expression {
public:
    std::string name;

    Identifier(const std::string& name)
        : name(name) {}
};

// Number literal
class Number : public Expression {
public:
    double value;

    Number(double value)
        : value(value) {}
};

// Binary operation
class BinaryOperation : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryOperation(const std::string& op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

// Unary operation
class UnaryOperation : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> operand;

    UnaryOperation(const std::string& op, std::unique_ptr<Expression> operand)
        : op(op), operand(std::move(operand)) {}
};

// Function call
class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments)
        : functionName(functionName), arguments(std::move(arguments)) {}
};

// If statement
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch; // Optional

    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement> elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
};

// While statement
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

// For statement
class ForStatement : public Statement {
public:
    std::unique_ptr<Assignment> initializer;
    std::unique_ptr<Expression> endCondition;
    std::unique_ptr<Statement> body;

    ForStatement(std::unique_ptr<Assignment> initializer,
                 std::unique_ptr<Expression> endCondition,
                 std::unique_ptr<Statement> body)
        : initializer(std::move(initializer)), endCondition(std::move(endCondition)), body(std::move(body)) {}
};

#endif // AST_HPP
