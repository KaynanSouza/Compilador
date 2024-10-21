// semantic_analyzer.cpp

#include "semantic_analyzer.hpp"
#include <stdexcept>
#include <cmath>

void SemanticAnalyzer::analyze(Program* program) {
    visitProgram(program);
}

void SemanticAnalyzer::visitProgram(Program* program) {
    symbolTable.enterScope(); // Entra no escopo global

    for (auto& stmt : program->statements) {
        if (auto func = dynamic_cast<Function*>(stmt.get())) {
            visitFunction(func);
        } else {
            visitStatement(stmt.get());
        }
    }

    symbolTable.exitScope(); // Sai do escopo global
}

void SemanticAnalyzer::visitFunction(Function* function) {
    // Declara a função na tabela de símbolos
    if (!symbolTable.declare(function->name, function->returnType, SymbolType::FUNCTION)) {
        throw std::runtime_error("Função '" + function->name + "' já foi declarada.");
    }

    // Armazena o tipo de retorno da função atual
    currentFunctionReturnType = function->returnType;

    symbolTable.enterScope(); // Entra no escopo da função

    for (auto& stmt : function->body) {
        visitStatement(stmt.get());
    }

    symbolTable.exitScope(); // Sai do escopo da função
}

void SemanticAnalyzer::visitBlockStatement(BlockStatement* block) {
    symbolTable.enterScope(); // Entra em um novo escopo para o bloco

    for (auto& stmt : block->statements) {
        visitStatement(stmt.get());
    }

    symbolTable.exitScope(); // Sai do escopo do bloco
}

void SemanticAnalyzer::visitVariableDeclaration(VariableDeclaration* varDecl) {
    // Declara a variável na tabela de símbolos
    if (!symbolTable.declare(varDecl->name, varDecl->type, SymbolType::VARIABLE)) {
        throw std::runtime_error("Variável '" + varDecl->name + "' já foi declarada.");
    }
}

void SemanticAnalyzer::visitAssignment(Assignment* assignment) {
    // Verifica se a variável à esquerda foi declarada
    auto symbol = symbolTable.resolve(assignment->left->name);
    if (!symbol) {
        throw std::runtime_error("Variável '" + assignment->left->name + "' não foi declarada.");
    }
    // Verifica o tipo da expressão à direita
    auto exprType = visitExpressionAndGetType(assignment->right.get());

    // Permite conversão implícita de INTEGER para REAL
    if (symbol->type == "REAL" && exprType == "INTEGER") {
        exprType = "REAL";
    }

    if (exprType != symbol->type) {
        throw std::runtime_error("Tipo incompatível na atribuição à variável '" + assignment->left->name + "'. Esperado '" + symbol->type + "', mas obteve '" + exprType + "'.");
    }
}

void SemanticAnalyzer::visitReturnStatement(ReturnStatement* returnStmt) {
    auto exprType = visitExpressionAndGetType(returnStmt->value.get());

    // Permite conversão implícita de INTEGER para REAL
    if (currentFunctionReturnType == "REAL" && exprType == "INTEGER") {
        exprType = "REAL";
    }

    if (exprType != currentFunctionReturnType) {
        throw std::runtime_error("Tipo de retorno incompatível na função. Esperado '" + currentFunctionReturnType + "', mas obteve '" + exprType + "'.");
    }
}

void SemanticAnalyzer::visitIfStatement(IfStatement* ifStmt) {
    auto conditionType = visitExpressionAndGetType(ifStmt->condition.get());
    if (conditionType != "BOOLEAN") {
        throw std::runtime_error("A condição do 'IF' deve ser do tipo BOOLEAN, mas obteve '" + conditionType + "'.");
    }
    visitStatement(ifStmt->thenBranch.get());
    if (ifStmt->elseBranch) {
        visitStatement(ifStmt->elseBranch.get());
    }
}

void SemanticAnalyzer::visitWhileStatement(WhileStatement* whileStmt) {
    auto conditionType = visitExpressionAndGetType(whileStmt->condition.get());
    if (conditionType != "BOOLEAN") {
        throw std::runtime_error("A condição do 'WHILE' deve ser do tipo BOOLEAN, mas obteve '" + conditionType + "'.");
    }
    visitStatement(whileStmt->body.get());
}

void SemanticAnalyzer::visitForStatement(ForStatement* forStmt) {
    symbolTable.enterScope(); // Entra em um novo escopo para o loop

    // Declarar a variável de controle do loop
    std::string loopVarName = forStmt->initializer->left->name;
    std::string initType = visitExpressionAndGetType(forStmt->initializer->right.get());

    if (!symbolTable.declare(loopVarName, initType, SymbolType::VARIABLE)) {
        throw std::runtime_error("Variável de controle do loop 'FOR' '" + loopVarName + "' já foi declarada.");
    }

    // Inicialização
    visitAssignment(forStmt->initializer.get());
    auto initVarType = symbolTable.resolve(loopVarName)->type;

    // Condição de término
    auto endType = visitExpressionAndGetType(forStmt->endCondition.get());

    // Permite conversão implícita de INTEGER para REAL
    if (initVarType == "REAL" && endType == "INTEGER") {
        endType = "REAL";
    } else if (initVarType == "INTEGER" && endType == "REAL") {
        initVarType = "REAL";
    }

    if (endType != initVarType) {
        throw std::runtime_error("Tipos incompatíveis no loop 'FOR'. Variável de controle é do tipo '" + initVarType + "', mas o valor final é do tipo '" + endType + "'.");
    }

    // Analisa o corpo do loop
    visitStatement(forStmt->body.get());

    symbolTable.exitScope(); // Sai do escopo do loop
}

void SemanticAnalyzer::visitExpression(Expression* expr) {
    // Implementação genérica se não precisar do tipo
}

// Método que visita uma expressão e retorna seu tipo
std::string SemanticAnalyzer::visitExpressionAndGetType(Expression* expr) {
    if (auto number = dynamic_cast<Number*>(expr)) {
        if (std::floor(number->value) == number->value) {
            return "INTEGER";
        } else {
            return "REAL";
        }
    } else if (auto boolLit = dynamic_cast<BooleanLiteral*>(expr)) {
        return "BOOLEAN";
    } else if (auto identifier = dynamic_cast<Identifier*>(expr)) {
        auto symbol = symbolTable.resolve(identifier->name);
        if (!symbol) {
            throw std::runtime_error("Variável ou função '" + identifier->name + "' não foi declarada.");
        }
        return symbol->type;
    } else if (auto binaryOp = dynamic_cast<BinaryOperation*>(expr)) {
        auto leftType = visitExpressionAndGetType(binaryOp->left.get());
        auto rightType = visitExpressionAndGetType(binaryOp->right.get());

        // Permite conversão implícita entre INTEGER e REAL
        if ((leftType == "INTEGER" && rightType == "REAL") || (leftType == "REAL" && rightType == "INTEGER")) {
            // Promove INTEGER para REAL
            leftType = rightType = "REAL";
        }

        if (leftType != rightType) {
            throw std::runtime_error("Operação '" + binaryOp->op + "' entre tipos incompatíveis: '" + leftType + "' e '" + rightType + "'.");
        }

        // Determina o tipo resultante da expressão
        if (binaryOp->op == "==" || binaryOp->op == "!=" || binaryOp->op == "<" || binaryOp->op == "<=" || binaryOp->op == ">" || binaryOp->op == ">=" || binaryOp->op == "AND" || binaryOp->op == "OR") {
            return "BOOLEAN";
        } else {
            return leftType;
        }
    } else if (auto unaryOp = dynamic_cast<UnaryOperation*>(expr)) {
        auto operandType = visitExpressionAndGetType(unaryOp->operand.get());
        if (unaryOp->op == "NOT" && operandType != "BOOLEAN") {
            throw std::runtime_error("Operador 'NOT' aplicado a um tipo não BOOLEAN.");
        }
        if (unaryOp->op == "-" && (operandType != "INTEGER" && operandType != "REAL")) {
            throw std::runtime_error("Operador '-' aplicado a um tipo não numérico.");
        }
        return operandType;
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(expr)) {
        auto symbol = symbolTable.resolve(funcCall->functionName);
        if (!symbol || symbol->symbolType != SymbolType::FUNCTION) {
            throw std::runtime_error("Função '" + funcCall->functionName + "' não foi declarada.");
        }
        // Verifica argumentos da função aqui, se necessário
        return symbol->type;
    } else {
        throw std::runtime_error("Expressão não reconhecida para verificação de tipo.");
    }
}

// Método genérico para visitar um statement
void SemanticAnalyzer::visitStatement(Statement* stmt) {
    if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        visitVariableDeclaration(varDecl);
    } else if (auto assignment = dynamic_cast<Assignment*>(stmt)) {
        visitAssignment(assignment);
    } else if (auto returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        visitReturnStatement(returnStmt);
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        visitIfStatement(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        visitWhileStatement(whileStmt);
    } else if (auto forStmt = dynamic_cast<ForStatement*>(stmt)) {
        visitForStatement(forStmt);
    } else if (auto block = dynamic_cast<BlockStatement*>(stmt)) {
        visitBlockStatement(block);
    } else {
        throw std::runtime_error("Tipo de declaração não reconhecido na análise semântica.");
    }
}
