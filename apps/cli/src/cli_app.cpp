#include "../include/cli_app.hpp"
#include "../include/commands/cmd_version.hpp"
#include "../include/commands/cmd_help.hpp"

#include <complex>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

#define SOCKET_PATH "/tmp/aetherd.socket"

int CliApp::run(int argc, char** argv)
{
    // Converte os argumentos recebidos para vector<string>
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }

    // Identifica se o comando foi executado com argumentos, caso "false" inicia o modo Shell:
    if (args.empty())
    {
        return runShell();
    }

    std::cout << "Comando incorreto, execute apenas aether para iniciar o Shell!" << std::endl;
    return 0;
}

std::vector<std::string> CliApp::splitArgs(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> parts;

    while (ss >> token) {
        parts.push_back(token);
    }

    return parts;
}

int CliApp::runShell()
{
    std::cout << "Running shell" << std::endl;

    while (true)
    {
        // Prompt
        std::cout << "\033[94mAether> \033[0m";
        std::cout.flush(); // Limpa o Buffer de saida do terminal

        // Ler linha inteira
        std::string line;
        if (!std::getline(std::cin, line))
            break;

        if (line.empty()) continue;

        auto args = CliApp::splitArgs(line);      // Passa os dados da linha para a função que realiza o split dos comandos
        const std::string& cmd = args[0];         // Transforma os args recebidos em string chamado "cmd" com o comando principal


        ///=============================
        ///==  Comandos do Console    ==
        ///=============================

        // Comando 'exit' / 'quit'
        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Finalizando shell...\n";
            break;
        }

        // Comando 'clear / 'c'
        if (cmd == "clear" || cmd == "c") {
            std::cout << "\033[2J\033[H";   //Limpa o console
            continue;
        }

        // Comando 'help'
        if (cmd == "help")
        {
            cmd_help(args);
        }

        ///=============================
        ///==    Comandos do CLI     ===
        ///=============================

        // PRIMEIRO ARGS = categoria (ex: core, module)
        std::string cmd_category = args[0];

        if (cmd_category == "core") {
            handleCoreCommand(args);
        } else {
            std::cout << "Comandos desconhecido" << std::endl;
        }

    }

    return 0;
}

void CliApp::handleCoreCommand(const std::vector<std::string>& args)
{
    /// Verifica se possui mais de 2 parametros/args
    if (args.size() < 2)
    {
        std::cout << "Uso: core <start|stop|status>" << std::endl;
        return;
    }

    std::string cmd_action = args[1];

    if (cmd_action == "start") {
        CliApp::sendCommand("core.start");
    }
    else if (cmd_action == "stop")
    {
        CliApp::sendCommand("core.stop");
    }
    else
    {
        std::cout << "Uso: core <start|stop|status>" << std::endl;
    }
}

std::string CliApp::sendCommand(std::string command)
{
    int fd = socket(AF_UNIX , SOCK_STREAM , 0);
    if (fd < 0) {
        perror("socket");
        return "ERROR";
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); //Copia o caminho do socket para o membro do sockaddr

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        perror("connect");
        return "ERROR";
    }

    write(fd, command.c_str(), command.size()); //Envia o comando para o daemon

    char buffer[512];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer)-1);

    if (bytes_read > 0) {
        return buffer;
    }

    close(fd);
    return "ERROR";
}