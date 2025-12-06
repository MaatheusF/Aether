# Se protocolos existirem, adiciona cada subdiretório
# Para já permitir expansão futura
file(GLOB PROTO_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(dir ${PROTO_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        add_subdirectory(${dir})
    endif()
endforeach()