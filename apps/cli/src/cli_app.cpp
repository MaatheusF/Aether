#include "../include/cli_app.hpp"
#include "../include/commands/cmd_version.hpp"
#include "../include/commands/cmd_help.hpp"

#include <complex>
#include <iostream>

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

    // Identifica se o comando foi executado com argumentos, caso "true" processa o comando direto
    return runOneShot(args);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
int CliApp::runOneShot(const std::vector<std::string>& args)
{
    const std::string command = args.at(0); // Converte o args para string

    if (command == "version")
    {
        cmd_version(args);
    }

    if (command == "help")
    {
        cmd_help(args);
    }

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

        // Comando 'exit' / 'quit'
        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Finalizando shell...\n";
            break;
        }

        // Comando 'clear / 'c'
        if (cmd == "clear" || cmd == "c") {
            std::cout << "\033[2J\033[H";   //Limpa o console
        }

        if (cmd == "version")
        {
            cmd_version(args);
        }

        if (cmd == "help")
        {
            cmd_help(args);
        }
    }

    return 0;
}