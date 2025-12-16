#pragma once

#include "../../../core/eventbus/include/Event.hpp"
#include "../../../protocols/aether/include/Packet.hpp"
#include "../../../protocols/aether/common/IProtocolHandler.hpp"

/**
 * @class PoseidonService
 * @brief Gerencia eventos e pacotes recebidos do Core via Callback
 */
class PoseidonService
{
public:
    /**
     * @brief Função que recebe um evento do Aether e processa a logica e regras de negocio
     * @param event dados do evento recebido
     */
    static void handleEvent(const Event& event);
    /**
     * @brief Função que recebe um pacote TCP do Aether e processa a logica e regras de negocio
     * @param packet dados do pacote recebido
     */
    static void handlePacket(const ProtocolAether::Packet& packet, std::shared_ptr<IResponseChannel> channel);
};
