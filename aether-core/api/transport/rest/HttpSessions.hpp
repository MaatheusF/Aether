#pragma once

#include "Router.hpp"

#include "../../common/HttpRequest.hpp"
#include "../../common/HttpResponse.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>

namespace Aether::Api
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    using tcp = boost::asio::ip::tcp;

    /**
     * @brief Gerenciador de sessão HTTP individual (assíncrono)
     *
     * Uma sessão HTTP representa uma conexão TCP entre cliente e servidor.
     * Toda leitura/escrita no socket usa async_read/async_write — a sessão
     * nunca bloqueia a thread do io_context esperando dados de rede, então
     * várias sessões avançam concorrentemente mesmo com poucas threads no
     * pool (ver HttpServer::start()).
     *
     * Vive como std::shared_ptr (via enable_shared_from_this): cada
     * callback assíncrono mantém a sessão viva capturando esse shared_ptr,
     * e ela só é destruída quando não sobra nenhuma operação pendente no
     * socket. Por isso HttpServer sempre a cria com std::make_shared,
     * nunca como valor/stack.
     *
     * O processamento em si (Router::dispatch) continua síncrono — roda
     * dentro do callback de leitura, na thread do io_context que o
     * recebeu. Isso é suficiente pra não travar as demais conexões (cada
     * uma tem sua própria sessão/callback), mas uma chamada de negócio
     * muito lenta (ex: câmera respondendo devagar) ainda ocupa aquela
     * thread do pool até terminar.
     *
     * @see HttpServer - quem cria as sessões (async_accept)
     * @see Router - quem processa as requisições
     */
    class HttpSession : public std::enable_shared_from_this<HttpSession>
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
         * @brief Inicia o ciclo assíncrono da sessão (dispara a 1ª leitura)
         *
         * Não bloqueia: agenda a leitura no io_context e retorna. O resto
         * do ciclo (processar, responder, ler a próxima requisição se a
         * conexão for keep-alive) continua encadeado via callbacks.
         */
        void run();

    private:
        /** @brief Agenda a leitura assíncrona da próxima requisição. */
        void doRead();

        /** @brief Callback ao terminar de ler a requisição. */
        void onRead(beast::error_code ec, std::size_t bytesTransferred);

        /** @brief Callback ao terminar de escrever a resposta. */
        void onWrite(beast::error_code ec, std::size_t bytesTransferred, bool close);

        /** @brief Encerra a conexão graciosamente. */
        void doClose();

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
        tcp::socket m_socket;                             /**< Socket TCP da conexão */
        Router& m_router;                                 /**< Referência do router para processar requisições */
        beast::flat_buffer m_buffer;                       /**< Buffer de leitura, reaproveitado entre requisições da mesma conexão */
        http::request<http::string_body> m_request;        /**< Requisição sendo lida no momento */
    };
}