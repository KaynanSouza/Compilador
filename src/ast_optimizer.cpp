// ast_optimizer.cpp

#include "ast_optimizer.hpp"
#include <cmath>

// Otimiza o programa
void ASTOptimizer::optimize(Program* program) {
    std::vector<std::unique_ptr<Statement>> optimizedStatements;
    for (auto& stmt : program->statements) {
        auto optimizedStmt = optimizeStatement(stmt.get());
        if (optimizedStmt) {
            optimizedStatements.push_back(std::move(optimizedStmt));
        }
    }
    program->statements = std::move(optimizedStatements);
}

// Otimiza uma declaração
std::unique_ptr<Statement> ASTOptimizer::optimizeStatement(Statement* stmt) {
    if (auto block = dynamic_cast<BlockStatement*>(stmt)) {
        // Otimiza cada declaração dentro do bloco
        std::vector<std::unique_ptr<Statement>> optimizedStatements;
        for (auto& s : block->statements) {
            auto optimizedStmt = optimizeStatement(s.get());
            if (optimizedStmt) {
                optimizedStatements.push_back(std::move(optimizedStmt));
            }
        }
        return std::make_unique<BlockStatement>(std::move(optimizedStatements));
    } else if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        // Otimiza o inicializador, se existir
        if (varDecl->initializer) {
            varDecl->initializer = optimizeExpression(varDecl->initializer.get());
        }
        return std::make_unique<VariableDeclaration>(varDecl->name, varDecl->type, std::move(varDecl->initializer));
    } else if (auto assignment = dynamic_cast<Assignment*>(stmt)) {
        // Otimiza ambos os lados da atribuição
        assignment->left = optimizeExpression(assignment->left.get());
        assignment->right = optimizeExpression(assignment->right.get());
        return std::make_unique<Assignment>(std::move(assignment->left), std::move(assignment->right));
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        // Otimiza a condição e os ramos
        ifStmt->condition = optimizeExpression(ifStmt->condition.get());
        ifStmt->thenBranch = optimizeStatement(ifStmt->thenBranch.get());
        if (ifStmt->elseBranch) {
            ifStmt->elseBranch = optimizeStatement(ifStmt->elseBranch.get());
        }
        return std::make_unique<IfStatement>(
            std::move(ifStmt->condition),
            std::move(ifStmt->thenBranch),
            std::move(ifStmt->elseBranch)
        );
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        // Otimiza a condição e o corpo do loop
        whileStmt->condition = optimizeExpression(whileStmt->condition.get());
        whileStmt->body = optimizeStatement(whileStmt->body.get());
        return std::make_unique<WhileStatement>(
            std::move(whileStmt->condition),
            std::move(whileStmt->body)
        );
    } else if (auto forStmt = dynamic_cast<ForStatement*>(stmt)) {
        // Otimiza a inicialização, condição e corpo do loop
        forStmt->initializer = std::unique_ptr<Assignment>(static_cast<Assignment*>(optimizeStatement(forStmt->initializer.get()).release()));
        forStmt->endCondition = optimizeExpression(forStmt->endCondition.get());
        forStmt->body = optimizeStatement(forStmt->body.get());
        return std::make_unique<ForStatement>(
            std::move(forStmt->initializer),
            std::move(forStmt->endCondition),
            std::move(forStmt->body)
        );
    } else if (auto function = dynamic_cast<Function*>(stmt)) {
        // Otimiza o corpo da função
        std::vector<std::unique_ptr<Statement>> optimizedBody;
        for (auto& s : function->body) {
            auto optimizedStmt = optimizeStatement(s.get());
            if (optimizedStmt) {
                optimizedBody.push_back(std::move(optimizedStmt));
            }
        }
        function->body = std::move(optimizedBody);
        return std::make_unique<Function>(function->name);
    } else if (auto returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        // Otimiza o valor de retorno
        returnStmt->value = optimizeExpression(returnStmt->value.get());
        return std::make_unique<ReturnStatement>(std::move(returnStmt->value));
    } else {
        // Outros tipos de declarações não otimizados
        return nullptr;
    }
}

// Otimiza uma expressão
std::unique_ptr<Expression> ASTOptimizer::optimizeExpression(Expression* expr) {
    if (auto binaryOp = dynamic_cast<BinaryOperation*>(expr)) {
        // Otimiza os operandos
        binaryOp->left = optimizeExpression(binaryOp->left.get());
        binaryOp->right = optimizeExpression(binaryOp->right.get());

        // Otimização de constantes (constant folding)
        if (auto leftNum = dynamic_cast<Number*>(binaryOp->left.get())) {
            if (auto rightNum = dynamic_cast<Number*>(binaryOp->right.get())) {
                double result = 0;
                switch (binaryOp->op) {
                    case OperatorType::ADD:
                        result = leftNum->value + rightNum->value;
                        break;
                    case OperatorType::SUBTRACT:
                        result = leftNum->value - rightNum->value;
                        break;
                    case OperatorType::MULTIPLY:
                        result = leftNum->value * rightNum->value;
                        break;
                    case OperatorType::DIVIDE:
                        if (rightNum->value != 0) {
                            result = leftNum->value / rightNum->value;
                        } else {
                            // Evita divisão por zero
                            return std::make_unique<BinaryOperation>(binaryOp->op, std::move(binaryOp->left), std::move(binaryOp->right));
                        }
                        break;
                    default:
                        // Outros operadores não otimizados
                        break;
                }
                return std::make_unique<Number>(result);
            }
        }
        return std::make_unique<BinaryOperation>(binaryOp->op, std::move(binaryOp->left), std::move(binaryOp->right));
    } else if (auto unaryOp = dynamic_cast<UnaryOperation*>(expr)) {
        // Otimiza o operando
        unaryOp->operand = optimizeExpression(unaryOp->operand.get());

        if (auto operandNum = dynamic_cast<Number*>(unaryOp->operand.get())) {
            double result = 0;
            switch (unaryOp->op) {
                case OperatorType::SUBTRACT:
                    result = -operandNum->value;
                    break;
                // Outros operadores unários não otimizados
                default:
                    break;
            }
            return std::make_unique<Number>(result);
        }
        return std::make_unique<UnaryOperation>(unaryOp->op, std::move(unaryOp->operand));
    } else if (auto number = dynamic_cast<Number*>(expr)) {
        return std::make_unique<Number>(number->value);
    } else if (auto boolLit = dynamic_cast<BooleanLiteral*>(expr)) {
        return std::make_unique<BooleanLiteral>(boolLit->value);
    } else if (auto identifier = dynamic_cast<Identifier*>(expr)) {
        return std::make_unique<Identifier>(identifier->name);
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(expr)) {
        // Otimiza os argumentos da função
        std::vector<std::unique_ptr<Expression>> optimizedArgs;
        for (auto& arg : funcCall->arguments) {
            optimizedArgs.push_back(optimizeExpression(arg.get()));
        }
        return std::make_unique<FunctionCall>(funcCall->functionName, std::move(optimizedArgs));
    } else if (auto arrayAccess = dynamic_cast<ArrayAccess*>(expr)) {
        // Otimiza o array e os índices
        arrayAccess->array = optimizeExpression(arrayAccess->array.get());
        for (auto& indexExpr : arrayAccess->indices) {
            indexExpr = optimizeExpression(indexExpr.get());
        }
        return std::make_unique<ArrayAccess>(std::move(arrayAccess->array), std::move(arrayAccess->indices));
    } else {
        // Outros tipos de expressões não otimizadas
        return nullptr;
    }
}
