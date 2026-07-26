#pragma once

#include <cstdint>
#include <string>

namespace Aether::Core::Utils
{
    /**
     * @brief Utilitário de hash MD5
     *
     * Implementação própria e independente (sem dependência de OpenSSL/libcurl)
     * usada para o cálculo de HA1/HA2/response do HTTP Digest Authentication
     * (RFC 2617), necessário para autenticar nas câmeras IP.
     *
     * @see CameraService
     */
    class Md5
    {
        public:
            /**
             * @brief Calcula o hash MD5 de uma string
             * @param input Texto de entrada
             * @return Hash MD5 em hexadecimal minúsculo (32 caracteres)
             */
            static std::string hash(const std::string& input);

        private:
            static void transform(std::uint32_t state[4], const std::uint8_t block[64]);
    };
}
