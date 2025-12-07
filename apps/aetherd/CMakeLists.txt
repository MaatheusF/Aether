add_executable(aetherd
        src/main.cpp
        src/commands/cmd_core_stop.cpp
        include/commands/cmd_core_stop.hpp
        src/commands/cmd_core_start.cpp
        include/commands/cmd_core_start.hpp
        src/daemon.cpp
        include/daemon.hpp
)

# Link com core e qualquer outra lib que existir
target_link_libraries(aetherd PRIVATE aether_core)