cmake_minimum_required(VERSION 3.10)

# Cria a biblioteca estática do módulo
add_library(ModuleTest STATIC
        src/ModuleTest.cpp
        include/ModuleTest.hpp
)

# Expõe o include para quem linkar com essa biblioteca
target_include_directories(ModuleTest PUBLIC include)