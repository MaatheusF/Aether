#pragma once
#include <memory>
#include <vector>

#include "../../../core/eventbus/include/IModule.hpp"
#include "../../../core/network/TcpServer.hpp"

class ModuleTest;
class IModule;

class AetherDaemon
{
public:
    /**
     * @brief Função que inicia o daemon do Aether
     * @return retorna 0 caso o daemon for finalizado
     */
    int initializeAetherDaemon();
private:
    /**
     * @brief Função que realiza a inicialização inicial de todos os modulos do Aether
     * @return retorna true se todos os modulos foram inicializados com sucesso
     */
    void initializeModules();
    /**
     * @brief Função que realiza a inicialização do socket de conexão com o CLI
     */
    void initializeCliSocket();
    /**
     * @brief Função que realiza o processamento dos comandos recebidos pelo CLI
     */
    void processCliCommands(const std::string& command);
    /**
     * @brief Função que inicializa o servidor TCP para comunicação externa
     */
    void initializeTcpServer();

    /**
     * @brief Função que instancia todos os modulos do sistema Aether,
     * utilizado no Daemon para registrar os modulos na conexão TCP principalmente
     * @return retorna um elemento contendo todos os modulos criados
     */
    static std::vector<std::shared_ptr<IModule>> createModules();

    std::vector<IModule*> modules;                           /// Lista de Modulos
    std::vector<std::shared_ptr<IModule>> loadedModules;     /// Lista de Modulos do Aether Inicializados
    int server_fd = 0;                                       /// File descriptor do socket do servidor CLI
    std::unique_ptr<TcpServer> tcpServer;                    /// Servidor TCP para comunicação externa
    std::shared_ptr<ModuleTest> moduleTest;                  /// Módulo de Teste (Ponteiro direto para facilitar o acesso)
};