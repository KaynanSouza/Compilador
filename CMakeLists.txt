cmake_minimum_required(VERSION 3.29)
project(Compilador)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compilador main.cpp
        src/compiler.cpp
        src/scanner.cpp
        src/compiler.hpp
        src/scanner.hpp
        src/symbol_table.cpp
        src/symbol_table.hpp
        src/token.hpp)
