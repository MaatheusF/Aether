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

/**
 * Inicia a conexão TCP
 */
void TcpConnection::start()
{
    isRunning = true;                                          /// Marca a conexão como ativa
    readThread = std::thread(&TcpConnection::readLoop, this);  /// Inicia a thread de leitura
}

/**
 * Realiza o encerramento da conexão TCP
 */
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

/**
 * Loop de leitura de dados do socket
 */
void TcpConnection::readLoop()
{
    uint8_t buffer[1024]; /// Buffer para leitura de dados
    while (isRunning)
    {
        ssize_t bytesRead = recv(socketFd, buffer, sizeof(buffer), 0); /// Recebe dados do socket
        if (bytesRead <= 0)
        {
            break;
        }
        std::cout << "[TcpConnection] recv() bytes=" << buffer << std::endl;
        onSocketRead(buffer, bytesRead); /// Chama a função onSocketRead para processar os bytes recebidos
    }

    isRunning = false;

    if (onDisconnect)
    {
        onDisconnect();
    }

    close(socketFd);
}

/**
 * Processa os dados recebidos do socket
 * @param data Ponteiro para os dados recebidos (Buffer, string de dados)
 * @param len Tamanho dos dados recebidos (Tamanho da String)
 */
void TcpConnection::onSocketRead(const uint8_t* data, size_t len)
{
    recvBuffer.insert(recvBuffer.end(), data, data + len);  /// Adiciona os dados recebidos ao buffer

    if (onBytesReceived)
    {
        std::cout << "[TcpConnection] chamando onBytesReceived" << std::endl;
        onBytesReceived(recvBuffer); /// Passa o buffer completo para o callback
        recvBuffer.clear();          /// Limpa o buffer após o processamento
    }

    if (!onBytesReceived)
    {
        std::cout << "[TcpConnection] onBytesReceived not set\n";
    }
}

/**
 * Envia dados através do socket TCP (Servidor -> Cliente)
 * @param data Vetor de bytes a serem enviados
 */
void TcpConnection::sendBytes(const std::vector<uint8_t>& data) const
{
    ssize_t bytesSent = ::send(socketFd, data.data(), data.size(), 0);  /// Envia os dados pelo socket
    if (bytesSent < 0)
    {
        std::cerr << "[TcpConncetion] Erro ao enviar dados para o cliente!" << std::endl;
    }
}