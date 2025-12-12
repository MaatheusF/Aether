#include "TcpServer.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>


/**
 * @brief Construtor do servidor TCP
 * @param port Porta na qual o servidor irá escutar
 */
TcpServer::TcpServer(const int port) : serverSocket(-1), serverPort(port), isRunning(false) {}

/** * Destrutor do servidor TCP */
TcpServer::~TcpServer()
{
    stop(); /// Para o servidor ao destruir
}

/** Inicia o servidor TCP */
void TcpServer::start()
{
    if (isRunning) return;
    isRunning = true;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);                 /// Cria o socket do servidor
    if (serverSocket < 0)
    {
        throw std::runtime_error("[Core TCP] Erro ao criar Socket do servidor");
    }


    sockaddr_in serverAddr{};                                       /// Estrutura para o endereço do servidor
    serverAddr.sin_family = AF_INET;                                /// Família de endereços IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;                        /// Aceita conexões de qualquer endereço
    serverAddr.sin_port = htons(static_cast<uint16_t>(serverPort)); /// Define a porta do servidor

    /// Faz o bind do socket do servidor ao endereço e porta especificados
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        close(serverSocket);    /// Fecha o socket em caso de erro
        throw std::runtime_error("[Core TCP] Erro ao fazer bind do Socket do servidor");
    }

    listen(serverSocket, 10); /// Coloca o servidor em modo de escuta

    acceptThread = std::thread(&TcpServer::acceptLoop, this); /// Inicia a thread para aceitar conexões de clientes
}

/** Para o servidor TCP */
void TcpServer::stop()
{
    if (!isRunning) return;
    isRunning = false;

shutdown(serverSocket, SHUT_RDWR);        /// Encerra as operações de leitura e escrita no socket do servidor
    close(serverSocket);                  /// Fecha o socket do servidor

    if (acceptThread.joinable())
    {
        acceptThread.join();              /// Aguarda a thread de aceitação terminar
    }

    for (auto &conn : connections)
    {
        conn->stop();  /// Fecha todas as conexões ativas
    }

    connections.clear(); /// Limpa a lista de threads de clientes
}

/** Loop para aceitar conexões de clientes */
void TcpServer::acceptLoop()
{
    while (isRunning)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr); /// Aceita uma nova conexão de cliente
        if (clientSocket < 0)
        {
            continue;   /// Continua se houver erro ao aceitar conexão
        }

        auto conn = std::make_shared<TcpConnection>(clientSocket);
        connections.insert(conn);

        if (onClientConnected)
        {
            onClientConnected(clientSocket); /// Chama o callback de conexão de cliente
        }

        conn->setOnMessage([this, conn](const std::string& msg)
        {
           if (onDataReceived)
           {
               onDataReceived(conn->getFd(), msg);
           }
        });

        conn->setOnDisconnect([this, conn]()
        {
            if (onClientDisconnected)
            {
                onClientDisconnected(conn->getFd());
            }
            connections.erase(conn);
        });

        conn->start(); /// Inicia a thread de comunicação com o cliente
    }
}