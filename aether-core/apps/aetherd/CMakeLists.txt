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


# Raiz do aether-core, relativa a este CMakeLists.txt -- ao contrário de
# CMAKE_SOURCE_DIR, não depende de qual diretório foi aberto como raiz do
# projeto (aether-core/ direto, ou o monorepo inteiro via o CMakeLists.txt
# wrapper na raiz do repo).
set(AETHER_CORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Inclui os headers do módulo
target_include_directories(aetherd
        PRIVATE
        ${AETHER_CORE_ROOT}/modules/ModuleTest/include
        ${AETHER_CORE_ROOT}/modules/ModulePoseidon/include
        ${AETHER_CORE_ROOT}/include
        ${AETHER_CORE_ROOT}
)