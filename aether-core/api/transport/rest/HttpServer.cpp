#include "HttpServer.hpp"
#include "HttpSessions.hpp"
#include "AccessLogger.hpp"

#include <iostream>
#include <memory>
#include <boost/asio/ip/address.hpp>

namespace Aether::Api
{
    /**
     * Inicializa o servidor HTTP
     *
     * Cria:
     * - io_context para ASIO
     * - acceptor TCP escutando em host:port
     * - log de acesso (AccessLogger), aberto já na construção pra estar
     *   pronto antes da primeira requisição chegar
     *
     * O servidor não começa a escutar até start() ser chamado.
     */
    HttpServer::HttpServer(const ApiConfig& config)
        : m_config(config),
          m_ioContext(),
          m_acceptor(
              m_ioContext,
              boost::asio::ip::tcp::endpoint(
                  boost::asio::ip::make_address(config.host),
                  config.port))
    {
        AccessLogger::Initialize(
            config.accessLogPath,
            config.requestDetailsDir,
            config.loggedMethods,
            config.detailedMethods);
    }

    /**
     * Para o io_context e aguarda as threads do pool encerrarem, evitando
     * threads soltas ou acesso a membros já destruídos.
     */
    HttpServer::~HttpServer()
    {
        m_ioContext.stop();

        for (auto& thread : m_threadPool)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    /**
     * Inicia servidor e começa a escutar por conexões
     *
     * Fluxo:
     * 1. Agenda a primeira aceitação assíncrona (doAccept)
     * 2. Sobe (threads - 1) threads extras rodando io_context::run()
     * 3. A própria thread chamadora também roda io_context::run()
     *
     * Como doAccept() sempre reagenda a próxima aceitação, o io_context
     * nunca fica sem trabalho — todas as threads (pool + chamadora) ficam
     * bloqueadas em run() processando conexões até o servidor ser parado
     * (~HttpServer). Do ponto de vista de quem chama start(), o
     * comportamento observável é o mesmo de antes (bloqueante); a
     * diferença é que agora várias conexões avançam em paralelo nas
     * threads do pool, em vez de uma de cada vez.
     */
    void HttpServer::start()
    {
        std::cout << "HTTP Server iniciado em "
                  << m_config.host << ":"
                  << m_config.port
                  << " (" << m_config.threads << " threads)" << std::endl;

        doAccept();

        const unsigned int extraThreads =
            m_config.threads > 0 ? m_config.threads - 1 : 0;

        m_threadPool.reserve(extraThreads);
        for (unsigned int i = 0; i < extraThreads; ++i)
        {
            m_threadPool.emplace_back([this]() { m_ioContext.run(); });
        }

        m_ioContext.run();
    }

    /**
     * Agenda a aceitação assíncrona da próxima conexão TCP
     *
     * Passos:
     * 1. async_accept não bloqueia: registra o callback e retorna na hora
     * 2. Quando uma conexão chega (em qualquer thread do pool), cria a
     *    HttpSession via shared_ptr e chama run() — a sessão então cuida
     *    de si mesma via I/O assíncrono (ver HttpSession)
     * 3. Reagenda a próxima aceitação, com sucesso ou erro — senão o
     *    servidor pararia de aceitar novas conexões após a primeira falha
     */
    void HttpServer::doAccept()
    {
        m_acceptor.async_accept(
            [this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<HttpSession>(std::move(socket), m_router)->run();
                }
                else
                {
                    std::cerr
                        << "Erro ao aceitar conexão: "
                        << ec.message()
                        << std::endl;
                }

                doAccept();
            });
    }
}