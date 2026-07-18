#pragma once

#include "HttpMethod.hpp"
#include <string>
#include <unordered_map>

namespace Aether::Api
{
    /**
     * @brief Estrutura que representa uma requisição HTTP
     *
     * Contém todas as informações extraídas da requisição HTTP para
     * processamento interno pela API. Convertida de um objeto Boost.Beast
     * pela classe HttpSession.
     *
     * @see HttpSession::createRequest()
     * @see Router::dispatch()
     */
    struct HttpRequest
    {
        HttpMethod method = HttpMethod::UNKNOWN;              /**< Modelo da requisição HTTP (GET, POST, etc) */
        std::string path;                                     /**< Caminho da requisição (ex: /api/status) */
        std::string body;                                     /**< Corpo da requisição (POST/PUT/PATCH) */
        std::unordered_map<std::string,std::string> headers;  /**< Headers HTTP (User-Agent, Content-Type, etc) */
        std::unordered_map<std::string,std::string> query;    /**< Parâmetros de query string (ex: ?id=123) */
    };
}