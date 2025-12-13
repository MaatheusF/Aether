# Protocol Aether

add_library(aether_protocol
        src/dummy.cpp
        include/Packet.hpp
        include/CommandType.hpp
        src/Parser.cpp
        include/Parser.hpp
        common/IResponseChannel.hpp
        src/PacketBuilder.cpp
        include/PacketBuilder.hpp
        common/IProtocolHandler.hpp
)

target_include_directories(aether_protocol
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(aether_protocol
        PUBLIC aether_core
)