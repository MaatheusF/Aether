#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

#include "ConnectionHandle.hpp"
#include "PostgresDriver.hpp"

/**
 * @brief Classe que gerencia um pool de conexões PostgresDriver.
 * Permite adquirir e liberar conexões de forma thread-safe.
 */
class ConnectionPool
{
public:

    /**
     * @brief Construtor que inicializa o pool com um número fixo de conexões.
     * @param connString String de conexão para o banco de dados.
     * @param poolSize Número de conexões a serem criadas no pool.
     */
    ConnectionPool(const std::string& connString, size_t poolSize) : connectionString(connString)
    {
        for (size_t i = 0; i < poolSize; i++)
        {
            auto* conn = new PostgresDriver(connString); // Cria nova conexão
            freeConnections.push(conn);                  // Adiciona à fila de conexões livres
        }
    }

    /**
     * @brief Destrutor que libera todas as conexões do pool.
     */
    ~ConnectionPool()
    {
        while (!freeConnections.empty())        // Enquanto houver conexões na fila
        {
            delete freeConnections.front();     // Libera memória da conexão
            freeConnections.pop();              // Remove da fila
        }
    }

    /**
     * @brief Adquire uma conexão do pool.
     * Bloqueia até que uma conexão esteja disponível.
     * @return ConnectionHandle que gerencia a conexão adquirida.
     */
    ConnectionHandle acquire()
    {
        std::unique_lock<std::mutex> lock(mutex);

        cv.wait(lock, [&] { return !freeConnections.empty(); }); // Aguarda até que haja conexões disponíveis

        PostgresDriver* conn = freeConnections.front();         // Obtém a conexão do topo da fila
        freeConnections.pop();                                  // Remove da fila

        return ConnectionHandle(conn, [&](PostgresDriver* c)
        {
            release(c);   // Retorna a conexão ao pool ao destruir o handle
        });
    }

private:

    /**
     * @brief Libera uma conexão de volta ao pool.
     * @param conn Ponteiro para a conexão a ser liberada.
     */
    void release(PostgresDriver* conn)
    {
        std::unique_lock<std::mutex> lock(mutex);
        freeConnections.push(conn);  // Adiciona a conexão de volta à fila
        cv.notify_one();             // Notifica uma thread aguardando por uma conexão
    }

    std::string connectionString;                /// String de conexão com o banco de dados
    std::queue<PostgresDriver*> freeConnections; /// Fila de conexões livres

    std::mutex mutex;                           /// Mutex para proteger o acesso ao pool
    std::condition_variable cv;                 /// Variável de condição para aguardar conexões disponíveis
};