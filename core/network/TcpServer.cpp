#include "TcpServer.hpp"
#include "../../protocols/aether/include/Parser.hpp"
#include "TcpResponseChannel.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Construtor do servidor TCP
 * @param port Porta na qual o servidor irá escutar
 */
TcpServer::TcpServer(const int port) : serverSocket(-1), serverPort(port), isRunning(false), parser(std::make_unique<ProtocolAether::Parser>()) {}

/** Destrutor do servidor TCP */
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

/**
 * @brief Define o manipulador de protocolo utilizado pelo servidor TCP.
 *
 * O TcpServer mantém um std::shared_ptr para garantir o ciclo de vida
 * do IProtocolHandler durante toda a execução do servidor.
 *
 * O Parser recebe apenas um ponteiro cru (non-owning pointer),
 * sendo responsável apenas por chamar o handler quando um pacote
 * completo é parseado, sem gerenciar sua memória.
 */
void TcpServer::setProtocolHandler(std::shared_ptr<IProtocolHandler> handler)
{
    protocolHandler = handler;              // mantém vivo
    parser->setHandler(handler.get());      // apenas observa
}

/** Para o servidor TCP */
void TcpServer::stop()
{
    if (!isRunning) return;
    isRunning = false;

    shutdown(serverSocket, SHUT_RDWR);    /// Encerra as operações de leitura e escrita no socket do servidor
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

        /// Cria uma nova conexão TCP para o cliente
        auto conn = std::make_shared<TcpConnection>(clientSocket);
        /// Adiciona a conexão à lista de conexões ativas
        connections.insert(conn);


        /// Cria o canal de resposta e a sessão para este cliente
        auto channel = std::make_shared<TcpResponseChannel>(conn);
        auto session = std::make_shared<ConnSession>(channel);
        sessions[clientSocket] = session;

        /// Quando o handshake completar, o feed normal do parser é liberado
        session->setOnHandshakeComplete([this, channel](std::vector<uint8_t>& bytes)
        {
            parser->feed(bytes, channel);
        });

        /// Quando o handshake falhar, encerra a conexão TCP
        session->setOnHandshakeFailed([this, conn]()
        {
            std::cout << "[TcpServer] Encerrando conexão após falha no handshake fd=" << conn->getFd() << std::endl;
            sessions.erase(conn->getFd());
            connections.erase(conn);
            shutdown(conn->getFd(), SHUT_RDWR);
        });

        /// Chama o callback de conexão de cliente
        if (onClientConnected)
        {
            onClientConnected(clientSocket);
        }

        /** Define o callback para recebimento de bytes */
        conn->setOnBytesReceived([session](std::vector<uint8_t>& bytes)
        {
            session->feed(bytes); /// A sessão decide: handshake ou pipeline normal
        });

        /** Define o callback para desconexão do cliente */
        conn->setOnDisconnect([this, conn]()
        {
            if (onClientDisconnected)
            {
                onClientDisconnected(conn->getFd());
            }
            sessions.erase(conn->getFd()); /// Remove a sessão ao desconectar
            connections.erase(conn);
        });

        /** Inicia a conexão para começar a receber dados */
        conn->start();
    }
}