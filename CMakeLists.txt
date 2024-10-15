cmake_minimum_required(VERSION 3.29)
project(Compilador)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compilador
        src/compiler.cpp
        src/scanner.cpp
        src/compiler.hpp
        src/scanner.hpp
        src/symbol_table.cpp
        src/symbol_table.hpp
        src/token.hpp
        src/ast.hpp
        src/parser.hpp
        src/parser.cpp
        src/parser_tests.cpp
        src/statement.hpp
)
