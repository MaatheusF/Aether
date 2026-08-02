#include "CameraService.hpp"
#include "../core/utils/Md5.hpp"
#include "../core/utils/logger.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <random>
#include <sstream>
#include <utility>

namespace Aether::Api
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    using tcp = boost::asio::ip::tcp;

    namespace
    {
        /**
         * Gera um cnonce (client nonce) aleatório exigido pelo Digest Auth
         * quando qop=auth está presente no desafio.
         */
        std::string generateCnonce()
        {
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_int_distribution<std::uint64_t> dist;

            std::ostringstream oss;
            oss << std::hex << dist(gen);
            return oss.str();
        }
    }

    CameraService::CameraService(CameraConfig config)
        : m_config(std::move(config))
    {
    }

    /**
     * Extrai um parâmetro do header WWW-Authenticate, que chega no formato:
     * Digest realm="...", nonce="...", qop="auth", opaque="...", ...
     * (qop as vezes vem sem aspas: qop=auth)
     */
    std::string CameraService::extractAuthParam(const std::string& header, const std::string& key)
    {
        const std::string quotedMarker = key + "=\"";
        auto pos = header.find(quotedMarker);

        if (pos != std::string::npos)
        {
            pos += quotedMarker.size();
            const auto end = header.find('"', pos);
            if (end == std::string::npos)
                return "";

            return header.substr(pos, end - pos);
        }

        const std::string bareMarker = key + "=";
        pos = header.find(bareMarker);
        if (pos == std::string::npos)
            return "";

        pos += bareMarker.size();
        const auto end = header.find_first_of(", ", pos);
        return header.substr(pos, end - pos);
    }

    /**
     * Monta o header Authorization: Digest ... (RFC 2617) a partir do
     * desafio recebido em WWW-Authenticate e das credenciais configuradas.
     */
    std::string CameraService::buildDigestAuthorizationHeader(
        const std::string& method,
        const std::string& uri,
        const std::string& wwwAuthenticateHeader) const
    {
        const std::string realm  = extractAuthParam(wwwAuthenticateHeader, "realm");
        const std::string nonce  = extractAuthParam(wwwAuthenticateHeader, "nonce");
        const std::string qop    = extractAuthParam(wwwAuthenticateHeader, "qop");
        const std::string opaque = extractAuthParam(wwwAuthenticateHeader, "opaque");

        const std::string ha1 = Aether::Core::Utils::Md5::hash(
            m_config.user + ":" + realm + ":" + m_config.password);

        const std::string ha2 = Aether::Core::Utils::Md5::hash(
            method + ":" + uri);

        const std::string cnonce = generateCnonce();
        const std::string nc = "00000001";

        std::string response;
        if (!qop.empty())
        {
            response = Aether::Core::Utils::Md5::hash(
                ha1 + ":" + nonce + ":" + nc + ":" + cnonce + ":" + qop + ":" + ha2);
        }
        else
        {
            response = Aether::Core::Utils::Md5::hash(
                ha1 + ":" + nonce + ":" + ha2);
        }

        std::ostringstream authHeader;
        authHeader << "Digest username=\"" << m_config.user << "\""
                   << ", realm=\"" << realm << "\""
                   << ", nonce=\"" << nonce << "\""
                   << ", uri=\"" << uri << "\""
                   << ", response=\"" << response << "\"";

        if (!qop.empty())
        {
            authHeader << ", qop=" << qop
                       << ", nc=" << nc
                       << ", cnonce=\"" << cnonce << "\"";
        }

        if (!opaque.empty())
        {
            authHeader << ", opaque=\"" << opaque << "\"";
        }

        return authHeader.str();
    }

    /**
     * Captura o snapshot JPEG da câmera autenticando via HTTP Digest:
     *
     * 1ª requisição (sem Authorization) -> câmera responde 401 com o desafio
     * 2ª requisição (com Authorization: Digest ...) -> câmera responde 200 com o JPEG
     */
    Dto::CameraSnapshotResponse CameraService::getSnapshot(int channel)
    {
        Dto::CameraSnapshotResponse result;
        const std::string uri = m_config.cgiPath + std::to_string(channel);

        try
        {
            boost::asio::io_context ioContext;
            tcp::resolver resolver(ioContext);
            const auto endpoints = resolver.resolve(m_config.host, std::to_string(m_config.port));

            std::string wwwAuthenticate;

            // 1a requisicao - obtem o desafio Digest (401 + WWW-Authenticate)
            {
                tcp::socket socket(ioContext);
                boost::asio::connect(socket, endpoints);

                http::request<http::empty_body> request{http::verb::get, uri, 11};
                request.set(http::field::host, m_config.host);
                request.set(http::field::user_agent, "Aether-Core");

                http::write(socket, request);

                beast::flat_buffer buffer;
                http::response<http::vector_body<std::uint8_t>> response;
                http::read(socket, buffer, response);

                if (response.result_int() != 401)
                {
                    // Camera nao exigiu autenticacao (ou ja respondeu a imagem direto)
                    result.httpStatus = response.result_int();
                    result.success = response.result_int() == 200;

                    if (result.success)
                    {
                        result.data = std::move(response.body());
                        if (response.count(http::field::content_type))
                            result.contentType = std::string(response[http::field::content_type]);
                    }
                    else
                    {
                        result.message = "Camera retornou status inesperado (" +
                            std::to_string(result.httpStatus) + ")";
                    }

                    beast::error_code ec;
                    socket.shutdown(tcp::socket::shutdown_both, ec);
                    return result;
                }

                wwwAuthenticate = std::string(response[http::field::www_authenticate]);

                beast::error_code ec;
                socket.shutdown(tcp::socket::shutdown_both, ec);
            }

            if (wwwAuthenticate.empty())
            {
                result.message = "Camera nao retornou header WWW-Authenticate";
                return result;
            }

            const std::string authorization =
                buildDigestAuthorizationHeader("GET", uri, wwwAuthenticate);

            // 2a requisicao - com Authorization: Digest ...
            tcp::socket socket(ioContext);
            boost::asio::connect(socket, endpoints);

            http::request<http::empty_body> request{http::verb::get, uri, 11};
            request.set(http::field::host, m_config.host);
            request.set(http::field::user_agent, "Aether-Core");
            request.set(http::field::authorization, authorization);

            http::write(socket, request);

            beast::flat_buffer buffer;
            http::response<http::vector_body<std::uint8_t>> response;
            http::read(socket, buffer, response);

            result.httpStatus = response.result_int();
            result.success = response.result_int() == 200;

            if (result.success)
            {
                result.data = std::move(response.body());

                if (response.count(http::field::content_type))
                    result.contentType = std::string(response[http::field::content_type]);
            }
            else
            {
                result.message = "Falha na autenticacao Digest ou na captura do snapshot (status " +
                    std::to_string(result.httpStatus) + ")";
            }

            beast::error_code ec;
            socket.shutdown(tcp::socket::shutdown_both, ec);
        }
        catch (const std::exception& e)
        {
            result.success = false;
            result.message = std::string("Erro ao conectar na camera: ") + e.what();
            AetherCoreLogger::Log(std::string("[CameraService] ") + result.message);
        }

        return result;
    }
}
