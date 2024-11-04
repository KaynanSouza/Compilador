// ast_optimizer.cpp

#include "ast_optimizer.hpp"
#include <cmath>
#include <memory>

void ASTOptimizer::optimize(Program* program) {
    optimizeProgram(program);
}

void ASTOptimizer::optimizeProgram(Program* program) {
    for (auto& stmt : program->statements) {
        optimizeStatement(stmt);
    }
}

void ASTOptimizer::optimizeStatement(std::unique_ptr<Statement>& stmt) {
    if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt.get())) {
        optimizeVariableDeclaration(varDecl);
    } else if (auto arrayDecl = dynamic_cast<ArrayDeclaration*>(stmt.get())) {
        optimizeArrayDeclaration(arrayDecl);
    } else if (auto assignment = dynamic_cast<Assignment*>(stmt.get())) {
        optimizeAssignment(assignment);
    } else if (auto returnStmt = dynamic_cast<ReturnStatement*>(stmt.get())) {
        optimizeReturnStatement(returnStmt);
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt.get())) {
        optimizeIfStatement(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(stmt.get())) {
        optimizeWhileStatement(whileStmt);
    } else if (auto forStmt = dynamic_cast<ForStatement*>(stmt.get())) {
        optimizeForStatement(forStmt);
    } else if (auto function = dynamic_cast<Function*>(stmt.get())) {
        optimizeFunction(function);
    } else if (auto blockStmt = dynamic_cast<BlockStatement*>(stmt.get())) {
        optimizeBlockStatement(blockStmt);
    } else if (auto exprStmt = dynamic_cast<ExpressionStatement*>(stmt.get())) {
        optimizeExpressionStatement(exprStmt);
    }
}

void ASTOptimizer::optimizeExpression(std::unique_ptr<Expression>& expr) {
    if (auto binOp = dynamic_cast<BinaryOperation*>(expr.get())) {
        // Otimiza os operandos
        optimizeExpression(binOp->left);
        optimizeExpression(binOp->right);

        // Tenta realizar a dobra de constantes
        if (auto leftNum = dynamic_cast<Number*>(binOp->left.get())) {
            if (auto rightNum = dynamic_cast<Number*>(binOp->right.get())) {
                double result = 0;
                bool canOptimize = true;
                switch (binOp->op) {
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
                            canOptimize = false;
                        }
                        break;
                    default:
                        canOptimize = false;
                        break;
                }
                if (canOptimize) {
                    // Substitui a operação pelo resultado constante
                    expr = std::make_unique<Number>(result);
                }
            }
        }
    } else if (auto unaryOp = dynamic_cast<UnaryOperation*>(expr.get())) {
        optimizeExpression(unaryOp->operand);

        if (auto operandNum = dynamic_cast<Number*>(unaryOp->operand.get())) {
            double result = 0;
            bool canOptimize = true;
            switch (unaryOp->op) {
                case OperatorType::SUBTRACT:
                    result = -operandNum->value;
                    break;
                // Outros operadores unários podem ser adicionados aqui
                default:
                    canOptimize = false;
                    break;
            }
            if (canOptimize) {
                // Substitui a operação pelo resultado constante
                expr = std::make_unique<Number>(result);
            }
        }
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(expr.get())) {
        for (auto& arg : funcCall->arguments) {
            optimizeExpression(arg);
        }
    } else if (auto arrayAccess = dynamic_cast<ArrayAccess*>(expr.get())) {
        optimizeExpression(arrayAccess->array);
        for (auto& index : arrayAccess->indices) {
            optimizeExpression(index);
        }
    }
    // Para Number, Identifier, BooleanLiteral, não há nada a fazer
}

void ASTOptimizer::optimizeVariableDeclaration(VariableDeclaration* varDecl) {
    if (varDecl->initializer) {
        optimizeExpression(varDecl->initializer);
    }
}

void ASTOptimizer::optimizeArrayDeclaration(ArrayDeclaration* arrayDecl) {
    if (arrayDecl->initializer) {
        optimizeExpression(arrayDecl->initializer);
    }
}

void ASTOptimizer::optimizeAssignment(Assignment* assignment) {
    optimizeExpression(assignment->left);
    optimizeExpression(assignment->right);
}

void ASTOptimizer::optimizeReturnStatement(ReturnStatement* returnStmt) {
    optimizeExpression(returnStmt->value);
}

void ASTOptimizer::optimizeIfStatement(IfStatement* ifStmt) {
    optimizeExpression(ifStmt->condition);
    optimizeStatement(ifStmt->thenBranch);
    if (ifStmt->elseBranch) {
        optimizeStatement(ifStmt->elseBranch);
    }
}

void ASTOptimizer::optimizeWhileStatement(WhileStatement* whileStmt) {
    optimizeExpression(whileStmt->condition);
    optimizeStatement(whileStmt->body);
}

void ASTOptimizer::optimizeForStatement(ForStatement* forStmt) {
    optimizeAssignment(forStmt->initializer.get());
    optimizeExpression(forStmt->endCondition);
    optimizeStatement(forStmt->body);
}

void ASTOptimizer::optimizeFunction(Function* function) {
    for (auto& stmt : function->body) {
        optimizeStatement(stmt);
    }
}

void ASTOptimizer::optimizeBlockStatement(BlockStatement* blockStmt) {
    for (auto& stmt : blockStmt->statements) {
        optimizeStatement(stmt);
    }
}

void ASTOptimizer::optimizeExpressionStatement(ExpressionStatement* exprStmt) {
    optimizeExpression(exprStmt->expression);
}
