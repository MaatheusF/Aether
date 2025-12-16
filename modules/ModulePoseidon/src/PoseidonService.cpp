#include "../include/PoseidonService.hpp"

#include "../include/PoseidonMain.hpp"
#include "../../../protocols/aether/include/CommandType.hpp"
#include "../../../protocols/aether/include/PacketBuilder.hpp"
#include "../../../protocols/aether/common/IResponseChannel.hpp"

/**
 * @brief Callback chamado quando um modulo é adicionado no EventBus,
 * chama a classe de serviços para processar os dados
 */
void PoseidonService::handleEvent(const Event& event)
{

}

/**
 * @brief Callback chamado quando um pacote TCP identificado é recebido via callback
 * chama a classe de serviços para processar os dados
 */
void PoseidonService::handlePacket(const ProtocolAether::Packet& packet, std::shared_ptr<IResponseChannel> channel)
{
    auto response = ProtocolAether::PacketBuilder::build(
        CommandType::ACK, // ou static_cast<CommandType>(0xFF)
        /* module */ packet.module,
        /* payload */ {  }
    );

    channel->sendResponse(response);
}