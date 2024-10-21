// ast.hpp

#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

// Classe base para todos os nós da AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Classe base para declarações (statements)
class Statement : public ASTNode {
};

// Classe base para expressões
class Expression : public ASTNode {
};

// Classes para diferentes tipos de expressões

// Representa um número (inteiro ou real)
class Number : public Expression {
public:
    double value;

    Number(double value) : value(value) {}
};

// Representa um literal booleano (TRUE ou FALSE)
class BooleanLiteral : public Expression {
public:
    bool value;

    BooleanLiteral(bool value) : value(value) {}
};

// Representa um identificador (nome de variável ou função)
class Identifier : public Expression {
public:
    std::string name;

    Identifier(const std::string& name) : name(name) {}
};

// Representa uma operação binária (ex: a + b)
class BinaryOperation : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryOperation(const std::string& op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

// Representa uma operação unária (ex: -a, NOT a)
class UnaryOperation : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> operand;

    UnaryOperation(const std::string& op, std::unique_ptr<Expression> operand)
        : op(op), operand(std::move(operand)) {}
};

// Representa uma chamada de função (ex: foo(a, b))
class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments)
        : functionName(functionName), arguments(std::move(arguments)) {}
};

// Classes para diferentes tipos de declarações

// Representa o programa inteiro
class Program : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    void addStatement(std::unique_ptr<Statement> stmt) {
        statements.push_back(std::move(stmt));
    }
};

// Representa uma função ou programa
class Function : public Statement {
public:
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;

    Function(const std::string& name)
        : name(name), returnType("VOID") {}
};

// Representa a declaração de uma variável
class VariableDeclaration : public Statement {
public:
    std::string name;
    std::string type;

    VariableDeclaration(const std::string& name, const std::string& type)
        : name(name), type(type) {}
};

// Representa uma atribuição (ex: a := b + c)
class Assignment : public Statement {
public:
    std::unique_ptr<Identifier> left;
    std::unique_ptr<Expression> right;

    Assignment(std::unique_ptr<Identifier> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}
};

// Representa uma instrução RETURN
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> value)
        : value(std::move(value)) {}
};

// Representa uma instrução IF
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}
};

// Representa uma instrução WHILE
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> condition,
                   std::unique_ptr<Statement> body)
        : condition(std::move(condition)),
          body(std::move(body)) {}
};

// Representa uma instrução FOR
class ForStatement : public Statement {
public:
    std::unique_ptr<Assignment> initializer;   // Inicialização do loop (ex: i := 0)
    std::unique_ptr<Expression> endCondition;  // Condição de término (ex: i < 10)
    std::unique_ptr<Statement> body;           // Corpo do loop

    ForStatement(std::unique_ptr<Assignment> initializer,
                 std::unique_ptr<Expression> endCondition,
                 std::unique_ptr<Statement> body)
        : initializer(std::move(initializer)),
          endCondition(std::move(endCondition)),
          body(std::move(body)) {}
};

// Representa um bloco de declarações (ex: corpo de funções, blocos de código)
class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}
};

#endif // AST_HPP
