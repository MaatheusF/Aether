add_executable(aether_cli
        src/main.cpp
        ../../core/utils/src/logger.cpp
)

# Link com core e qualquer outra lib que existir
target_link_libraries(aether_cli PRIVATE aether_core)