add_executable(aetherd
        src/main.cpp
        src/commands/cmd_core_stop.cpp
        include/commands/cmd_core_stop.hpp
)

# Link com core e qualquer outra lib que existir
target_link_libraries(aetherd PRIVATE aether_core)