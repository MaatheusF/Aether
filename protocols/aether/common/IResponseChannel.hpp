#pragma once
#include "../include/Packet.hpp"

/**
 * @brief Interface para um canal de resposta no protocolo Aether.
 *
 * Esta interface define os métodos necessários para enviar pacotes de resposta
 * através de um canal específico. Cada canal de resposta deve implementar esta
 * interface para garantir a capacidade de enviar pacotes e fornecer um identificador único.
 */
class IResponseChannel
{
public:
    virtual ~IResponseChannel() = default;                                      /// Destrutor virtual padrão
    virtual void sendResponse(const ProtocolAether::Packet& pkt) = 0;           /// Envia um pacote de resposta através do canal
    virtual uint16_t id() const = 0;                                            /// Identificador único do canal de resposta
};
