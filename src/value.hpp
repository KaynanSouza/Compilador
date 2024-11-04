// value.hpp

#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>

class Value {
public:
    enum class Type {
        INTEGER,
        REAL,
        BOOLEAN,
        VOID
    };

    // Construtores
    Value(); // Construtor padrão para VOID
    Value(int intValue);
    Value(double realValue);
    Value(bool boolValue);

    // Método estático para criar um Value do tipo VOID
    static Value Void() {
        return Value();
    }

    // Métodos de acesso
    Type getType() const;
    int getIntValue() const;
    double getRealValue() const;
    bool getBoolValue() const;
    std::string toString() const;

private:
    Type type;
    union {
        int intValue;
        double realValue;
        bool boolValue;
    };
};

#endif // VALUE_HPP
