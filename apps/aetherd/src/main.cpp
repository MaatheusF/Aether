#include <chrono>
#include <iostream>
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <memory>
#include <vector>

#include "../../../core/eventbus/include/EventBus.hpp"
#include "../../../core/eventbus/include/EventTypes.hpp"
#include "../../../core/eventbus/include/Event.hpp"

#include "../../../core/tests/moduleTest.hpp"

#include "../include/commands/cmd_core_stop.hpp"

#define SOCKET_PATH "/tmp/aetherd.socket"

/**
 * @brief Função que processa os comandos recebidos pelo CLI
 * @param command comando recebido
 * @param modules lista de modulos inscritos
 */
void processCliCommand(const std::string& command, const std::vector<IModule*>& modules)
{
    if (command == "core.stop")
    {
        StopListener stopListener;
        EventBus::getInstance().subscribe(&stopListener);

        EventBus::getInstance().publish(
            Event("CLI", "", Events::CORE_STOP, {})
        );

        // Aguarda todos os módulos confirmarem
        waitForAllModulesToStop(modules);

        EventBus::getInstance().unsubscribe(&stopListener);
        std::cout << "[CLI] Todos os módulos parados." << std::endl;
    }
}

/**Função principal do daemon que inicia o AetherCore, modulos, bibliotecas e serviços, alem de criar um socket UNIX
 *para comunicação local entre os serviços LINUX. Inicia um servidor local socket usando AF_UNIX que recebe comandos,
 *processa e retorna o status para o CLI (TODO: É ideial portar o servidor que response o CLI para outro diretorio)
 */
int main() {
    std::cout << "AETHER daemon starting..." << std::endl;

    // ===========================
    // INICIALIZAÇÃO DOS MODULOS
    // ===========================

    // Inicialização dos módulos
    std::vector<std::unique_ptr<IModule>> loadedModules;
    loadedModules.push_back(std::make_unique<ModuleTest>());

    // Vetor de ponteiros para passar para processCliCommand
    for (auto& m : loadedModules) {
        EventBus::getInstance().subscribe(m.get());
    }

    // Vetor de ponteiros crus para facilitar o controle
    std::vector<IModule*> modules;
    for (auto& m : loadedModules) {
        modules.push_back(m.get());
    }

    //ModuleTest moduletest;
    //moduletest.initialize();

    std::cout << "[Aetherd] Iniciado." << std::endl;

    // ===========================
    // INICIALIZAÇÃO DO SOCKET CLI
    // ===========================

    unlink(SOCKET_PATH); // Exclui um arquivo de socket Unix do Sistema (Garante que não tenha lixo na memoria)

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // Cria um Socket do tipo UNIX
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); //Copia o caminho do socket para o membro do sockaddr

    //Associa o socket (bind) a um endereço e porta
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    std::cout << "[Aetherd] Daemon iniciado. Socket: " << SOCKET_PATH << "\n";

    // TODO: iniciar core, carregar módulos, adapters etc

    while (true) {
        // Loop do daemon

        int client_fd = accept(server_fd, nullptr, nullptr); // Bloqueia o while e aguarda uma conexão do ciente
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[512];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) -1);

        if (bytes_read > 0)
        {
            /*
            buffer[bytes_read] = '\0';
            std::string cmd(buffer);
            std::string response; //String que armazena o dado de retorno do comando
            write(client_fd, response.c_str(), response.size()); //Envia o retorno do comando para o CLI*/

            std::string command(buffer, bytes_read);

            processCliCommand(command, modules);
        }
        write(client_fd, "ACK", 3);
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
