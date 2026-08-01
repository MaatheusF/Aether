#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <thread>

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

        /**
         * @brief Quantidade de threads que executam o io_context da API.
         *
         * Este valor é o total de requisições que a API processa de fato em paralelo.
         *
         * Default: nº de cores da máquina, com piso de 2 — mesmo em
         * hardware modesto, isso já evita que uma conexão lenta trave as demais.
         */
        unsigned int threads = std::max(2u, std::thread::hardware_concurrency());

        /**
         * @brief Caminho do arquivo de log de acesso (estilo access.log do Apache).
         * @see AccessLogger
         */
        std::string accessLogPath = "/var/log/aether/aether_api_access.log";
    };
}