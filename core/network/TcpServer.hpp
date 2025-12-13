#pragma once
#include "TcpConnection.hpp"
#include "../../protocols/aether/common/IProtocolHandler.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <thread>

/** Espaço de nomes para o protocolo Aether */
namespace ProtocolAether {
    class Parser;
}

/** Classe que implementa um servidor TCP */
class TcpServer
{
public:
    using OnClientConnected = std::function<void(int)>;                           /// Callback para quando um cliente se conecta
    using OnDataReceived = std::function<void(int, const std::vector<uint8_t>&)>; /// Callback para quando dados são recebidos
    using OnClientDisconnected = std::function<void(int)>;                        /// Callback para quando um cliente se desconecta

    /** Construtor do servidor TCP
     * @param port Porta na qual o servidor irá escutar
     */
    explicit TcpServer(int port);

    /** Destrutor do servidor TCP */
    ~TcpServer();

    /** Inicia o servidor TCP */
    void start();

    /** Para o servidor TCP */
    void stop();

    void setOnClientConnected(const OnClientConnected& cb) { onClientConnected = cb; }          /// Define o callback para conexão de cliente
    void setOnDataReceived(const OnDataReceived& cb) { onDataReceived = cb; }                   /// Define o callback para recebimento de dados
    void setOnClientDisconnected(const OnClientDisconnected& cb) { onClientDisconnected = cb; } /// Define o callback para descon
    void setProtocolHandler(IProtocolHandler* handler);                                         /// Define o handler de protocolo

private:
    void acceptLoop();                  /// Loop para aceitar conexões de clientes
    void clientLoop(int clientSocket);  /// Loop para comunicação com o cliente

    int serverSocket;                   /// Socket do servidor
    int serverPort;                     /// Porta do servidor
    std::atomic<bool> isRunning;        /// Indica se o servidor está em execução
    std::thread acceptThread;           /// Thread para aceitar conexões de clientes

    std::set<std::shared_ptr<TcpConnection>> connections;   /// Lista de Conexões ativas
    OnClientConnected onClientConnected = nullptr;          /// Callback para quando um cliente se conecta
    OnDataReceived onDataReceived = nullptr;                /// Callback para quando dados são recebidos
    OnClientDisconnected onClientDisconnected = nullptr;    /// Callback para quando um cliente se desconecta
    std::unique_ptr<ProtocolAether::Parser> parser;         /// Parser de pacotes
};
