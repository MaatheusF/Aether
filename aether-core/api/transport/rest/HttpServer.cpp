#include "HttpServer.hpp"
#include "HttpSessions.hpp"

#include <iostream>
#include <boost/asio/ip/address.hpp>

namespace Aether::Api
{
    /**
     * Inicializa o servidor HTTP
     *
     * Cria:
     * - io_context para ASIO
     * - acceptor TCP escutando em host:port
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
    }

    /**
     * Inicia servidor e começa a escutar por conexões
     *
     * Fluxo:
     * 1. Printa mensagem de inicialização
     * 2. Loop infinito chamando accept()
     * 3. Para cada conexão:
     *    - Cria HttpSession
     *    - Executa session.run()
     *    - Aguarda próxima conexão
     *
     * @note Método bloqueante. Para uso em produção com
     *       múltiplas conexões reais, use threads ou ASIO async
     */
    void HttpServer::start()
    {
        std::cout << "HTTP Server iniciado em "
                  << m_config.host << ":"
                  << m_config.port << std::endl;

        while (true)
        {
            accept();
        }
    }

    /**
     * Aceita uma conexão TCP e a processa
     *
     * Passos:
     * 1. Cria socket TCP
     * 2. Aceita conexão do acceptor
     * 3. Cria HttpSession passando socket e router
     * 4. Executa sessão (processa requisição e responde)
     * 5. Retorna e aguarda próxima conexão
     *
     * Erros durante aceitação são capturados e logados
     * sem interromper o servidor.
     */
    void HttpServer::accept()
    {
        try
        {
            boost::asio::ip::tcp::socket socket(m_ioContext);

            m_acceptor.accept(socket);

            HttpSession session(
                std::move(socket),
                m_router);

            session.run();
        }
        catch (const std::exception& ex)
        {
            std::cerr
                << "Erro ao aceitar conexão: "
                << ex.what()
                << std::endl;
        }
    }
}