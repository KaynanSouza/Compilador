// ast.hpp

#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include "operator_type.hpp"

// Declarações antecipadas
class Expression;
class Statement;

// Representa um programa que contém uma lista de declarações
class Program {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void addStatement(std::unique_ptr<Statement> stmt) {
        statements.push_back(std::move(stmt));
    }
};

// Classe base para todas as declarações (statements)
class Statement {
public:
    virtual ~Statement() = default;
};

// Classe base para todas as expressões
class Expression {
public:
    virtual ~Expression() = default;
};

// Declaração de variável
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::string type;
    std::unique_ptr<Expression> initializer;

    VariableDeclaration(const std::string& name, const std::string& type, std::unique_ptr<Expression> initializer = nullptr)
        : name(name), type(type), initializer(std::move(initializer)) {}
};

// Declaração de array
class ArrayDeclaration : public Statement {
public:
    std::string name;
    std::string baseType;
    std::vector<std::pair<int, int>> dimensions;
    std::unique_ptr<Expression> initializer;

    ArrayDeclaration(const std::string& name, const std::string& baseType, const std::vector<std::pair<int, int>>& dimensions, std::unique_ptr<Expression> initializer = nullptr)
        : name(name), baseType(baseType), dimensions(dimensions), initializer(std::move(initializer)) {}
};

// Atribuição
class Assignment : public Statement {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    Assignment(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}
};

// Declaração de retorno
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> value)
        : value(std::move(value)) {}
};

// Declaração condicional (if)
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenBranch, std::unique_ptr<Statement> elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
};

// Declaração de loop while
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

// Declaração de loop for
class ForStatement : public Statement {
public:
    std::unique_ptr<Assignment> initializer;
    std::unique_ptr<Expression> endCondition;
    std::unique_ptr<Statement> body;

    ForStatement(std::unique_ptr<Assignment> initializer, std::unique_ptr<Expression> endCondition, std::unique_ptr<Statement> body)
        : initializer(std::move(initializer)), endCondition(std::move(endCondition)), body(std::move(body)) {}
};

// Declaração de função
class Function : public Statement {
public:
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;

    Function(const std::string& name) : name(name) {}
};

// Declaração de bloco de código
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}
};

// Expressões

// Identificador
class Identifier : public Expression {
public:
    std::string name;

    Identifier(const std::string& name) : name(name) {}
};

// Número (literal numérico)
class Number : public Expression {
public:
    double value;

    Number(double value) : value(value) {}
};

// Literal booleano
class BooleanLiteral : public Expression {
public:
    bool value;

    BooleanLiteral(bool value) : value(value) {}
};

// Operação binária
class BinaryOperation : public Expression {
public:
    OperatorType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryOperation(OperatorType op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

// Operação unária
class UnaryOperation : public Expression {
public:
    OperatorType op;
    std::unique_ptr<Expression> operand;

    UnaryOperation(OperatorType op, std::unique_ptr<Expression> operand)
        : op(op), operand(std::move(operand)) {}
};

// Chamada de função
class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments)
        : functionName(functionName), arguments(std::move(arguments)) {}
};

// Acesso a elemento de array
class ArrayAccess : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::vector<std::unique_ptr<Expression>> indices;

    ArrayAccess(std::unique_ptr<Expression> array, std::vector<std::unique_ptr<Expression>> indices)
        : array(std::move(array)), indices(std::move(indices)) {}
};

#endif // AST_HPP
