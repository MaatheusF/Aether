#pragma once

#include <cstdint>
#include <string>

namespace Aether::Api
{
    /**
     * @brief Configuração da API HTTP
     *
     * Define parâmetros de inicialização do servidor HTTP como
     * host e porta de escuta.
     *
     * @example
     * @code
     *   ApiConfig config;
     *   config.host = "0.0.0.0";  // Escuta todas as interfaces
     *   config.port = 9001;       // Porta 9001
     *   HttpServer server(config);
     *   server.start();
     * @endcode
     */
    struct ApiConfig
    {
        std::string host = "0.0.0.0";  /**< Host/IP para escutar as requisições (0.0.0.0 = todas as interfaces) */
        uint16_t port = 9001;          /**< Porta */
    };
}