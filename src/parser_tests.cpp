#include "parser.hpp"
#include "scanner.hpp"
#include <iostream>
#include <cassert>

void testParser() {
    std::string code = R"(
        VAR
            a : REAL;
            b : REAL;
        END_VAR

        FUNCTION CALCULA_VAZAO : REAL
            VAR_INPUT
                ALTURA : REAL;
                POTENCIA : REAL;
            END_VAR
            VAR
                RESULTADO : REAL;
            END_VAR
            RESULTADO := ALTURA * POTENCIA + 10.0;
            RETURN RESULTADO;
        END_FUNCTION

        PROGRAM MAIN
            VAR
                NIVEL : REAL;
                POT : REAL;
                VAZAO : REAL;
            END_VAR

            NIVEL := 50.0;
            POT := 2.5;
            VAZAO := CALCULA_VAZAO(NIVEL, POT);
        END_PROGRAM
    )";

    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    auto ast = parser.parse();

    // Basic checks
    assert(ast != nullptr);
    assert(!ast->statements.empty());

    // Find MAIN program
    Function* mainProgram = nullptr;
    for (const auto& stmt : ast->statements) {
        if (auto func = dynamic_cast<Function*>(stmt.get())) {
            if (func->name == "MAIN") {
                mainProgram = func;
                break;
            }
        }
    }

    assert(mainProgram != nullptr);
    assert(mainProgram->name == "MAIN");

    // Check assignments in MAIN
    size_t assignmentCount = 0;
    for (const auto& stmt : mainProgram->body) {
        if (dynamic_cast<Assignment*>(stmt.get())) {
            assignmentCount++;
        }
    }

    assert(assignmentCount == 3); // NIVEL := 50.0; POT := 2.5; VAZAO := CALCULA_VAZAO(NIVEL, POT);

    std::cout << "Parser test passed successfully." << std::endl;
}

int main() {
    try {
        testParser();
    } catch (const std::exception& e) {
        std::cerr << "Parser test failed: " << e.what() << std::endl;
    }
    return 0;
}
