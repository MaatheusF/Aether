#pragma once

#include <string>

namespace Aether::Api::Dto
{
    /**
     * @brief DTO de resposta de status
     *
     * Data Transfer Object que carrega informações de status
     * entre a camada Service e Controller.
     *
     * @see StatusService
     * @see StatusController
     */
    struct StatusResponse
    {
        bool status;            /**< Status da API (true = funcional, false = falha)*/
        std::string message;    /**< Mensagem de status */
    };
}