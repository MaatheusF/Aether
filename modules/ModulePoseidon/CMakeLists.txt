add_library(ModulePoseidon STATIC
        src/PoseidonMain.cpp
        include/PoseidonMain.hpp
        src/PoseidonService.cpp
        include/PoseidonService.hpp
)

# Expõe o include para quem linkar com essa biblioteca
target_include_directories(ModulePoseidon PUBLIC include)
