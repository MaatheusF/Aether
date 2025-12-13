#pragma once
#include <memory>


namespace ProtocolAether {
    struct Packet;
}

class IResponseChannel;

/**
 * @brief Interface para manipuladores de protocolo Aether.
 *
 * Esta interface define o contrato para manipuladores de protocolo que processam
 * pacotes recebidos e respondem através de um canal de resposta.
 */
class IProtocolHandler {
public:
    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~IProtocolHandler() = default;

    /**
     * @brief Manipula um pacote recebido.
     *
     * @param packet O pacote recebido para processamento.
     * @param channel O canal de resposta para enviar respostas, se necessário.
     */
    virtual void onPacket(const ProtocolAether::Packet& packet, std::shared_ptr<IResponseChannel> channel) = 0;
};
