#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <thread>

class TcpConnection
{
public:
    using OnBytes = std::function<void(std::vector<uint8_t>&)>;               /// Callback para mensagem recebida (dados binários)
    using OnDisconnect = std::function<void()>;                               /// Callback para desconexão

    /**
     * Construtor da conexão TCP
     * @param socketFd Descritor do socket da conexão
     */
    explicit TcpConnection(int socketFd);

    /**
     * Destrutor da conexão TCP
     */
    ~TcpConnection();

    void start();                                           /// Inicia a conexão TCP
    void stop();                                            /// Inicia e para a conexão TCP
    void sendBytes(const std::vector<uint8_t>& data) const; /// Envia uma mensagem pela conexão (Servidor para cliente)

    /** Retorna o descritor do socket da conexão */
    int getFd() const { return socketFd; }

    void setOnBytesReceived(const OnBytes& cb) { onBytesReceived = cb; }    /// Define o callback para dados recebidos
    void setOnDisconnect(const OnDisconnect& cb) { onDisconnect = cb; }     /// Define o callback para desconexão

private:
    void readLoop();                                                        /// Loop de leitura de dados do socket
    void onSocketRead(const uint8_t* data, size_t len);                     /// Manipula os dados lidos do socket

    int socketFd;                   /// Socket da conexão
    std::atomic<bool> isRunning;    /// Indica se a conexão está ativa
    std::thread readThread;         /// Thread para leitura de dados

    std::vector<uint8_t> recvBuffer;/// Buffer de recepção de dados
    OnBytes onBytesReceived;        /// Callback para mensagem recebida
    OnDisconnect onDisconnect;      /// Callback para desconexão
};