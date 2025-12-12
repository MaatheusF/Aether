#include "TcpConnection.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>

/**
 * Construtor da conexão TCP
 * @param socketFd Descritor do socket da conexão
 */
TcpConnection::TcpConnection(int socketFd) : socketFd(socketFd), isRunning(false) {}

/** Destrutor da conexão TCP */
TcpConnection::~TcpConnection()
{
    stop(); /// Para a conexão ao destruir
}

void TcpConnection::start()
{
    isRunning = true;                                          /// Marca a conexão como ativa
    readThread = std::thread(&TcpConnection::readLoop, this);  /// Inicia a thread de leitura
}

/** Para a conexão TCP */
void TcpConnection::stop()
{
    if (!isRunning) return;
    isRunning = false;

    shutdown(socketFd, SHUT_RDWR);  /// Encerra as operações de leitura e escrita no socket
    close(socketFd);                /// Fecha o socket da conexão

    if (readThread.joinable())
    {
        readThread.join(); /// Aguarda a thread de leitura terminar
    }
}

/** Looping de recebimento dos dados */
void TcpConnection::readLoop()
{
    char buffer[1024]; /// Buffer para leitura de dados
    while (isRunning)
    {
        ssize_t bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0); /// Lê dados do socket
        if (bytesRead <= 0)
        {
            break;
        }

        if (onMessage)
        {
            onMessage(std::string(buffer, bytesRead)); /// Chama o callback de mensagem recebida
        }
    }

    isRunning = false;

    if (onDisconnect)
    {
        onDisconnect();
    }

    close(socketFd);
}
