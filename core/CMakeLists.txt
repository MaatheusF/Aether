# Cria a biblioteca estática do core
add_library(aether_core
        utils/src/logger.cpp
        eventbus/include/Event.hpp
        eventbus/include/EventTypes.hpp
        eventbus/include/EventBus.hpp
        eventbus/src/EventBus.cpp
        module/IModule.hpp
        eventbus/include/IModule.hpp
        # tests/moduleTest.cpp
        tests/moduleTest.hpp
        # outros .cpp do core aqui quando existirem
        # src/runtime.cpp
        # src/eventbus.cpp
        # src/network.cpp
)

# Inclui todos os headers públicos do core
target_include_directories(aether_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/include
        # adicionar outros include paths dos subdiretórios
        # ${CMAKE_CURRENT_SOURCE_DIR}/network/include
        # ${CMAKE_CURRENT_SOURCE_DIR}/eventbus/include
)

# Link com pthread (se precisar)
target_link_libraries(aether_core PUBLIC pthread)