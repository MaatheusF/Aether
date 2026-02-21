cmake_minimum_required(VERSION 3.10)
project(aether_core)

# Cria a biblioteca estática do core apenas com os .cpp
add_library(aether_core STATIC
        utils/logger.cpp
        eventbus/src/EventBus.cpp
        database/src/PostgresDriver.cpp
        database/include/PostgresDriver.hpp
        database/src/ConnectionHandle.cpp
        database/include/ConnectionHandle.hpp
        database/src/ConnectionPool.cpp
        database/include/ConnectionPool.hpp
        network/TcpServer.cpp
        network/TcpServer.hpp
        network/TcpConnection.cpp
        network/TcpConnection.hpp
        network/TcpResponseChannel.cpp
        network/TcpResponseChannel.hpp
        network/SessionManager.cpp
        network/SessionManager.hpp
        utils/logger.hpp
        network/session/ConnSession.hpp
)

# Inclui todos os headers públicos do core
target_include_directories(aether_core PUBLIC
        utils/include
        eventbus/include
)

# Link com pthread (se necessário)
target_link_libraries(aether_core PUBLIC pthread)

# Link com a biblioteca do PostgreSQL
find_package(PostgreSQL REQUIRED)
target_link_libraries(aether_core PRIVATE ${PostgreSQL_LIBRARIES})
target_include_directories(aether_core PRIVATE ${PostgreSQL_INCLUDE_DIRS})