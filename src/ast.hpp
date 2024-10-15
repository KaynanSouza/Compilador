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
    char op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryOperation(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

// Function call
class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments)
        : functionName(functionName), arguments(std::move(arguments)) {}
};

#endif // AST_HPP
