// parser_tests.cpp

#include "parser.hpp"
#include "scanner.hpp"
#include <iostream>
#include <cassert>

void testParser() {
    std::string code = R"(
        PROGRAM MAIN
            VAR
                x : INTEGER;
                y : INTEGER;
                result : INTEGER;
            END_VAR

            x := 10;
            y := 20;

            IF (x < y) THEN
                result := x + y;
            ELSE
                result := x - y;
            END_IF;

            WHILE (x > 0) DO
                x := x - 1;
            END_WHILE;

            FOR i := 0 TO 10 DO
                result := result + i;
            END_FOR;
        END_PROGRAM
    )";

    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    try {
        auto ast = parser.parse();
        // Você pode adicionar verificações ou imprimir a AST aqui
        std::cout << "Parser test passed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Parser test failed: " << e.what() << std::endl;
    }
}

int main() {
    testParser();
    return 0;
}
