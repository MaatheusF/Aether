add_executable(aether
        #CPP
        src/main.cpp
        src/utils.cpp
        src/cli_app.cpp

        #CPP dos comandos
        src/commands/cmd_version.cpp
        src/commands/cmd_help.cpp

        #INCLUDES
        include/cli_app.hpp
        include/utils.hpp

        #Header/Include dos comandos
        include/commands/cmd_help.hpp
        include/commands/cmd_version.hpp
)

install(TARGETS aether RUNTIME DESTINATION bin)

# Link com core e qualquer outra lib que existir
target_link_libraries(aether PRIVATE aether_core)