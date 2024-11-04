// value.cpp

#include "value.hpp"
#include <stdexcept>

Value::Value() : type(Type::VOID) {}

Value::Value(int intValue) : type(Type::INTEGER), intValue(intValue) {}

Value::Value(double realValue) : type(Type::REAL), realValue(realValue) {}

Value::Value(bool boolValue) : type(Type::BOOLEAN), boolValue(boolValue) {}

Value::Type Value::getType() const {
    return type;
}

int Value::getIntValue() const {
    if (type == Type::INTEGER) {
        return intValue;
    } else {
        throw std::runtime_error("Value is not an INTEGER");
    }
}

double Value::getRealValue() const {
    if (type == Type::REAL) {
        return realValue;
    } else if (type == Type::INTEGER) {
        return static_cast<double>(intValue);
    } else {
        throw std::runtime_error("Value is not a REAL");
    }
}

bool Value::getBoolValue() const {
    if (type == Type::BOOLEAN) {
        return boolValue;
    } else {
        throw std::runtime_error("Value is not a BOOLEAN");
    }
}

std::string Value::toString() const {
    switch (type) {
        case Type::INTEGER:
            return std::to_string(intValue);
        case Type::REAL:
            return std::to_string(realValue);
        case Type::BOOLEAN:
            return boolValue ? "TRUE" : "FALSE";
        case Type::VOID:
            return "VOID";
        default:
            return "UNKNOWN";
    }
}
