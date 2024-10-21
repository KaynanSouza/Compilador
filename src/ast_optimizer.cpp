// ast_optimizer.cpp

#include "ast_optimizer.hpp"
#include <cmath>
#include <stdexcept>

void ASTOptimizer::optimize(Program* program) {
    for (auto& stmt : program->statements) {
        stmt = optimizeStatement(stmt.get());
    }
}

std::unique_ptr<Statement> ASTOptimizer::optimizeStatement(Statement* stmt) {
    if (auto block = dynamic_cast<BlockStatement*>(stmt)) {
        std::vector<std::unique_ptr<Statement>> optimizedStatements;
        for (auto& innerStmt : block->statements) {
            auto optimizedStmt = optimizeStatement(innerStmt.get());
            if (optimizedStmt) {
                optimizedStatements.push_back(std::move(optimizedStmt));
            }
        }
        block->statements = std::move(optimizedStatements);
        return std::unique_ptr<Statement>(block);
    } else if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        return std::unique_ptr<Statement>(varDecl);
    } else if (auto assignment = dynamic_cast<Assignment*>(stmt)) {
        assignment->right = optimizeExpression(assignment->right.get());
        return std::unique_ptr<Statement>(assignment);
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        return eliminateDeadCode(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        return eliminateDeadCode(whileStmt);
    } else if (auto forStmt = dynamic_cast<ForStatement*>(stmt)) {
        // Otimiza o inicializador
        auto optimizedInitializerStmt = optimizeStatement(forStmt->initializer.get());
        Assignment* optimizedInitializerRaw = dynamic_cast<Assignment*>(optimizedInitializerStmt.release());
        if (optimizedInitializerRaw) {
            forStmt->initializer = std::unique_ptr<Assignment>(optimizedInitializerRaw);
        } else {
            throw std::runtime_error("Inicializador do loop 'FOR' não é uma atribuição.");
        }

        // Otimiza a condição de término
        forStmt->endCondition = optimizeExpression(forStmt->endCondition.get());

        // Otimiza o corpo
        auto optimizedBodyStmt = optimizeStatement(forStmt->body.get());
        if (optimizedBodyStmt) {
            forStmt->body = std::move(optimizedBodyStmt);
        } else {
            // Se o corpo for nulo, define como uma declaração vazia ou lida conforme necessário
            forStmt->body = nullptr;
        }

        return std::unique_ptr<Statement>(forStmt);
    } else if (auto returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        returnStmt->value = optimizeExpression(returnStmt->value.get());
        return std::unique_ptr<Statement>(returnStmt);
    } else if (auto func = dynamic_cast<Function*>(stmt)) {
        for (auto& funcStmt : func->body) {
            funcStmt = optimizeStatement(funcStmt.get());
        }
        return std::unique_ptr<Statement>(func);
    } else {
        // Outros tipos de declarações
        return std::unique_ptr<Statement>(stmt);
    }
}

std::unique_ptr<Expression> ASTOptimizer::optimizeExpression(Expression* expr) {
    if (auto number = dynamic_cast<Number*>(expr)) {
        return std::make_unique<Number>(number->value);
    } else if (auto boolLit = dynamic_cast<BooleanLiteral*>(expr)) {
        return std::make_unique<BooleanLiteral>(boolLit->value);
    } else if (auto identifier = dynamic_cast<Identifier*>(expr)) {
        return std::make_unique<Identifier>(identifier->name);
    } else if (auto binaryOp = dynamic_cast<BinaryOperation*>(expr)) {
        binaryOp->left = optimizeExpression(binaryOp->left.get());
        binaryOp->right = optimizeExpression(binaryOp->right.get());

        // Aplica constant folding
        if (auto foldedExpr = foldConstants(binaryOp)) {
            return foldedExpr;
        }

        // Simplifica operações triviais
        if (auto simplifiedExpr = simplifyTrivialOperations(binaryOp)) {
            return simplifiedExpr;
        }

        return std::unique_ptr<Expression>(binaryOp);
    } else if (auto unaryOp = dynamic_cast<UnaryOperation*>(expr)) {
        unaryOp->operand = optimizeExpression(unaryOp->operand.get());
        // Se o operando for uma constante, avalia em tempo de compilação
        if (auto number = dynamic_cast<Number*>(unaryOp->operand.get())) {
            if (unaryOp->op == "-") {
                return std::make_unique<Number>(-number->value);
            }
        } else if (auto boolLit = dynamic_cast<BooleanLiteral*>(unaryOp->operand.get())) {
            if (unaryOp->op == "NOT") {
                return std::make_unique<BooleanLiteral>(!boolLit->value);
            }
        }
        return std::unique_ptr<Expression>(unaryOp);
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(expr)) {
        for (auto& arg : funcCall->arguments) {
            arg = optimizeExpression(arg.get());
        }
        return std::unique_ptr<Expression>(funcCall);
    } else {
        // Outros tipos de expressões
        return std::unique_ptr<Expression>(expr);
    }
}

// Implementa constant folding para operações com constantes
std::unique_ptr<Expression> ASTOptimizer::foldConstants(BinaryOperation* binOp) {
    auto leftNumber = dynamic_cast<Number*>(binOp->left.get());
    auto rightNumber = dynamic_cast<Number*>(binOp->right.get());
    auto leftBool = dynamic_cast<BooleanLiteral*>(binOp->left.get());
    auto rightBool = dynamic_cast<BooleanLiteral*>(binOp->right.get());

    if (leftNumber && rightNumber) {
        double result = 0;
        if (binOp->op == "+") {
            result = leftNumber->value + rightNumber->value;
        } else if (binOp->op == "-") {
            result = leftNumber->value - rightNumber->value;
        } else if (binOp->op == "*") {
            result = leftNumber->value * rightNumber->value;
        } else if (binOp->op == "/") {
            // Verifica divisão por zero
            if (rightNumber->value == 0) {
                throw std::runtime_error("Divisão por zero detectada em tempo de compilação.");
            }
            result = leftNumber->value / rightNumber->value;
        } else if (binOp->op == "<") {
            return std::make_unique<BooleanLiteral>(leftNumber->value < rightNumber->value);
        } else if (binOp->op == "<=") {
            return std::make_unique<BooleanLiteral>(leftNumber->value <= rightNumber->value);
        } else if (binOp->op == ">") {
            return std::make_unique<BooleanLiteral>(leftNumber->value > rightNumber->value);
        } else if (binOp->op == ">=") {
            return std::make_unique<BooleanLiteral>(leftNumber->value >= rightNumber->value);
        } else if (binOp->op == "==") {
            return std::make_unique<BooleanLiteral>(leftNumber->value == rightNumber->value);
        } else if (binOp->op == "!=") {
            return std::make_unique<BooleanLiteral>(leftNumber->value != rightNumber->value);
        } else {
            // Operador desconhecido
            return nullptr;
        }
        return std::make_unique<Number>(result);
    } else if (leftBool && rightBool) {
        bool result = false;
        if (binOp->op == "AND") {
            result = leftBool->value && rightBool->value;
        } else if (binOp->op == "OR") {
            result = leftBool->value || rightBool->value;
        } else if (binOp->op == "==") {
            result = leftBool->value == rightBool->value;
        } else if (binOp->op == "!=") {
            result = leftBool->value != rightBool->value;
        } else {
            // Operador desconhecido
            return nullptr;
        }
        return std::make_unique<BooleanLiteral>(result);
    }
    return nullptr;
}

// Simplifica operações triviais (ex: multiplicação por 1)
std::unique_ptr<Expression> ASTOptimizer::simplifyTrivialOperations(BinaryOperation* binOp) {
    // Simplificações para operações triviais
    auto leftNumber = dynamic_cast<Number*>(binOp->left.get());
    auto rightNumber = dynamic_cast<Number*>(binOp->right.get());

    if (binOp->op == "*") {
        // x * 1 => x
        if (rightNumber && rightNumber->value == 1) {
            return std::move(binOp->left);
        }
        // 1 * x => x
        if (leftNumber && leftNumber->value == 1) {
            return std::move(binOp->right);
        }
        // x * 0 => 0
        if ((leftNumber && leftNumber->value == 0) || (rightNumber && rightNumber->value == 0)) {
            return std::make_unique<Number>(0);
        }
    } else if (binOp->op == "+") {
        // x + 0 => x
        if (rightNumber && rightNumber->value == 0) {
            return std::move(binOp->left);
        }
        if (leftNumber && leftNumber->value == 0) {
            return std::move(binOp->right);
        }
    } else if (binOp->op == "-") {
        // x - 0 => x
        if (rightNumber && rightNumber->value == 0) {
            return std::move(binOp->left);
        }
    } else if (binOp->op == "/") {
        // x / 1 => x
        if (rightNumber && rightNumber->value == 1) {
            return std::move(binOp->left);
        }
    }

    return nullptr;
}

// Elimina código morto em instruções IF
std::unique_ptr<Statement> ASTOptimizer::eliminateDeadCode(IfStatement* ifStmt) {
    ifStmt->condition = optimizeExpression(ifStmt->condition.get());

    if (auto conditionBool = dynamic_cast<BooleanLiteral*>(ifStmt->condition.get())) {
        if (conditionBool->value) {
            // Condição é sempre verdadeira; mantém apenas o ramo 'then'
            return optimizeStatement(ifStmt->thenBranch.get());
        } else {
            // Condição é sempre falsa; mantém apenas o ramo 'else', se existir
            if (ifStmt->elseBranch) {
                return optimizeStatement(ifStmt->elseBranch.get());
            } else {
                // Nenhum código a executar; elimina o 'if'
                return nullptr;
            }
        }
    } else {
        ifStmt->thenBranch = optimizeStatement(ifStmt->thenBranch.get());
        if (ifStmt->elseBranch) {
            ifStmt->elseBranch = optimizeStatement(ifStmt->elseBranch.get());
        }
        return std::unique_ptr<Statement>(ifStmt);
    }
}

// Elimina código morto em instruções WHILE
std::unique_ptr<Statement> ASTOptimizer::eliminateDeadCode(WhileStatement* whileStmt) {
    whileStmt->condition = optimizeExpression(whileStmt->condition.get());

    if (auto conditionBool = dynamic_cast<BooleanLiteral*>(whileStmt->condition.get())) {
        if (conditionBool->value) {
            // Loop infinito detectado; pode ser um erro ou intencional
            // Neste caso, não eliminamos o código, mas poderíamos emitir um aviso
            whileStmt->body = optimizeStatement(whileStmt->body.get());
            return std::unique_ptr<Statement>(whileStmt);
        } else {
            // Condição sempre falsa; remove o loop
            return nullptr;
        }
    } else {
        whileStmt->body = optimizeStatement(whileStmt->body.get());
        return std::unique_ptr<Statement>(whileStmt);
    }
}
