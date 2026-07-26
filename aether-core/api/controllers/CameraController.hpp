#pragma once

#include "../services/CameraService.hpp"
#include "../common/HttpResponse.hpp"
#include "../common/HttpRequest.hpp"

namespace Aether::Api
{
    /**
     * @brief Controller de Câmeras da API
     *
     * Responsável por processar requisições relacionadas à captura de
     * snapshot (imagem JPEG) das câmeras IP conectadas ao Aether Core.
     *
     * @see CameraService
     * @see Router
     */
    class CameraController
    {
        public:
            /**
             * @brief Processa requisição GET de snapshot de uma câmera
             *
             * Rota: GET /api/cameras/:channel/snapshot
             *
             * @param request Requisição HTTP recebida
             * @return Resposta HTTP com a imagem JPEG (Content-Type: image/jpeg)
             *         ou um erro em JSON quando a captura falha
             */
            HttpResponse getSnapshot(const HttpRequest& request);

        private:
            CameraService m_service;  /**< Service de câmeras */

            /**
             * @brief Extrai o número do canal a partir do path da requisição
             * (ex: "/api/cameras/1/snapshot" -> 1)
             * @param path Path da requisição
             * @return Número do canal, ou -1 se o path for inválido
             */
            static int parseChannelFromPath(const std::string& path);
    };
}
