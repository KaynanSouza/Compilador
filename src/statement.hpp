#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include <iostream>

// Classe base para todos os tipos de statements (declarações)
class Statement {
public:
    virtual ~Statement() = default;

    // Método virtual para imprimir informações sobre o statement (opcional)
    virtual void print() const = 0;
};

#endif // STATEMENT_HPP
