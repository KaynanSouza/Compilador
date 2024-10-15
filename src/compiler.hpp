#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <string>
#include "symbol_table.hpp"

class Compiler {
public:
    Compiler();
    void compile(const std::string& sourceCode);

private:
    SymbolTable symbolTable;
};

#endif // COMPILER_HPP
