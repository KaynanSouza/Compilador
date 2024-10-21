// parser_tests.cpp

#include "parser.hpp"
#include "scanner.hpp"
#include "semantic_analyzer.hpp"
#include "ast_optimizer.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

void testParser() {
    std::string code = R"(
        FUNCTION AddNumbers : INTEGER
            VAR_INPUT
                a : INTEGER;
                b : INTEGER;
            END_VAR

            VAR
                result : INTEGER;
            END_VAR

            result := a + b;
            RETURN result;
        END_FUNCTION

        PROGRAM MAIN
            VAR
                x : INTEGER;
                y : REAL;
                z : INTEGER;
                flag : BOOLEAN;
                result : INTEGER;
                w : INTEGER;
                // Declaração da variável 'i' para o loop FOR
                // i : INTEGER; // Não é mais necessário, pois o analisador semântico declara automaticamente
            END_VAR

            x := 10;
            y := 20.5;
            z := 0;
            flag := TRUE;

            // Teste de atribuicao e expressao aritmetica
            x := x + 1;

            // Teste de chamada de funcao
            z := AddNumbers(x, 5);

            // Teste de IF/ELSE
            IF (x > z) THEN
                y := y + 1.5;
            ELSE
                y := y - 1.5;
            END_IF;

            // Teste de WHILE
            WHILE (x < 20) DO
                x := x + 1;
            END_WHILE;

            // Teste de FOR
            FOR i := 0 TO 5 DO
                z := z + i;
            END_FOR;

            // Teste de expressao logica
            flag := (x > y) AND (z <= x);

            // Teste de otimizacoes
            result := 2 + 3 * 4; // Deve ser simplificado para result := 14;
            w := x * 1; // Deve ser simplificado para w := x;
            IF (FALSE) THEN
                x := x + 1; // Codigo morto, deve ser eliminado
            END_IF;
            WHILE (0) DO
                x := x - 1; // Codigo morto, deve ser eliminado
            END_WHILE;
        END_PROGRAM
    )";

    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    Parser parser(tokens);
    try {
        auto ast = parser.parse();

        // Realiza a análise semântica
        SemanticAnalyzer analyzer;
        analyzer.analyze(ast.get());

        // Otimiza a AST
        ASTOptimizer optimizer;
        optimizer.optimize(ast.get());

        std::cout << "Análise semântica e otimização concluídas com sucesso." << std::endl;

        // Opcional: Implemente uma função para imprimir a AST otimizada
        // printAST(ast.get());
    } catch (const std::exception& e) {
        std::cerr << "Erro durante a análise: " << e.what() << std::endl;
    }
}

int main() {
    testParser();
    return 0;
}
