#include "../include/cli_app.hpp"

#include <chrono>
#include "../include/commands/cmd_version.hpp"
#include "../include/commands/cmd_help.hpp"
#include "../include/utils.hpp"

#include <complex>
#include <deque>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <thread>

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
    /// imprime a mensagem de boas vindas
    Utils::printWelcomeMessage();

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
        } else if (cmd_category == "logs"){
            handleLogsCommand(args);
        } else {
            std::cout << "Comandos desconhecido" << std::endl;
        }

    }

    return 0;
}

/**
 * @brief Implementa uma função para exibir os logs do Aether (tail -f)
 * @param args argumentos fornecidos no Shell
 */
void CliApp::handleLogsCommand(const std::vector<std::string>& args)
{

    /// ================================================
    ///      Tratamento dos Parametros de entrada
    /// ================================================

    int lines = 10; /// Linhas de logs que serão exibidas por padrão

    /// Verifica se possui maior de 1 parametros/args
    if (args.size() > 1)
    {
        /// Se o parametro for maior que o padrão, utiliza o parametro fornecido no CLI
        try {
            int argLines = std::stoi(args[1]);
            if (argLines > lines)
                lines = argLines;
        } catch (...) {
            /// Se receber um valor incorreto mantem 'lines' com o valor padrão
        }
    }

    /// ================================================
    ///                 Exibição dos logs
    /// ================================================

    std::ifstream file("/var/log/aether/aether_log", std::ios::in);
    if (!file.is_open())
    {
        std::cerr << "Não foi possível abrir o arquivo de logs em " << "/var/log/aether/aether_log" << std::endl;
        return;
    }

    std::deque<std::string> buffer;
    std::string line; // <-- variável correta para getline

    // Le o arquivo inteiro, mas mantem apenas as ultimas linhas
    while (std::getline(file, line))
    {
        buffer.push_back(line);
        if (buffer.size() > lines)
        {
            buffer.pop_front();
        }
    }

    // Exibe as ultimas linhas
    for (const auto& l : buffer)
    {
        std::cout << l << std::endl;
    }

    // Move para o final do arquivo para acompanhar novas linhas
    file.clear();                   /// Limpa flags EOF
    file.seekg(0, std::ios::end);   /// Posiciona o ponteiro do arquivo para a ultima linha

    while (true)
    {
        std::streampos currentPos = file.tellg();
        if (std::getline(file, line))
        {
            std::cout << line << std::endl;
        } else
        {
            file.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            file.seekg(currentPos);
        }
    }
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