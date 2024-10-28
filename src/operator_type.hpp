// operator_type.hpp

#ifndef OPERATOR_TYPE_HPP
#define OPERATOR_TYPE_HPP

#include <string>

enum class OperatorType {
    ADD,            // '+'
    SUBTRACT,       // '-'
    MULTIPLY,       // '*'
    DIVIDE,         // '/'
    LESS,           // '<'
    LESS_EQUAL,     // '<='
    GREATER,        // '>'
    GREATER_EQUAL,  // '>='
    EQUAL_EQUAL,    // '=='
    NOT_EQUAL,      // '!='
    AND,            // 'AND'
    OR,             // 'OR'
    NOT             // 'NOT'
};

inline std::string operatorTypeToString(OperatorType op) {
    switch (op) {
        case OperatorType::ADD:
            return "+";
        case OperatorType::SUBTRACT:
            return "-";
        case OperatorType::MULTIPLY:
            return "*";
        case OperatorType::DIVIDE:
            return "/";
        case OperatorType::LESS:
            return "<";
        case OperatorType::LESS_EQUAL:
            return "<=";
        case OperatorType::GREATER:
            return ">";
        case OperatorType::GREATER_EQUAL:
            return ">=";
        case OperatorType::EQUAL_EQUAL:
            return "==";
        case OperatorType::NOT_EQUAL:
            return "!=";
        case OperatorType::AND:
            return "AND";
        case OperatorType::OR:
            return "OR";
        case OperatorType::NOT:
            return "NOT";
        default:
            return "UNKNOWN_OPERATOR";
    }
}

#endif // OPERATOR_TYPE_HPP
