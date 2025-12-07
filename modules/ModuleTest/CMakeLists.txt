cmake_minimum_required(VERSION 3.10)
project(ModuleTest)

# Cria uma biblioteca estática do módulo
add_library(ModuleTest STATIC
        src/ModuleTest.cpp
        include/ModuleTest.hpp
)

# Expõe o include para o projeto principal
target_include_directories(aetherd PUBLIC include)