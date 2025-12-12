#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <thread>

class TcpConnection
{
public:
    using OnMessage = std::function<void(const std::string&)>; /// Callback para mensagem recebida
    using OnDisconnect = std::function<void()>;                /// Callback para desconexão

    /**
     * Construtor da conexão TCP
     * @param socketFd Descritor do socket da conexão
     */
    explicit TcpConnection(int socketFd);

    /**
     * Destrutor da conexão TCP
     */
    ~TcpConnection();

    void start();                               /// Inicia a conexão TCP
    void stop();                                /// Inicia e para a conexão TCP
    void send(const std::string& msg) const;    /// Envia uma mensagem pela conexão

    /** Retorna o descritor do socket da conexão */
    int getFd() const { return socketFd; }

    void setOnMessage(const OnMessage& cb) { onMessage = cb; }          /// Define o callback para mensagem recebida
    void setOnDisconnect(const OnDisconnect& cb) { onDisconnect = cb; } /// Define o callback para desconexão



private:

    /** Tipo de callback para mensagem recebida */
    void readLoop();

    int socketFd;                   /// Socket da conexão
    std::atomic<bool> isRunning;    /// Indica se a conexão está ativa
    std::thread readThread;         /// Thread para leitura de dados

    OnMessage onMessage;            /// Callback para mensagem recebida
    OnDisconnect onDisconnect;      /// Callback para desconexão
};