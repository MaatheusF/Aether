#include <iostream>
#include "../include/cli_app.hpp"

int main(int argc, char** argv) {

    // Inicializa o CLI
    CliApp app;
    app.run(argc, argv);

    return 0;
}