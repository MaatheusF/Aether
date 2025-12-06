add_executable(aetherd
        src/main.cpp
)

# Link com core e qualquer outra lib que existir
target_link_libraries(aetherd PRIVATE aether_core)