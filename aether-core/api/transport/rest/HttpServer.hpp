#pragma once

#include "Router.hpp"
#include "../../config/ApiConfig.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <vector>

namespace Aether::Api
{
    /**
     * @brief Servidor HTTP assíncrono com suporte a conexões concorrentes
     *
     * Servidor HTTP com Boost.Asio/Beast que:
     * - Aceita conexões TCP em host/port configurável, de forma assíncrona
     *   (async_accept) — nunca fica bloqueado esperando a próxima conexão
     * - Processa requisições HTTP via Boost.Beast, com leitura/escrita
     *   assíncronas (ver HttpSession)
     * - Roda o io_context num pool de threads (ApiConfig::threads,
     *   incluindo a própria thread chamadora de start()) — múltiplas
     *   conexões avançam em paralelo de verdade, uma conexão lenta não
     *   trava as demais
     *
     * Usa RAII (Resource Acquisition Is Initialization) para garantir
     * limpeza apropriada de recursos.
     *
     * @example
     * @code
     *   ApiConfig config;
     *   config.host = "0.0.0.0";
     *   config.port = 9001;
     *
     *   HttpServer server(config);
     *   server.start();  // Bloqueante do ponto de vista do chamador,
     *                     // mas internamente processa várias conexões
     *                     // em paralelo
     * @endcode
     *
     * @see HttpSession - gerencia conexões individuais
     * @see Router - processa requisições
     */
    class HttpServer
    {
    public:
        /**
         * @brief Construtor - inicializa servidor com configuração
         *
         * Cria o acceptor ASIO porém NÃO começa a escutar ainda.
         * Chame start() para iniciar.
         *
         * @param config Configuração com host, porta e nº de threads
         * @throws std::exception Se falhar ao criar acceptor
         */
        explicit HttpServer(const ApiConfig& config);

        /**
         * @brief Destrutor - para o io_context e aguarda as threads do pool
         */
        ~HttpServer();

        /**
         * @brief Inicia servidor e começa a escutar (bloqueante)
         *
         * Sobe (config.threads - 1) threads extras rodando
         * io_context::run(), e a própria thread chamadora entra no
         * io_context também — todas processam conexões em paralelo.
         * Só retorna quando o io_context para (ex: no destrutor).
         *
         * @throws std::runtime_error Se erro ao iniciar
         */
        void start();

    private:
        /**
         * @brief Agenda a próxima aceitação assíncrona de conexão
         *
         * - async_accept não bloqueia: agenda o callback e retorna
         * - Ao aceitar, cria a HttpSession (via shared_ptr) e chama run()
         * - Sempre reagenda a próxima aceitação, mesmo em caso de erro —
         *   senão o servidor para de aceitar novas conexões após a
         *   primeira falha
         */
        void doAccept();

    private:
        ApiConfig m_config;                                         /**< Configuração (host, porta, threads) */
        boost::asio::io_context m_ioContext;                        /**< Contexto ASIO, compartilhado por todas as threads do pool */
        boost::asio::ip::tcp::acceptor m_acceptor;                  /**< Acceptor TCP */
        Router m_router;                                            /**< Router de requisições, compartilhado entre sessões/threads */
        std::vector<std::thread> m_threadPool;                      /**< Threads extras rodando io_context::run() */
    };
}
