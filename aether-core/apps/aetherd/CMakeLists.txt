add_executable(aetherd
        src/main.cpp
        src/commands/cmd_core_stop.cpp
        include/commands/cmd_core_stop.hpp
        src/daemon.cpp
        include/daemon.hpp
)

find_package(PostgreSQL REQUIRED)

# Link com core e qualquer outra lib que existir
target_link_libraries(aetherd
        PRIVATE aether_core
        PRIVATE aether_protocol
)

# Linka o módulo ao daemon
target_link_libraries(aetherd PRIVATE aether_core ModuleTest)
target_link_libraries(aetherd PRIVATE aether_core ModulePoseidon)
target_link_libraries(aetherd PRIVATE ${PostgreSQL_LIBRARIES})
target_link_libraries(aetherd PRIVATE aether_api)


# Inclui os headers do módulo
target_include_directories(aetherd
        PRIVATE
        ${CMAKE_SOURCE_DIR}/aether-core/modules/ModuleTest/include
        ${CMAKE_SOURCE_DIR}/aether-core/modules/ModulePoseidon/include
        ${CMAKE_SOURCE_DIR}/aether-core/include
        ${CMAKE_SOURCE_DIR}/aether-core
)