# Protocol Aether

add_library(aether_protocol
        src/dummy.cpp
        include/Packet.hpp
        include/CommandType.hpp
)

target_include_directories(aether_protocol
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(aether_protocol
        PUBLIC aether_core
)