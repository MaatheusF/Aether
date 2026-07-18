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
        std::string status;     /**< Status da API ("UP" = funcional, "Down" = Existe alguma falha)*/
        std::string name;       /**< Nome da API */
        std::string timestamp;  /**< Timestamp da resposta */
        std::string message;    /**< Mensagem de status */
    };
}