#pragma once

#include <string>
#include <unordered_map>

namespace Aether::Api
{
    /**
     * @brief Estrutura que representa uma resposta HTTP
     *
     * Contém os dados da resposta que será enviada ao cliente.
     * Pode ser convertida para um objeto Boost.Beast pela classe
     * HttpSession para ser transmitida via socket TCP.
     *
     * @see HttpSession::createResponse()
     * @see Router::dispatch()
     */
    struct HttpResponse
    {
        int status = 200;                                     /**< Status HTTP (200, 404, 500, etc) */
        std::string body;                                     /**< Corpo da resposta (JSON, HTML, etc) */
        std::unordered_map<std::string,std::string> headers;  /**< Headers HTTP da resposta */
    };
}