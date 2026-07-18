cmake_minimum_required(VERSION 3.20)

project(aether_api)

file(GLOB_RECURSE API_SOURCES
        controllers/*.cpp
        services/*.cpp
        transport/*.cpp
        common/*.cpp
        config/*.cpp
)

add_library(aether_api STATIC
        ${API_SOURCES}
)

target_include_directories(aether_api
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(aether_api
        PUBLIC
        aether_core
        Boost::system
        Boost::thread
)

