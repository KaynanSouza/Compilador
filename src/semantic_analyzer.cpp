// semantic_analyzer.cpp

#include "semantic_analyzer.hpp"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include "operator_type.hpp"
#include "value.hpp" // Adicionado

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(Program* program) {
    program->accept(*this);
}

void SemanticAnalyzer::visitProgram(Program& program) {
    for (auto& stmt : program.statements) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visitFunction(Function& function) {
    // Registra a função na tabela de símbolos
    if (symbolTable.currentScope().find(function.name) != symbolTable.currentScope().end()) {
        throw std::runtime_error("Função '" + function.name + "' já foi declarada.");
    }
    symbolTable.define(function.name, function.returnType, SymbolType::FUNCTION);

    symbolTable.enterScope();
    currentFunctionReturnType = function.returnType;

    for (auto& stmt : function.body) {
        stmt->accept(*this);
    }

    symbolTable.exitScope();
}

void SemanticAnalyzer::visitVariableDeclaration(VariableDeclaration& varDecl) {
    if (symbolTable.currentScope().find(varDecl.name) != symbolTable.currentScope().end()) {
        throw std::runtime_error("Variável '" + varDecl.name + "' já foi declarada neste escopo.");
    }
    symbolTable.define(varDecl.name, varDecl.type, SymbolType::VARIABLE);

    if (varDecl.initializer) {
        Value initValue = varDecl.initializer->accept(*this);
        std::string initType = getTypeFromValue(initValue);
        if (initType != varDecl.type) {
            throw std::runtime_error("Tipo do inicializador '" + initType + "' não corresponde ao tipo da variável '" + varDecl.type + "'.");
        }
    }
}

void SemanticAnalyzer::visitArrayDeclaration(ArrayDeclaration& arrayDecl) {
    if (symbolTable.currentScope().find(arrayDecl.name) != symbolTable.currentScope().end()) {
        throw std::runtime_error("Array '" + arrayDecl.name + "' já foi declarado neste escopo.");
    }
    Symbol arraySymbol(arrayDecl.name, arrayDecl.baseType, SymbolType::ARRAY);
    arraySymbol.dimensions = arrayDecl.dimensions;
    symbolTable.currentScope()[arrayDecl.name] = arraySymbol;

    if (arrayDecl.initializer) {
        // Implementar verificação de tipos para inicializadores de arrays, se necessário
    }
}

void SemanticAnalyzer::visitAssignment(Assignment& assignment) {
    Value leftValue = assignment.left->accept(*this);
    Value rightValue = assignment.right->accept(*this);

    std::string leftType = getTypeFromValue(leftValue);
    std::string rightType = getTypeFromValue(rightValue);

    if (leftType != rightType) {
        throw std::runtime_error("Tipos incompatíveis na atribuição: '" + leftType + "' e '" + rightType + "'.");
    }
}

void SemanticAnalyzer::visitReturnStatement(ReturnStatement& returnStmt) {
    Value returnValue = returnStmt.value->accept(*this);
    std::string returnType = getTypeFromValue(returnValue);

    if (returnType != currentFunctionReturnType) {
        throw std::runtime_error("Tipo de retorno '" + returnType + "' não corresponde ao tipo de retorno da função '" + currentFunctionReturnType + "'.");
    }
}

void SemanticAnalyzer::visitIfStatement(IfStatement& ifStmt) {
    Value conditionValue = ifStmt.condition->accept(*this);
    std::string conditionType = getTypeFromValue(conditionValue);
    if (conditionType != "BOOLEAN") {
        throw std::runtime_error("A condição do 'IF' deve ser do tipo BOOLEAN, mas obteve '" + conditionType + "'.");
    }

    symbolTable.enterScope();
    ifStmt.thenBranch->accept(*this);
    symbolTable.exitScope();

    if (ifStmt.elseBranch) {
        symbolTable.enterScope();
        ifStmt.elseBranch->accept(*this);
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::visitWhileStatement(WhileStatement& whileStmt) {
    Value conditionValue = whileStmt.condition->accept(*this);
    std::string conditionType = getTypeFromValue(conditionValue);
    if (conditionType != "BOOLEAN") {
        throw std::runtime_error("A condição do 'WHILE' deve ser do tipo BOOLEAN, mas obteve '" + conditionType + "'.");
    }

    symbolTable.enterScope();
    whileStmt.body->accept(*this);
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitForStatement(ForStatement& forStmt) {
    forStmt.initializer->accept(*this);
    Value conditionValue = forStmt.endCondition->accept(*this);
    std::string conditionType = getTypeFromValue(conditionValue);
    // Verificação adicional do tipo da condição, se necessário

    forStmt.body->accept(*this);
}

void SemanticAnalyzer::visitBlockStatement(BlockStatement& blockStmt) {
    symbolTable.enterScope();
    for (auto& stmt : blockStmt.statements) {
        stmt->accept(*this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitExpressionStatement(ExpressionStatement& exprStmt) {
    exprStmt.expression->accept(*this);
}

Value SemanticAnalyzer::visitIdentifier(Identifier& identifier) {
    auto symbol = symbolTable.resolve(identifier.name);
    if (!symbol) {
        throw std::runtime_error("Variável ou função '" + identifier.name + "' não foi declarada.");
    }
    // Retorna um Value correspondente ao tipo do símbolo
    if (symbol->type == "INTEGER") {
        return Value(0);
    } else if (symbol->type == "REAL") {
        return Value(0.0);
    } else if (symbol->type == "BOOLEAN") {
        return Value(true);
    } else {
        throw std::runtime_error("Tipo de símbolo desconhecido para '" + identifier.name + "'.");
    }
}

Value SemanticAnalyzer::visitNumber(Number& number) {
    if (std::floor(number.value) == number.value) {
        return Value(static_cast<int>(number.value));
    } else {
        return Value(number.value);
    }
}

Value SemanticAnalyzer::visitBooleanLiteral(BooleanLiteral& boolLit) {
    return Value(boolLit.value);
}

Value SemanticAnalyzer::visitBinaryOperation(BinaryOperation& binOp) {
    Value leftValue = binOp.left->accept(*this);
    Value rightValue = binOp.right->accept(*this);

    std::string leftType = getTypeFromValue(leftValue);
    std::string rightType = getTypeFromValue(rightValue);

    std::string resultType;

    if (binOp.op == OperatorType::ADD ||
        binOp.op == OperatorType::SUBTRACT ||
        binOp.op == OperatorType::MULTIPLY ||
        binOp.op == OperatorType::DIVIDE) {
        if ((leftType == "INTEGER" || leftType == "REAL") &&
            (rightType == "INTEGER" || rightType == "REAL")) {
            resultType = (leftType == "REAL" || rightType == "REAL") ? "REAL" : "INTEGER";
        } else {
            throw std::runtime_error("Tipos inválidos para operação aritmética.");
        }
    } else if (binOp.op == OperatorType::AND ||
               binOp.op == OperatorType::OR) {
        if (leftType == "BOOLEAN" && rightType == "BOOLEAN") {
            resultType = "BOOLEAN";
        } else {
            throw std::runtime_error("Tipos inválidos para operação lógica.");
        }
    } else if (binOp.op == OperatorType::EQUAL_EQUAL ||
               binOp.op == OperatorType::NOT_EQUAL ||
               binOp.op == OperatorType::LESS ||
               binOp.op == OperatorType::LESS_EQUAL ||
               binOp.op == OperatorType::GREATER ||
               binOp.op == OperatorType::GREATER_EQUAL) {
        if (leftType == rightType) {
            resultType = "BOOLEAN";
        } else {
            throw std::runtime_error("Tipos incompatíveis em operação de comparação.");
        }
    } else {
        throw std::runtime_error("Operador binário desconhecido.");
    }

    if (debug) {
        std::cout << "BinaryOperation: op = " << operatorTypeToString(binOp.op)
                  << ", leftType = " << leftType << ", rightType = " << rightType
                  << ", resultType = " << resultType << std::endl;
    }

    // Retorna um Value fictício correspondente ao tipo de resultado
    if (resultType == "INTEGER") {
        return Value(0);
    } else if (resultType == "REAL") {
        return Value(0.0);
    } else if (resultType == "BOOLEAN") {
        return Value(true);
    } else {
        throw std::runtime_error("Tipo de resultado desconhecido na operação binária.");
    }
}

Value SemanticAnalyzer::visitUnaryOperation(UnaryOperation& unaryOp) {
    Value operandValue = unaryOp.operand->accept(*this);
    std::string operandType = getTypeFromValue(operandValue);

    std::string resultType;

    if (unaryOp.op == OperatorType::NOT) {
        if (operandType == "BOOLEAN") {
            resultType = "BOOLEAN";
        } else {
            throw std::runtime_error("Operando de 'NOT' deve ser BOOLEAN.");
        }
    } else if (unaryOp.op == OperatorType::SUBTRACT) {
        if (operandType == "INTEGER" || operandType == "REAL") {
            resultType = operandType;
        } else {
            throw std::runtime_error("Operando de '-' unário deve ser INTEGER ou REAL.");
        }
    } else {
        throw std::runtime_error("Operador unário desconhecido.");
    }

    if (debug) {
        std::cout << "UnaryOperation: op = " << operatorTypeToString(unaryOp.op)
                  << ", operandType = " << operandType << ", resultType = " << resultType << std::endl;
    }

    // Retorna um Value fictício correspondente ao tipo de resultado
    if (resultType == "INTEGER") {
        return Value(0);
    } else if (resultType == "REAL") {
        return Value(0.0);
    } else if (resultType == "BOOLEAN") {
        return Value(true);
    } else {
        throw std::runtime_error("Tipo de resultado desconhecido na operação unária.");
    }
}

Value SemanticAnalyzer::visitFunctionCall(FunctionCall& funcCall) {
    auto symbol = symbolTable.resolve(funcCall.functionName);
    if (!symbol || symbol->symbolType != SymbolType::FUNCTION) {
        throw std::runtime_error("Função '" + funcCall.functionName + "' não foi declarada.");
    }

    // Verificação de argumentos, se necessário
    // Atualmente assumimos que as funções não têm parâmetros

    std::string resultType = symbol->type;
    if (debug) {
        std::cout << "FunctionCall: functionName = " << funcCall.functionName
                  << ", resultType = " << resultType << std::endl;
    }

    // Retorna um Value fictício correspondente ao tipo de retorno da função
    if (resultType == "INTEGER") {
        return Value(0);
    } else if (resultType == "REAL") {
        return Value(0.0);
    } else if (resultType == "BOOLEAN") {
        return Value(true);
    } else {
        throw std::runtime_error("Tipo de retorno desconhecido para a função '" + funcCall.functionName + "'.");
    }
}

Value SemanticAnalyzer::visitArrayAccess(ArrayAccess& arrayAccess) {
    Value arrayValue = arrayAccess.array->accept(*this);
    std::string arrayType = getTypeFromValue(arrayValue);

    for (auto& indexExpr : arrayAccess.indices) {
        Value indexValue = indexExpr->accept(*this);
        std::string indexType = getTypeFromValue(indexValue);
        if (indexType != "INTEGER") {
            throw std::runtime_error("Os índices de arrays devem ser do tipo INTEGER.");
        }
    }

    // Retorna um Value fictício correspondente ao tipo base do array
    if (arrayType == "INTEGER") {
        return Value(0);
    } else if (arrayType == "REAL") {
        return Value(0.0);
    } else if (arrayType == "BOOLEAN") {
        return Value(true);
    } else {
        throw std::runtime_error("Tipo de array desconhecido.");
    }
}

std::string SemanticAnalyzer::getTypeFromValue(const Value& value) {
    switch (value.getType()) {
        case Value::Type::INTEGER:
            return "INTEGER";
        case Value::Type::REAL:
            return "REAL";
        case Value::Type::BOOLEAN:
            return "BOOLEAN";
        default:
            return "VOID";
    }
}

void SemanticAnalyzer::setDebug(bool value) {
    debug = value;
}
