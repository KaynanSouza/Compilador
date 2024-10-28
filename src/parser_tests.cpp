// parser_tests.cpp

#include "parser.hpp"
#include "scanner.hpp"
#include "semantic_analyzer.hpp"
#include "ast_optimizer.hpp"
#include <iostream>

void testParser() {
    std::string code = R"(
        (* Declaration of global variables *)
        VAR_GLOBAL
            globalCount : INT := 0;
            globalArray : ARRAY [1 TO 5] OF INT := [1, 2, 3, 4, 5];
        END_VAR

        (* Attempt to use a reserved word as a variable name - should cause a compiler error if uncommented *)
        (*
        VAR_GLOBAL
            IF : BOOL;  (* 'IF' is a reserved word and cannot be used as a variable name *)
        END_VAR
        *)

        (* Function definition *)
        FUNCTION MultiplyByTwo : INT
        VAR_INPUT
            number : INT;
        END_VAR
        MultiplyByTwo := number * 2;
        END_FUNCTION

        (* Function Block (Procedure) definition *)
        FUNCTION_BLOCK Counter
        VAR_INPUT
            increment : INT;
        END_VAR
        VAR
            count : INT := 0;
        END_VAR
        count := count + increment;
        END_FUNCTION_BLOCK

        (* Main Program *)
        PROGRAM MainProgram
        VAR
            localCount : INT := 0;
            myArray : ARRAY [1..10] OF INT;
            myMatrix : ARRAY [1..3, 1..3] OF REAL;
            result : INT;
            myCounter : Counter;
            timer : TON;
        END_VAR

        (* Assign values to the array and matrix *)
        myArray[5] := 20;
        myMatrix[2, 3] := 5.5;

        (* Use the function *)
        result := MultiplyByTwo(10);  (* Should return 20 *)

        (* Use the function block *)
        myCounter(increment := 1);

        (* Access global variables *)
        globalCount := globalCount + 1;

        (* Attempt to access a local variable from outside its scope - should cause an error if uncommented *)
        (*
        globalCount := localCount;  (* 'localCount' is not accessible globally *)
        *)

        (* Timed task simulation *)
        timer(IN := TRUE, PT := T#5S);

        IF timer.Q THEN
            (* This block runs every 5 seconds *)
            localCount := localCount + 1;
            timer(IN := TRUE);  (* Restart the timer *)
        END_IF

        (* Correct use of reserved words *)
        IF localCount > 10 THEN
            globalCount := 0;
        ELSIF localCount = 5 THEN
            globalCount := 5;
        ELSE
            globalCount := globalCount + 1;
        END_IF

        (* Incorrect use of reserved words - should cause compiler errors if uncommented *)
        (*
        VAR
            THEN : INT;  (* 'THEN' is a reserved word *)
        END_VAR
        *)
        END_PROGRAM

    )";

    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    try {
        auto ast = parser.parse();

        // Realiza a análise semântica
        SemanticAnalyzer analyzer;
        analyzer.debug = true; // Ativa o modo de depuração
        analyzer.analyze(ast.get());

        // Otimiza a AST
        ASTOptimizer optimizer;
        optimizer.optimize(ast.get());

        std::cout << "Análise semântica e otimização concluídas com sucesso." << std::endl;

        // Opcional: implementar uma função para imprimir a AST otimizada
        // printAST(ast.get());
    } catch (const std::exception& e) {
        std::cerr << "Erro durante a análise: " << e.what() << std::endl;
    }
}

int main() {
    testParser();
    return 0;
}
