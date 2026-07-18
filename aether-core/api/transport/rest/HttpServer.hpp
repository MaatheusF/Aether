#pragma once

#include "Router.hpp"
#include "../../config/ApiConfig.hpp"
#include <boost/asio.hpp>

namespace Aether::Api
{
    /**
     * @brief Servidor HTTP com suporte a múltiplas conexões
     *
     * Servidor HTTP com Boost.ASIO que:
     * - Aceita conexões TCP em host/port configurável
     * - Processa requisições HTTP via Boost.Beast
     * - Despachana requisições para rotas apropriadas
     * - Suporta múltiplas conexões simultâneas
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
     *   server.start();  // Bloqueante, escuta indefinidamente
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
         * @param config Configuração com host e porta
         * @throws std::exception Se falhar ao criar acceptor
         */
        explicit HttpServer(const ApiConfig& config);

        /**
         * @brief Destrutor - garante limpeza de recursos
         */
        ~HttpServer() = default;

        /**
         * @brief Inicia servidor e começa a escutar (bloqueante)
         *
         * Método bloqueante que continuará escutando indefinidamente.
         * Aceita conexões e cria HttpSession para cada uma.
         *
         * @throws std::runtime_error Se erro ao iniciar ou aceitar conexões
         *
         * @note Em produção, considere usar threads ou ASIO async
         */
        void start();

    private:
        /**
         * @brief Aceita uma única conexão e a processa
         *
         * - Aceita socket TCP
         * - Cria HttpSession passando socket e router
         * - Executa sessão (run)
         * - Repete para próxima conexão
         */
        void accept();

    private:
        ApiConfig m_config;                                         /**< Configuração (host, porta) */
        boost::asio::io_context m_ioContext;                       /**< Contexto ASIO */
        boost::asio::ip::tcp::acceptor m_acceptor;                /**< Acceptor TCP */
        Router m_router;                                            /**< Router de requisições */
    };
}
