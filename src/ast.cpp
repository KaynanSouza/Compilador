// ast.cpp

#include "ast.hpp"
#include "value.hpp"

// Implementações dos métodos accept para Statement

void Program::accept(Visitor& visitor) {
    visitor.visitProgram(*this);
}

void VariableDeclaration::accept(Visitor& visitor) {
    visitor.visitVariableDeclaration(*this);
}

void ArrayDeclaration::accept(Visitor& visitor) {
    visitor.visitArrayDeclaration(*this);
}

void Assignment::accept(Visitor& visitor) {
    visitor.visitAssignment(*this);
}

void ReturnStatement::accept(Visitor& visitor) {
    visitor.visitReturnStatement(*this);
}

void IfStatement::accept(Visitor& visitor) {
    visitor.visitIfStatement(*this);
}

void WhileStatement::accept(Visitor& visitor) {
    visitor.visitWhileStatement(*this);
}

void ForStatement::accept(Visitor& visitor) {
    visitor.visitForStatement(*this);
}

void Function::accept(Visitor& visitor) {
    visitor.visitFunction(*this);
}

void BlockStatement::accept(Visitor& visitor) {
    visitor.visitBlockStatement(*this);
}

void ExpressionStatement::accept(Visitor& visitor) {
    visitor.visitExpressionStatement(*this);
}

// Implementações dos métodos accept para Expression

Value Identifier::accept(Visitor& visitor) {
    return visitor.visitIdentifier(*this);
}

Value Number::accept(Visitor& visitor) {
    return visitor.visitNumber(*this);
}

Value BooleanLiteral::accept(Visitor& visitor) {
    return visitor.visitBooleanLiteral(*this);
}

Value BinaryOperation::accept(Visitor& visitor) {
    return visitor.visitBinaryOperation(*this);
}

Value UnaryOperation::accept(Visitor& visitor) {
    return visitor.visitUnaryOperation(*this);
}

Value FunctionCall::accept(Visitor& visitor) {
    return visitor.visitFunctionCall(*this);
}

Value ArrayAccess::accept(Visitor& visitor) {
    return visitor.visitArrayAccess(*this);
}
