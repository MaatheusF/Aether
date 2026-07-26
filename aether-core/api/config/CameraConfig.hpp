#pragma once

#include <cstdint>
#include <string>

namespace Aether::Api
{
    /**
     * @brief Configuração de acesso às câmeras IP (HTTP Digest Authentication)
     *
     * Centraliza host, porta, credenciais e path do endpoint de snapshot
     * usado pelas câmeras compatíveis com o padrão cgi-bin (ex: Dahua/Intelbras).
     *
     * @warning Os valores abaixo são o default de desenvolvimento. Em produção,
     * mover host/user/password para variáveis de ambiente ou arquivo de
     * configuração fora do versionamento.
     */
    struct CameraConfig
    {
        std::string host     = "192.168.0.110";                /**< IP/host da câmera */
        uint16_t    port     = 80;                              /**< Porta HTTP da câmera */
        std::string user     = "admin";                         /**< Usuário do Digest Auth */
        std::string password = "favero10";                      /**< Senha do Digest Auth */
        std::string cgiPath  = "/cgi-bin/snapshot.cgi?channel="; /**< Path base do snapshot (concatenar canal) */
    };
}
