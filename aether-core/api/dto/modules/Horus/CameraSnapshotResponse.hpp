#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Aether::Api::Dto
{
    /**
     * @brief DTO de resposta do snapshot de câmera
     *
     * Carrega o resultado da captura de imagem da câmera (via CameraService)
     * entre a camada Service e o Controller.
     *
     * @see CameraService
     * @see CameraController
     */
    struct CameraSnapshotResponse
    {
        bool success = false;                     /**< true se a imagem foi capturada com sucesso */
        int httpStatus = 0;                        /**< Status HTTP retornado pela câmera */
        std::string message;                       /**< Mensagem de erro (quando success = false) */
        std::string contentType = "image/jpeg";     /**< Content-Type da imagem retornada */
        std::vector<std::uint8_t> data;             /**< Bytes crus da imagem JPEG */
    };
}
