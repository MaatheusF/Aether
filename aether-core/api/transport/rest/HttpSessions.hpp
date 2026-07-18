#pragma once

#include "Router.hpp"

#include "../../common/HttpRequest.hpp"
#include "../../common/HttpResponse.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace Aether::Api
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    using tcp = boost::asio::ip::tcp;

    /**
     * @brief Gerenciador de sessão HTTP individual
     *
     * Uma sessão HTTP representa uma conexão TCP entre cliente e servidor.
     * Responsável por:
     * 1. Ler requisição HTTP do socket TCP usando Boost.Beast
     * 2. Converter para estrutura interna HttpRequest
     * 3. Passar para Router para processamento
     * 4. Converter resposta para formato Boost.Beast
     * 5. Enviar resposta ao cliente
     *
     * Cada sessão é criada quando uma nova conexão é aceita pelo
     * HttpServer e é destruída após resposta ser enviada.
     *
     * @see HttpServer - quem cria as sessões
     * @see Router - quem processa as requisições
     *
     * @example
     * @code
     *   tcp::socket socket(io_context);
     *   acceptor.accept(socket);
     *   HttpSession session(std::move(socket), router);
     *   session.run();  // Processa e responde
     * @endcode
     */
    class HttpSession
    {
    public:
        /**
         * @brief Construtor - inicializa sessão com socket
         * @param socket Socket TCP da conexão (movido para esta sessão)
         * @param router Referência ao router para despachar requisições
         */
        HttpSession(
            tcp::socket socket,
            Router& router);

        /**
         * @brief Executa a sessão (lê request, processa, envia response)
         *
         * Método bloqueante que:
         * 1. Lê requisição HTTP do socket
         * 2. Cria HttpRequest interna
         * 3. Despachana via router
         * 4. Converte resposta para Boost.Beast
         * 5. Escreve resposta no socket
         * 6. Fecha conexão gracefully
         */
        void run();

    private:
        /**
         * @brief Converte requisição Boost.Beast para HttpRequest interna
         * @param request Requisição no formato Boost.Beast
         * @return HttpRequest convertida para formato interno
         */
        HttpRequest createRequest(
            const http::request<http::string_body>& request);

        /**
         * @brief Converte resposta interna para formato Boost.Beast
         * @param response HttpResponse interna
         * @return Resposta formatada para transmissão HTTP
         */
        http::response<http::string_body> createResponse(
            const HttpResponse& response);

    private:
        tcp::socket m_socket;   /**< Socket TCP da conexão */
        Router& m_router;       /**< Referência do router para processar requisições */
    };
}