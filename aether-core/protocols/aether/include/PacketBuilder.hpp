#pragma once

#include "Packet.hpp"
#include "CommandType.hpp"

#include <vector>
#include <cstdint>


/**
 * @brief Namespace para a construção e serialização de pacotes do Protocolo Aether
 */
namespace ProtocolAether
{
    /**
     * @brief Classe responsável por construir e serializar pacotes do Protocolo Aether
     */
    class PacketBuilder
    {
    public:
        static constexpr uint16_t MAGIC = 0xAA55;       /// Valor mágico do protocolo
        static constexpr uint8_t  VERSION = 1;          /// Versão do protocolo

        /// Cria um pacote vazio (sem payload)
        static Packet build(CommandType cmd, uint16_t module);

        /// Cria um pacote com payload
        static Packet build(
            CommandType cmd,
            uint16_t module,
            const std::vector<uint8_t>& payload
        );

        /// Serializa o Packet em bytes (para enviar via TCP)
        static std::vector<uint8_t> encode(const Packet& pkt);
    };
}
