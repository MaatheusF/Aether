#pragma once

#include "../dto/StatusResponse.hpp"

namespace Aether::Api
{
    /**
     * @brief Service do controler StatusController
     *
     * Contém a lógica de negócio para obter via GET o status
     * e informações de saúde da API e do Aether Core
     *
     * @see StatusController
     */
    class StatusService
    {
    public:
        /**
         * @brief Retorna o status atual da API (Aether_Core)
         * @return DTO contendo status e mensagem
         */
        static Dto::StatusResponse get();
    };
}