#pragma once

#include "../config/CameraConfig.hpp"
#include "../dto/CameraSnapshotResponse.hpp"

#include <string>

namespace Aether::Api
{
    /**
     * @brief Service do CameraController
     *
     * Contém a lógica de negócio para capturar o snapshot (JPEG) de uma
     * câmera IP autenticada via HTTP Digest Authentication (RFC 2617).
     *
     * Fluxo:
     * 1. Envia GET sem Authorization para obter o desafio (401 + WWW-Authenticate)
     * 2. Calcula HA1/HA2/response (MD5) a partir do desafio e das credenciais
     * 3. Reenvia GET com o header Authorization: Digest ...
     * 4. Retorna os bytes crus da imagem (JPEG)
     *
     * @see CameraController
     * @see CameraConfig
     */
    class CameraService
    {
        public:
            /**
             * @brief Construtor
             * @param config Configuração de acesso à câmera (host, credenciais, path)
             */
            explicit CameraService(CameraConfig config = CameraConfig{});

            /**
             * @brief Captura o snapshot atual de um canal da câmera
             * @param channel Número do canal (ex: 1)
             * @return DTO com os bytes da imagem ou mensagem de erro
             */
            Dto::CameraSnapshotResponse getSnapshot(int channel);

        private:
            CameraConfig m_config;  /**< Configuração de acesso à câmera */

            /**
             * @brief Monta o header "Authorization: Digest ..." a partir do
             * desafio recebido em WWW-Authenticate
             * @param method Método HTTP da requisição (ex: "GET")
             * @param uri URI requisitada (ex: /cgi-bin/snapshot.cgi?channel=1)
             * @param wwwAuthenticateHeader Conteúdo do header WWW-Authenticate retornado pela câmera
             * @return Valor completo do header Authorization
             */
            std::string buildDigestAuthorizationHeader(
                const std::string& method,
                const std::string& uri,
                const std::string& wwwAuthenticateHeader) const;

            /**
             * @brief Extrai um parâmetro (realm, nonce, qop, opaque, ...) do
             * header WWW-Authenticate
             * @param header Conteúdo do header WWW-Authenticate
             * @param key Nome do parâmetro a extrair
             * @return Valor do parâmetro, ou string vazia se não encontrado
             */
            static std::string extractAuthParam(const std::string& header, const std::string& key);
    };
}
