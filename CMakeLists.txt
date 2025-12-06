cmake_minimum_required(VERSION 4.2)
project(Aether)

# Usar C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Diretórios principais
add_subdirectory(core)
add_subdirectory(protocols)
add_subdirectory(modules)
add_subdirectory(apps)
