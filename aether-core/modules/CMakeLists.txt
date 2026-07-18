# Mesmo esquema dos protocolos: cada módulo é subdiretório (subrepo)
file(GLOB MODULE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(dir ${MODULE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
        add_subdirectory(${dir})
    endif()
endforeach()