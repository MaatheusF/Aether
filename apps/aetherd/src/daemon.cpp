#include "../include/daemon.hpp"

#include <memory>
#include <vector>
#include <sys/socket.h>
#include <sys/un.h>

#include "../../../modules/ModuleTest/include/ModuleTest.hpp"
#include "../../../modules/ModulePoseidon/include/PoseidonMain.hpp"
#include "../include/commands/cmd_core_stop.hpp"
#include "../../../core/network/TcpServer.hpp"
#include "../../../core/utils/logger.hpp"
#include "../../../core/network/ProtocolRouter.hpp"

#define SOCKET_PATH "/tmp/aetherd.socket"

class IModule; /// Declaração antecipada da classe IModule

/**
 * @brief Função que inicia o daemon do Aether
 * @return retorna 0 caso o daemon for finalizado
 */
int AetherDaemon::initializeAetherDaemon()
{
    std::cout << "[Daemon] Aether daemon inicializando..." << std::endl;

    AetherCoreLogger::Initialize("/var/log/aether/aether_log"); /// Inicializa o sistema de logs do AetherCore

    initializeModules();   /// Inicializa os modulos do Aether
    initializeCliSocket(); /// Inicializa o socket de comunicação com o CLI
    initializeTcpServer(); /// Inicializa o servidor TCP para comunicação externa

    std::cout << "[Daemon] Aether daemon executando com sucesso." << std::endl;

    /// Loop principal do daemon
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr); /// Aceita uma conexão do CLI
        if (client_fd < 0) {
            perror("accept");
        }

        char buffer[512];                                                /// Buffer para armazenar o comando recebido
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) -1); /// Lê o comando enviado pelo CLI

        if (bytes_read > 0)
        {
            /*
            buffer[bytes_read] = '\0';
            std::string cmd(buffer);
            std::string response; //String que armazena o dado de retorno do comando
            write(client_fd, response.c_str(), response.size()); //Envia o retorno do comando para o CLI*/

            std::string command(buffer, bytes_read);
            processCliCommands(command);
        }
        write(client_fd, "ACK", 3); /// Envia um ACK simples para o CLI
        close(client_fd);           /// Fecha a conexão com o cliente
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

/**
 * @brief Função que realiza a inicialização inicial de todos os modulos do Aether
 * @return retorna true se todos os modulos foram inicializados com sucesso
 */
void AetherDaemon::initializeModules()
{
    std::cout << "[Aetherd] Inicializando Modulos individuais." << std::endl;

    /// Carrega os Modulos Individuais
    loadedModules = createModules();

    /// Inicializa os modulos
    for (auto& m : loadedModules) {
        m->start();                 // Inicia o módulo (thread ou loop interno)
        modules.push_back(m.get()); // Adiciona ao vetor de ponteiros crus
    }

    std::cout << "[Aetherd] Modulos individuais inicializados com sucesso." << std::endl;
}

/**
 * @brief Função que instancia todos os modulos do sistema Aether,
 * utilizado no Daemon para registrar os modulos na conexão TCP principalmente
 * @return retorna um elemento contendo todos os modulos criados
 */
std::vector<std::shared_ptr<IModule>> AetherDaemon::createModules()
{
    std::vector<std::shared_ptr<IModule>> mods; /// Lista de modulos

    mods.push_back(std::make_shared<ModuleTest>());
    mods.push_back(std::make_shared<ModulePoseidon>());
    //mods.push_back(std::make_shared<ModuleGps>());
    //mods.push_back(std::make_shared<ModuleTelemetry>());

    return mods;
}

/**
 * @brief Função que realiza a inicialização do socket de conexão com o CLI
 */
void AetherDaemon::initializeCliSocket()
{
    std::cout << "[Daemon] Inicializando Daemon Socket CLI." << std::endl;

    unlink(SOCKET_PATH); // Exclui um arquivo de socket Unix do Sistema (Garante que não tenha lixo na memoria)
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // Cria um Socket do tipo UNIX

    /// Verifica se socket foi criado com sucesso
    if (server_fd < 0) {
        perror("socket");
    }

    sockaddr_un addr{};        /// Estrutura de endereço do socket UNIX
    addr.sun_family = AF_UNIX; /// Define a família do socket como UNIX
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); /// Copia o caminho do socket para o membro do sockaddr

    /// Associa o socket (bind) a um endereço e porta
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
    }

    /// Coloca o socket em modo de escuta (listen) para aceitar conexões
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
    }

    std::cout << "[Daemon] Daemon Socket CLI inicializado com sucesso." << std::endl;
}

/**
 * @brief Função que realiza o processamento dos comandos recebidos pelo CLI
 * @param command comando recebido
 */
void AetherDaemon::processCliCommands(const std::string& command)
{
    if (command == "core.stop")
    {
        std::cout << "[CLI] Comando recebido 'core.stop'. Parando todos os modulos..." << std::endl;

        StopListener stopListener;                        /// Listener para aguardar confirmação de parada dos módulos
        EventBus::getInstance().subscribe(&stopListener); /// Inscreve o listener no EventBus para ouvir o callback

        /// Publica o evento de parada para todos os módulos
        /*EventBus::getInstance().publish(
            Event("CLI", "", Events::CORE_STOP, {})
        );*/

        // Aguarda todos os módulos confirmarem
        waitForAllModulesToStop(modules);

        EventBus::getInstance().unsubscribe(&stopListener); /// Remove o listener do EventBus

        std::cout << "[CLI] Todos os módulos parados." << std::endl;
    }
}

/**
 * @brief Função que inicializa o servidor TCP para comunicação externa
 */
void AetherDaemon::initializeTcpServer()
{
    std::cout << "[TcpServer] Inicializando TCP SERVER." << std::endl;
    tcpServer = std::make_unique<TcpServer>(9000); /// Cria o servidor TCP na porta 9000
    auto router = std::make_shared<ProtocolRouter>();

    /// Registra automaticamente todos os módulos que falam TCP
    for (auto& m : loadedModules)
    {
        auto handler = std::dynamic_pointer_cast<IProtocolHandler>(m);
        if (handler)
        {
            router->registerModule(handler, m);
            std::cout << "[TcpServer] Módulo registrado no ProtocolRouter: 0x" << std::hex << static_cast<int>(handler->moduleId()) << std::dec << std::endl;
        }
    }

    tcpServer->setProtocolHandler(router);  /// Define o router como destino de todos os pacotes TCP
    tcpServer->start(); /// Inicia o servidor TCP

    std::cout << "[TcpServer] Tcp Server inicializado" << std::endl;
}