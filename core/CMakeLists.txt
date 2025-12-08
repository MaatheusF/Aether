cmake_minimum_required(VERSION 3.10)
project(aether_core)

# Cria a biblioteca estática do core apenas com os .cpp
add_library(aether_core STATIC
        utils/src/logger.cpp
        eventbus/src/EventBus.cpp
)

# Inclui todos os headers públicos do core
target_include_directories(aether_core PUBLIC
        utils/include
        eventbus/include
)

# Link com pthread (se necessário)
target_link_libraries(aether_core PUBLIC pthread)
