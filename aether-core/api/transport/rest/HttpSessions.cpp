#include "HttpSessions.hpp"
#include "AccessLogger.hpp"

#include <chrono>
#include <iostream>

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
     * Dispara o ciclo assíncrono da sessão. Não bloqueia — só agenda a
     * primeira leitura e retorna; o resto acontece nos callbacks abaixo.
     */
    void HttpSession::run()
    {
        doRead();
    }

    /**
     * Agenda a leitura assíncrona da próxima requisição nesta conexão.
     * m_request é resetada antes de cada leitura, senão o corpo da
     * requisição anterior ficaria acumulado (relevante em conexões
     * keep-alive, que reaproveitam a mesma sessão pra várias requisições).
     */
    void HttpSession::doRead()
    {
        m_request = {};

        auto self = shared_from_this();

        http::async_read(
            m_socket,
            m_buffer,
            m_request,
            [self](beast::error_code ec, std::size_t bytesTransferred)
            {
                self->onRead(ec, bytesTransferred);
            });
    }

    /**
     * Callback de leitura concluída:
     * 1. Converte para HttpRequest interna
     * 2. Despacha via Router (síncrono, roda nesta mesma thread do pool)
     * 3. Loga a requisição no AccessLogger (método, rota, status, corpos,
     *    duração do dispatch) -- ver AccessLogger para o formato
     * 4. Converte resposta para Boost.Beast
     * 5. Escreve a resposta de forma assíncrona
     *
     * end_of_stream (cliente fechou a conexão) e demais erros só encerram
     * esta sessão — não derrubam o servidor nem afetam outras conexões.
     */
    void HttpSession::onRead(beast::error_code ec, std::size_t /*bytesTransferred*/)
    {
        if (ec == http::error::end_of_stream)
        {
            doClose();
            return;
        }

        if (ec)
        {
            std::cerr << "Erro ao ler requisicao: " << ec.message() << std::endl;
            return;
        }

        auto request = createRequest(m_request);

        const auto dispatchStart = std::chrono::steady_clock::now();
        auto response = m_router.dispatch(request);
        const auto dispatchDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - dispatchStart);

        boost::system::error_code endpointEc;
        const auto endpoint = m_socket.remote_endpoint(endpointEc);
        const std::string clientIp = endpointEc ? "desconhecido" : endpoint.address().to_string();

        AccessLogger::Log(clientIp, request, response, dispatchDuration);

        auto beastResponse =
            std::make_shared<http::response<http::string_body>>(createResponse(response));

        const bool keepAlive = m_request.keep_alive();
        beastResponse->keep_alive(keepAlive);

        auto self = shared_from_this();

        // beastResponse é capturado no shared_ptr da lambda pra continuar
        // vivo durante toda a escrita assíncrona.
        http::async_write(
            m_socket,
            *beastResponse,
            [self, beastResponse, keepAlive](beast::error_code ec, std::size_t bytesTransferred)
            {
                self->onWrite(ec, bytesTransferred, !keepAlive);
            });
    }

    /**
     * Callback de escrita concluída. Se a conexão for keep-alive, volta a
     * ler a próxima requisição na mesma sessão; senão, encerra.
     */
    void HttpSession::onWrite(beast::error_code ec, std::size_t /*bytesTransferred*/, bool close)
    {
        if (ec)
        {
            std::cerr << "Erro ao escrever resposta: " << ec.message() << std::endl;
            return;
        }

        if (close)
        {
            doClose();
            return;
        }

        doRead();
    }

    /**
     * Encerra a conexão graciosamente (half-close do lado de escrita).
     */
    void HttpSession::doClose()
    {
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