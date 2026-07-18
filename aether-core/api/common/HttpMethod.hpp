#pragma once

namespace Aether::Api
{
    /**
     * @brief Métodos HTTP suportados
     *
     * Define os métodos HTTP que podem ser processados pela API.
     * DELETE_ usa underscore para evitar conflito com palavra reservada do C++ DELETE, não deve ser alterado no futuro!
     */
    enum class HttpMethod
    {
        GET,        /**< Requisição GET */
        POST,       /**< Requisição POST */
        PUT,        /**< Requisição PUT */
        PATCH,      /**< Requisição PATCH */
        DELETE_,    /**< Requisição DELETE */
        UNKNOWN     /**< Modelo de requisição desconhecido ou não suportado */
    };
}