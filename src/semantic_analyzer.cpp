// semantic_analyzer.cpp

#include "semantic_analyzer.hpp"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include "operator_type.hpp"

void SemanticAnalyzer::analyze(Program* program) {
    visitProgram(program);
}

void SemanticAnalyzer::visitProgram(Program* program) {
    for (auto& stmt : program->statements) {
        visitStatement(stmt.get());
    }
}

void SemanticAnalyzer::visitFunction(Function* function) {
    symbolTable.enterScope();

    currentFunctionReturnType = function->returnType;

    for (auto& stmt : function->body) {
        visitStatement(stmt.get());
    }

    symbolTable.exitScope();
}

void SemanticAnalyzer::visitVariableDeclaration(VariableDeclaration* varDecl) {
    if (symbolTable.currentScope().find(varDecl->name) != symbolTable.currentScope().end()) {
        throw std::runtime_error("Variável '" + varDecl->name + "' já foi declarada neste escopo.");
    }
    symbolTable.define(varDecl->name, varDecl->type, SymbolType::VARIABLE);

    if (varDecl->initializer) {
        auto initType = visitExpressionAndGetType(varDecl->initializer.get());
        if (initType != varDecl->type) {
            throw std::runtime_error("Tipo do inicializador '" + initType + "' não corresponde ao tipo da variável '" + varDecl->type + "'.");
        }
    }
}

void SemanticAnalyzer::visitArrayDeclaration(ArrayDeclaration* arrayDecl) {
    if (symbolTable.currentScope().find(arrayDecl->name) != symbolTable.currentScope().end()) {
        throw std::runtime_error("Array '" + arrayDecl->name + "' já foi declarado neste escopo.");
    }
    Symbol arraySymbol(arrayDecl->name, arrayDecl->baseType, SymbolType::ARRAY);
    arraySymbol.dimensions = arrayDecl->dimensions;
    symbolTable.currentScope()[arrayDecl->name] = arraySymbol;

    // If there is an initializer, type-check it
    if (arrayDecl->initializer) {
        // Implement type-checking for array initializers if needed
        // This may involve traversing the initializer structure and comparing types
    }
}

void SemanticAnalyzer::visitAssignment(Assignment* assignment) {
    auto leftType = visitExpressionAndGetType(assignment->left.get());
    auto rightType = visitExpressionAndGetType(assignment->right.get());

    if (leftType != rightType) {
        throw std::runtime_error("Tipos incompatíveis na atribuição: '" + leftType + "' e '" + rightType + "'.");
    }
}

void SemanticAnalyzer::visitReturnStatement(ReturnStatement* returnStmt) {
    auto returnType = visitExpressionAndGetType(returnStmt->value.get());
    if (returnType != currentFunctionReturnType) {
        throw std::runtime_error("Tipo de retorno '" + returnType + "' não corresponde ao tipo de retorno da função '" + currentFunctionReturnType + "'.");
    }
}

void SemanticAnalyzer::visitIfStatement(IfStatement* ifStmt) {
    auto conditionType = visitExpressionAndGetType(ifStmt->condition.get());
    if (conditionType != "BOOLEAN") {
        throw std::runtime_error("A condição do 'IF' deve ser do tipo BOOLEAN, mas obteve '" + conditionType + "'.");
    }

    symbolTable.enterScope();
    visitStatement(ifStmt->thenBranch.get());
    symbolTable.exitScope();

    if (ifStmt->elseBranch) {
        symbolTable.enterScope();
        visitStatement(ifStmt->elseBranch.get());
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::visitWhileStatement(WhileStatement* whileStmt) {
    auto conditionType = visitExpressionAndGetType(whileStmt->condition.get());
    if (conditionType != "BOOLEAN") {
        throw std::runtime_error("A condição do 'WHILE' deve ser do tipo BOOLEAN, mas obteve '" + conditionType + "'.");
    }

    symbolTable.enterScope();
    visitStatement(whileStmt->body.get());
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitForStatement(ForStatement* forStmt) {
    visitAssignment(forStmt->initializer.get());
    auto conditionType = visitExpressionAndGetType(forStmt->endCondition.get());
    if (conditionType != "INTEGER") {
        throw std::runtime_error("A condição final do 'FOR' deve ser do tipo INTEGER, mas obteve '" + conditionType + "'.");
    }

    symbolTable.enterScope();
    visitStatement(forStmt->body.get());
    symbolTable.exitScope();
}

void SemanticAnalyzer::visitStatement(Statement* stmt) {
    if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        visitVariableDeclaration(varDecl);
    } else if (auto arrayDecl = dynamic_cast<ArrayDeclaration*>(stmt)) {
        visitArrayDeclaration(arrayDecl);
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
    } else if (auto function = dynamic_cast<Function*>(stmt)) {
        visitFunction(function);
    } else {
        throw std::runtime_error("Tipo de declaração não reconhecido no analisador semântico.");
    }
}

// Verifica o tipo de uma expressão e retorna o tipo resultante
std::string SemanticAnalyzer::visitExpressionAndGetType(Expression* expr) {
    std::string exprType;
    if (auto number = dynamic_cast<Number*>(expr)) {
        // Determina se é INTEGER ou REAL
        if (std::floor(number->value) == number->value) {
            exprType = "INTEGER";
        } else {
            exprType = "REAL";
        }
        if (debug) {
            std::cout << "Number: value = " << number->value << ", type = " << exprType << std::endl;
        }
        return exprType;
    } else if (auto boolLit = dynamic_cast<BooleanLiteral*>(expr)) {
        exprType = "BOOLEAN";
        if (debug) {
            std::cout << "BooleanLiteral: value = " << (boolLit->value ? "TRUE" : "FALSE") << ", type = BOOLEAN" << std::endl;
        }
        return exprType;
    } else if (auto identifier = dynamic_cast<Identifier*>(expr)) {
        // Verifica se o identificador foi declarado
        auto symbol = symbolTable.resolve(identifier->name);
        if (!symbol) {
            throw std::runtime_error("Variável ou função '" + identifier->name + "' não foi declarada.");
        }
        exprType = symbol->type;
        if (debug) {
            std::cout << "Identifier: name = " << identifier->name << ", type = " << exprType << std::endl;
        }
        return exprType;
    } else if (auto arrayAccess = dynamic_cast<ArrayAccess*>(expr)) {
        // Obtém o identificador do array
        auto arrayIdentifier = dynamic_cast<Identifier*>(arrayAccess->array.get());
        if (!arrayIdentifier) {
            throw std::runtime_error("O array deve ser um identificador simples.");
        }
        auto symbol = symbolTable.resolve(arrayIdentifier->name);
        if (!symbol || symbol->symbolType != SymbolType::ARRAY) {
            throw std::runtime_error("Tentativa de acessar um elemento de um não-array: '" + arrayIdentifier->name + "'.");
        }
        if (arrayAccess->indices.size() != symbol->dimensions.size()) {
            throw std::runtime_error("Número incorreto de índices ao acessar o array '" + arrayIdentifier->name + "'.");
        }
        // Verifica se os índices são do tipo INTEGER
        for (auto& indexExpr : arrayAccess->indices) {
            auto indexType = visitExpressionAndGetType(indexExpr.get());
            if (indexType != "INTEGER") {
                throw std::runtime_error("Os índices de arrays devem ser do tipo INTEGER.");
            }
        }
        exprType = symbol->type; // Retorna o tipo base do array
        if (debug) {
            std::cout << "ArrayAccess: arrayName = " << arrayIdentifier->name
                      << ", type = " << exprType << std::endl;
        }
        return exprType;
    } else if (auto binaryOp = dynamic_cast<BinaryOperation*>(expr)) {
        // Processa operação binária
        // ...
        // (Código existente para processar operações binárias)
    } else if (auto unaryOp = dynamic_cast<UnaryOperation*>(expr)) {
        // Processa operação unária
        // ...
        // (Código existente para processar operações unárias)
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(expr)) {
        // Processa chamada de função
        // ...
        // (Código existente para processar chamadas de função)
    } else {
        throw std::runtime_error("Expressão não reconhecida para verificação de tipo.");
    }
}
