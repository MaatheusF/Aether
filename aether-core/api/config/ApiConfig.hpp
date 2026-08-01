#pragma once

#include "../common/HttpMethod.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <thread>
#include <unordered_set>

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

        /**
         * @brief Diretório onde cada requisição grava seu próprio arquivo
         * com o payload da requisição e o corpo da resposta completos.
         * @see AccessLogger
         */
        std::string requestDetailsDir = "/var/log/aether/api_requests";

        /**
         * @brief Métodos HTTP que geram entrada no access log + arquivo de detalhe.
         *
         * Requisições com método fora desse conjunto não geram linha
         * nenhuma no access log, nem arquivo de detalhe -- são ignoradas
         * pelo AccessLogger por completo (mas continuam sendo processadas
         * normalmente pelo Router). Default: todos os métodos suportados.
         *
         * Exemplo pra logar só GET (ex: ignorar POSTs de atuadores, que já
         * ficam registrados no log operacional de cada módulo):
         * @code
         *   config.loggedMethods = { HttpMethod::GET };
         * @endcode
         */
        std::unordered_set<HttpMethod> loggedMethods = {
            HttpMethod::GET,
            HttpMethod::POST,
            HttpMethod::PUT,
            HttpMethod::PATCH,
            HttpMethod::DELETE_,
        };

        /**
         * @brief Métodos HTTP que, além da linha resumo, também geram
         * arquivo de detalhe (payload + response completos).
         *
         * Só tem efeito pra métodos que já estão em loggedMethods -- um
         * método fora de loggedMethods nem chega a ser avaliado aqui,
         * já que é ignorado antes. Métodos em loggedMethods mas fora
         * desse conjunto continuam gerando a linha normal no access log,
         * só que com `detail=-` (sem arquivo de detalhe). Default: todos
         * os métodos suportados (mesmo comportamento de antes).
         *
         * Exemplo pra logar todo método na linha resumo, mas só gravar o
         * arquivo de detalhe pesado (payload/response) de POST/PUT/DELETE,
         * sem o volume extra de GET (ex: snapshots de câmera a cada
         * segundo, que já viram placeholder no detalhe mas nem precisam
         * do arquivo em si):
         * @code
         *   config.detailedMethods = { HttpMethod::POST, HttpMethod::PUT, HttpMethod::DELETE_ };
         * @endcode
         */
        std::unordered_set<HttpMethod> detailedMethods = {
            HttpMethod::GET,
            HttpMethod::POST,
            HttpMethod::PUT,
            HttpMethod::PATCH,
            HttpMethod::DELETE_,
        };
    };
}