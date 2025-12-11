#pragma once

#include <functional>
#include "PostgresDriver.hpp"

/** * @brief Gerencia o ciclo de vida de uma conexão ao banco de dados PostgreSQL.
 *
 * Esta classe encapsula um ponteiro para um `PostgresDriver` e garante que a
 * conexão seja devolvida ao pool de conexões quando o objeto `ConnectionHandle`
 * for destruído. Isso é feito através de um callback fornecido no momento da
 * criação do objeto.
 */
class ConnectionHandle
{
public:
    /// Tipo do callback de devolução da conexão
    using ReturnCallback = std::function<void(PostgresDriver*)>;

    /// Construtor → recebe o driver e o callback de devolução
    ConnectionHandle(PostgresDriver* driver, ReturnCallback callback)
        : driver(driver), returnCallback(std::move(callback)) {}

    // Desabilita cópia para evitar múltiplas devoluções da mesma conexão ao pool de conexões
    /*
    ConnectionHandle(const ConnectionHandle&) = delete;
    ConnectionHandle& operator=(const ConnectionHandle&) = delete;

    ConnectionHandle(ConnectionHandle&& other) noexcept
        : driver(other.driver),
          returnCallback(std::move(other.returnCallback))
    {
        other.driver = nullptr; // Move-safe
    }*/

    /**
     * Move constructor
     * @brief Transfere a posse da conexão de outro `ConnectionHandle` para este.
     */
    ConnectionHandle& operator=(ConnectionHandle&& other) noexcept
    {
        if (this != &other)
        {
            // Devolve a conexão atual (se existir)
            if (driver)
                returnCallback(driver);

            driver = other.driver;
            returnCallback = std::move(other.returnCallback);

            other.driver = nullptr;
        }
        return *this;
    }

    // Destrutor → devolve conexão ao pool
    ~ConnectionHandle()
    {
        if (driver)
            returnCallback(driver);
    }

    // Acesso ao driver
    PostgresDriver* operator->() const { return driver; }

    // Permite usar `if (conn)`
    explicit operator bool() const { return driver != nullptr; }

private:
    PostgresDriver* driver;
    ReturnCallback returnCallback;
};
