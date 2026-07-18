#pragma once

#include "../services/StatusService.hpp"
#include "../common/HttpResponse.hpp"
#include "../common/HttpRequest.hpp"

namespace Aether::Api
{
    /**
     * @brief Controller de Status da API
     *
     * Responsável por processar requisições relacionadas ao status
     * e saúde da API.
     *
     * @see StatusService
     * @see Router
     */
    class StatusController
    {
        public:
            /**
             * @brief Processa requisição GET de status
             * @param request Requisição HTTP recebida
             * @return Resposta HTTP com status da API
             */
            HttpResponse get(const HttpRequest& request);

        private:
            StatusService m_service;  /**< Service de status */
    };
}
