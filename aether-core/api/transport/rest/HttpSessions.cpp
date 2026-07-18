#include "HttpSessions.hpp"

namespace Aether::Api
{
    /**
     * Inicializa a sessão com socket e roteador
     */
    HttpSession::HttpSession(
        tcp::socket socket,
        Router& router)
        :
        m_socket(std::move(socket)),
        m_router(router)
    {
    }

    /**
     * Executa o ciclo completo da sessão HTTP:
     *
     * 1. Cria buffer para leitura
     * 2. Lê requisição HTTP do socket com Boost.Beast
     * 3. Converte para HttpRequest interna
     * 4. Despachana via Router
     * 5. Converte resposta para formato Boost.Beast
     * 6. Escreve resposta no socket
     * 7. Fecha gracefully
     */
    void HttpSession::run()
    {
        beast::flat_buffer buffer;

        http::request<http::string_body> beastRequest;

        http::read(
            m_socket,
            buffer,
            beastRequest);

        auto request =
            createRequest(beastRequest);

        auto response =
            m_router.dispatch(request);

        auto beastResponse =
            createResponse(response);

        http::write(
            m_socket,
            beastResponse);

        beast::error_code ec;

        m_socket.shutdown(
            tcp::socket::shutdown_send,
            ec);
    }

    /**
     * Converte requisição Boost.Beast para formato interno
     *
     * Mapeia:
     * - Método HTTP (GET, POST, PATCH, DELETE, PUT)
     * - Path (target da requisição)
     * - Headers (name/value pairs)
     * - Corpo (body da requisição)
     */
    HttpRequest HttpSession::createRequest(
    const http::request<http::string_body>& req)
    {
        HttpRequest request;

        switch(req.method())
        {
        case http::verb::get:
            request.method = HttpMethod::GET;
            break;

        case http::verb::post:
            request.method = HttpMethod::POST;
            break;

        case http::verb::put:
            request.method = HttpMethod::PUT;
            break;

        case http::verb::delete_:
            request.method = HttpMethod::DELETE_;
            break;

        default:
            request.method = HttpMethod::UNKNOWN;
            break;
        }

        request.path = std::string(req.target());

        request.body = req.body();

        for(auto const& header : req)
        {
            request.headers.emplace(
                std::string(header.name_string()),
                std::string(header.value()));
        }

        return request;
    }

    /**
     * Converte resposta interna para formato Boost.Beast
     *
     * Mapeia:
     * - Status code para http::status
     * - Headers customizados
     * - Body da resposta
     * - Versão HTTP (1.1)
     */
    http::response<http::string_body>
HttpSession::createResponse(
    const HttpResponse& response)
    {
        http::response<http::string_body> res;

        res.version(11);

        res.result(
            static_cast<http::status>(
                response.status));

        res.body() =
            response.body;

        for(auto& header : response.headers)
        {
            res.set(
                header.first,
                header.second);
        }

        res.prepare_payload();

        return res;
    }

}