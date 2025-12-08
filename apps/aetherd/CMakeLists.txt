add_executable(aetherd
        src/main.cpp
        src/commands/cmd_core_stop.cpp
        include/commands/cmd_core_stop.hpp
        src/daemon.cpp
        include/daemon.hpp
)

# Link com core e qualquer outra lib que existir
target_link_libraries(aetherd PRIVATE aether_core)

# Linka o módulo ao daemon
target_link_libraries(aetherd PRIVATE aether_core ModuleTest)

# Inclui os headers do módulo
target_include_directories(aetherd
        PRIVATE
        ${CMAKE_SOURCE_DIR}/modules/ModuleTest/include
)