// parser_tests.cpp

#include "parser.hpp"
#include "scanner.hpp"
#include "semantic_analyzer.hpp"
#include "ast_optimizer.hpp"
#include "value.hpp" // Incluído para usar a definição da classe Value
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

using namespace std;

// Removemos a definição duplicada da classe 'Value'

// Classe Interpreter para executar a AST
class Interpreter : public Visitor {
public:
    void interpret(Program& program);

    // Implementação dos métodos visit
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

    Value visitIdentifier(Identifier& identifier) override;
    Value visitNumber(Number& number) override;
    Value visitBooleanLiteral(BooleanLiteral& boolLit) override;
    Value visitBinaryOperation(BinaryOperation& binOp) override;
    Value visitUnaryOperation(UnaryOperation& unaryOp) override;
    Value visitFunctionCall(FunctionCall& funcCall) override;
    Value visitArrayAccess(ArrayAccess& arrayAccess) override;

private:
    // Ambiente de execução
    vector<unordered_map<string, Value>> environment;
    std::unordered_map<std::string, Function*> functions;

    Value lastValue;

    void enterScope();
    void exitScope();
    void defineVariable(const std::string& name, const Value& value);
    Value getVariable(const std::string& name);
};

// Implementação da classe Interpreter

void Interpreter::interpret(Program& program) {
    // Inicializa o ambiente global
    environment.push_back({});
    // Coleta todas as funções definidas
    for (auto& stmt : program.statements) {
        if (auto func = dynamic_cast<Function*>(stmt.get())) { functions[func->name] = func; }
    }
    // Executa o programa principal
    for (auto& stmt : program.statements) {
        if (auto func = dynamic_cast<Function*>(stmt.get())) {
            if (func->name == "MainProgram") {
                func->accept(*this);
                break;
            }
        }
    }
    environment.pop_back();
}

void Interpreter::visitProgram(Program& program) {
    // Não faz nada aqui
}

void Interpreter::visitVariableDeclaration(VariableDeclaration& varDecl) {
    Value value;
    if (varDecl.initializer) { value = varDecl.initializer->accept(*this); }
    defineVariable(varDecl.name, value);
}

void Interpreter::visitArrayDeclaration(ArrayDeclaration& arrayDecl) {
    // Para simplificar, não implementaremos arrays neste exemplo
    defineVariable(arrayDecl.name, Value::Void());
}

void Interpreter::visitAssignment(Assignment& assignment) {
    Value value = assignment.right->accept(*this);
    if (auto identifier = dynamic_cast<Identifier*>(assignment.left.get())) {
        // Atribuição simples
        for (auto scopeIt = environment.rbegin(); scopeIt != environment.rend(); ++scopeIt) {
            auto it = scopeIt->find(identifier->name);
            if (it != scopeIt->end()) {
                it->second = value;
                return;
            }
        }
        throw std::runtime_error("Variável não definida: " + identifier->name);
    } else {
        throw std::runtime_error("Tipo de atribuição não suportado.");
    }
}

void Interpreter::visitReturnStatement(ReturnStatement& returnStmt) { lastValue = returnStmt.value->accept(*this); }

void Interpreter::visitIfStatement(IfStatement& ifStmt) {
    Value condition = ifStmt.condition->accept(*this);

    if (condition.getType() != Value::Type::BOOLEAN) { throw std::runtime_error("A condição do IF deve ser BOOLEAN."); }
    if (condition.getBoolValue()) { ifStmt.thenBranch->accept(*this); }
    else if (ifStmt.elseBranch) { ifStmt.elseBranch->accept(*this); }
}

void Interpreter::visitWhileStatement(WhileStatement& whileStmt) {
    while (true) {
        Value condition = whileStmt.condition->accept(*this);

        if (condition.getType() != Value::Type::BOOLEAN) { throw std::runtime_error("A condição do WHILE deve ser BOOLEAN."); }
        if (!condition.getBoolValue()) { break; }
        whileStmt.body->accept(*this);
    }
}

void Interpreter::visitForStatement(ForStatement& forStmt) {
    forStmt.initializer->accept(*this);
    Value endValue = forStmt.endCondition->accept(*this);
    if (endValue.getType() != Value::Type::INTEGER) { throw std::runtime_error("A condição final do FOR deve ser INTEGER."); }
    std::string varName;
    if (auto varAssign = dynamic_cast<Assignment*>(forStmt.initializer.get())) {
        if (auto identifier = dynamic_cast<Identifier*>(varAssign->left.get())) {
            varName = identifier->name;
        } else {
            throw std::runtime_error("A variável de controle do FOR deve ser um identificador simples.");
        }
    } else {
        throw std::runtime_error("Inicialização do FOR inválida.");
    }

    while (true) {
        Value currentValue = getVariable(varName);
        if (currentValue.getType() != Value::Type::INTEGER) {
            throw std::runtime_error("A variável de controle do FOR deve ser INTEGER.");
        }
        if (currentValue.getIntValue() > endValue.getIntValue()) {
            break;
        }
        forStmt.body->accept(*this);
        // Incrementa a variável de controle
        defineVariable(varName, Value(currentValue.getIntValue() + 1));
    }
}

void Interpreter::visitFunction(Function& function) {
    enterScope();
    for (auto& stmt : function.body) {
        stmt->accept(*this);
        if (function.returnType != "VOID" && lastValue.getType() != Value::Type::VOID) {
            // Retorno encontrado
            break;
        }
    }
    exitScope();
}

void Interpreter::visitBlockStatement(BlockStatement& blockStmt) {
    enterScope();
    for (auto& stmt : blockStmt.statements) {
        stmt->accept(*this);
    }
    exitScope();
}

void Interpreter::visitExpressionStatement(ExpressionStatement& exprStmt) {
    exprStmt.expression->accept(*this);
}

Value Interpreter::visitIdentifier(Identifier& identifier) {
    return getVariable(identifier.name);
}

Value Interpreter::visitNumber(Number& number) {
    if (std::floor(number.value) == number.value) {
        return Value(static_cast<int>(number.value));
    } else {
        return Value(number.value);
    }
}

Value Interpreter::visitBooleanLiteral(BooleanLiteral& boolLit) {
    return Value(boolLit.value);
}

Value Interpreter::visitBinaryOperation(BinaryOperation& binOp) {
    Value left = binOp.left->accept(*this);
    Value right = binOp.right->accept(*this);

    switch (binOp.op) {
        case OperatorType::ADD:
            if (left.getType() == Value::Type::INTEGER && right.getType() == Value::Type::INTEGER) {
                return Value(left.getIntValue() + right.getIntValue());
            } else if (left.getType() == Value::Type::REAL || right.getType() == Value::Type::REAL) {
                double l = (left.getType() == Value::Type::REAL) ? left.getRealValue() : left.getIntValue();
                double r = (right.getType() == Value::Type::REAL) ? right.getRealValue() : right.getIntValue();
                return Value(l + r);
            }
            break;
        case OperatorType::SUBTRACT:
            if (left.getType() == Value::Type::INTEGER && right.getType() == Value::Type::INTEGER) {
                return Value(left.getIntValue() - right.getIntValue());
            } else if (left.getType() == Value::Type::REAL || right.getType() == Value::Type::REAL) {
                double l = (left.getType() == Value::Type::REAL) ? left.getRealValue() : left.getIntValue();
                double r = (right.getType() == Value::Type::REAL) ? right.getRealValue() : right.getIntValue();
                return Value(l - r);
            }
            break;
        case OperatorType::MULTIPLY:
            if (left.getType() == Value::Type::INTEGER && right.getType() == Value::Type::INTEGER) {
                return Value(left.getIntValue() * right.getIntValue());
            } else if (left.getType() == Value::Type::REAL || right.getType() == Value::Type::REAL) {
                double l = (left.getType() == Value::Type::REAL) ? left.getRealValue() : left.getIntValue();
                double r = (right.getType() == Value::Type::REAL) ? right.getRealValue() : right.getIntValue();
                return Value(l * r);
            }
            break;
        case OperatorType::DIVIDE:
            if (left.getType() == Value::Type::INTEGER && right.getType() == Value::Type::INTEGER) {
                if (right.getIntValue() == 0) {
                    throw std::runtime_error("Divisão por zero.");
                }
                return Value(left.getIntValue() / right.getIntValue());
            } else if (left.getType() == Value::Type::REAL || right.getType() == Value::Type::REAL) {
                double l = (left.getType() == Value::Type::REAL) ? left.getRealValue() : left.getIntValue();
                double r = (right.getType() == Value::Type::REAL) ? right.getRealValue() : right.getIntValue();
                if (r == 0.0) {
                    throw std::runtime_error("Divisão por zero.");
                }
                return Value(l / r);
            }
            break;
        case OperatorType::LESS:
            return Value(left.getIntValue() < right.getIntValue());
        case OperatorType::LESS_EQUAL:
            return Value(left.getIntValue() <= right.getIntValue());
        case OperatorType::GREATER:
            return Value(left.getIntValue() > right.getIntValue());
        case OperatorType::GREATER_EQUAL:
            return Value(left.getIntValue() >= right.getIntValue());
        case OperatorType::EQUAL_EQUAL:
            return Value(left.getIntValue() == right.getIntValue());
        case OperatorType::NOT_EQUAL:
            return Value(left.getIntValue() != right.getIntValue());
        case OperatorType::AND:
            return Value(left.getBoolValue() && right.getBoolValue());
        case OperatorType::OR:
            return Value(left.getBoolValue() || right.getBoolValue());
        default:
            break;
    }

    throw std::runtime_error("Operação binária inválida.");
}

Value Interpreter::visitUnaryOperation(UnaryOperation& unaryOp) {
    Value operand = unaryOp.operand->accept(*this);

    switch (unaryOp.op) {
        case OperatorType::SUBTRACT:
            if (operand.getType() == Value::Type::INTEGER) {
                return Value(-operand.getIntValue());
            } else if (operand.getType() == Value::Type::REAL) {
                return Value(-operand.getRealValue());
            }
            break;
        case OperatorType::NOT:
            if (operand.getType() == Value::Type::BOOLEAN) {
                return Value(!operand.getBoolValue());
            }
            break;
        default:
            break;
    }

    throw std::runtime_error("Operação unária inválida.");
}

Value Interpreter::visitFunctionCall(FunctionCall& funcCall) {
    auto it = functions.find(funcCall.functionName);
    if (it == functions.end()) {
        throw std::runtime_error("Função não definida: " + funcCall.functionName);
    }
    Function* function = it->second;

    // Para simplificar, não trataremos parâmetros neste exemplo
    function->accept(*this);
    Value returnValue = lastValue;
    lastValue = Value::Void(); // Reseta o valor de retorno
    return returnValue;
}

Value Interpreter::visitArrayAccess(ArrayAccess& arrayAccess) {
    // Não implementado neste exemplo
    return Value::Void();
}

// Métodos auxiliares

void Interpreter::enterScope() {
    environment.push_back({});
}

void Interpreter::exitScope() {
    environment.pop_back();
}

void Interpreter::defineVariable(const std::string& name, const Value& value) {
    environment.back()[name] = value;
}

Value Interpreter::getVariable(const std::string& name) {
    for (auto scopeIt = environment.rbegin(); scopeIt != environment.rend(); ++scopeIt) {
        auto it = scopeIt->find(name);
        if (it != scopeIt->end()) {
            return it->second;
        }
    }
    throw std::runtime_error("Variável não definida: " + name);
}

void testParser() {
    std::string code = R"(
        (* Declaração de variáveis globais *)
        VAR_GLOBAL
            globalCount : INTEGER := 0;
            globalArray : ARRAY [1..5] OF INTEGER; (* Arrays não suportados neste exemplo *)
        END_VAR

        (* Definição de função *)
        FUNCTION MultiplyByTwo : INTEGER
        VAR_INPUT
            number : INTEGER;
        END_VAR
        MultiplyByTwo := number * 2;
        END_FUNCTION

        (* Programa Principal *)
        PROGRAM MainProgram
        VAR
            localCount : INTEGER := 0;
            result : INTEGER;
        END_VAR

        (* Usa a função *)
        result := MultiplyByTwo(10);  (* Deve retornar 20 *)

        (* Acessa variáveis globais *)
        globalCount := globalCount + 1;

        (* Uso correto de palavras reservadas *)
        IF localCount > 10 THEN
            globalCount := 0;
        ELSIF localCount = 5 THEN
            globalCount := 5;
        ELSE
            globalCount := globalCount + 1;
        END_IF

        END_PROGRAM

    )";

    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    try {
        auto ast = parser.parse();

        // Realiza a análise semântica
        SemanticAnalyzer analyzer;
        analyzer.setDebug(false); // Desativa o modo de depuração para limpeza de saída
        analyzer.analyze(ast.get());

        // Otimiza a AST
        ASTOptimizer optimizer;
        optimizer.optimize(ast.get());

        // Interpreta a AST
        Interpreter interpreter;
        interpreter.interpret(*ast);

        std::cout << "Execução concluída com sucesso." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Erro durante a análise ou execução: " << e.what() << std::endl;
    }
}

int main() {
    testParser();
    return 0;
}
