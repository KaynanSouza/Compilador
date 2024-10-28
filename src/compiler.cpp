//compiler.cpp

#include "compiler.hpp"
#include "scanner.hpp"
#include "parser.hpp"

Compiler::Compiler() : symbolTable() {}

void Compiler::compile(const std::string& sourceCode) {
    Scanner scanner(sourceCode);
    auto tokens = scanner.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    // Continue with the compilation process
}
