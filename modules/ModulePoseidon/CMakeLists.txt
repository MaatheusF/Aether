add_library(ModulePoseidon STATIC
        src/PoseidonMain.cpp
        include/PoseidonMain.hpp
        src/PoseidonService.cpp
        include/PoseidonService.hpp
        Schedule/ScheduleService.cpp
        Schedule/ScheduleService.hpp
)

# Expõe o include para quem linkar com essa biblioteca
target_include_directories(ModulePoseidon PUBLIC include
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ModulePoseidon PUBLIC aether_core)