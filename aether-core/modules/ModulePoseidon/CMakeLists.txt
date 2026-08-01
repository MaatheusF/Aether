add_library(ModulePoseidon STATIC
        src/PoseidonMain.cpp
        include/PoseidonMain.hpp
        src/PoseidonService.cpp
        include/PoseidonService.hpp
        Schedule/ScheduleService.cpp
        Schedule/ScheduleService.hpp
)

# Expõe o include para quem linkar com essa biblioteca
# (caminho relativo a este CMakeLists.txt, não a CMAKE_SOURCE_DIR -- ver
# apps/aetherd/CMakeLists.txt para o motivo)
target_include_directories(ModulePoseidon PUBLIC include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(ModulePoseidon PUBLIC aether_core)