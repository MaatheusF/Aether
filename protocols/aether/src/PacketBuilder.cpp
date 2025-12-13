#include "../include/PacketBuilder.hpp"

namespace ProtocolAether
{
    /**
     * Constrói um pacote sem payload.
     * @param cmd  Codigo do comando.
     * @param module Codigo do módulo.
     * @return O pacote construído.
     */
    Packet PacketBuilder::build(CommandType cmd, uint16_t module)
    {
        Packet pkt{};
        pkt.magic   = MAGIC;
        pkt.version = VERSION;
        pkt.type    = static_cast<uint16_t>(cmd);
        pkt.module  = module;
        pkt.length  = 0;
        pkt.payload.clear();
        return pkt;
    }

    /**
     *  Constrói um pacote com payload.
     * @param cmd  Codigo do comando.
     * @param module Codigo do módulo.
     * @param payload O payload do pacote.
     * @return O pacote construído.
     */
    Packet PacketBuilder::build(
        CommandType cmd,
        uint16_t module,
        const std::vector<uint8_t>& payload
    )
    {
        Packet pkt{};
        pkt.magic   = MAGIC;
        pkt.version = VERSION;
        pkt.type    = static_cast<uint16_t>(cmd);
        pkt.module  = module;
        pkt.length  = static_cast<uint32_t>(payload.size());
        pkt.payload = payload;
        return pkt;
    }

    /**
     * Encode um pacote em um buffer de bytes pronto para envio na rede.
     * @param pkt O pacote a ser codificado.
     * @return Um vetor de bytes contendo o pacote codificado.
     */
    std::vector<uint8_t> PacketBuilder::encode(const Packet& pkt)
    {
        /// Reserva espaço no buffer
        std::vector<uint8_t> buffer;
        buffer.reserve(11 + pkt.payload.size());

        /// Funções auxiliares para empurrar valores no buffer
        auto push16 = [&](uint16_t v) {
            buffer.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
            buffer.push_back(static_cast<uint8_t>(v & 0xFF));
        };

        /// Função auxiliar para empurrar um valor de 32 bits no buffer
        auto push32 = [&](uint32_t v) {
            buffer.push_back((v >> 24) & 0xFF);
            buffer.push_back((v >> 16) & 0xFF);
            buffer.push_back((v >> 8) & 0xFF);
            buffer.push_back(v & 0xFF);
        };

        push16(pkt.magic);              /// Magic
        buffer.push_back(pkt.version);  /// Version
        push16(pkt.type);               /// Type
        push16(pkt.module);             /// Module
        push32(pkt.length);             /// Length

        // Payload
        buffer.insert(buffer.end(), pkt.payload.begin(), pkt.payload.end());

        return buffer;
    }
}
