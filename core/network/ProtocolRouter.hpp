#pragma once
#include <iostream>
#include <unordered_map>

#include "../../protocols/aether/common/IProtocolHandler.hpp"
#include "include/Packet.hpp"

/**
 * Classe que implementa um Handler para gerenciar os pacotes recebidos via TCP e
 * distribuir entre os modulos corretamente, garantindo que cada module receba apenas
 * os pacotes destinados a dele.
 */
class ProtocolRouter : public IProtocolHandler
{
public:
    /**
     * @brief Registra um módulo capaz de receber pacotes TCP. Cada módulo implementa IProtocolHandler
     * @param handler
     * @param module
     */
    void registerModule(const std::shared_ptr<IProtocolHandler>& handler, const std::shared_ptr<IModule>& module)
    {

        std::cout << "[Router] Registrando módulo com ID="
          << static_cast<int>(handler->moduleId())
          << " (" << typeid(*module).name() << ")"
          << std::endl;

        modules.emplace_back(RegisteredModule{handler, module});;
    }

    /**
     * @brief Retorna o identificador (Id definido em /protocol/common/ModuleId.hpp) do modulo registrado
     * Nesse caso não é usado pra nada
     */
    uint8_t moduleId() const override
    {
        return 0xFF; // Router não é um módulo real, apenas retorna um codigo padrão
    }

    /**
     * @brief Chamado pelo Parser sempre que um pacote é considerado valido, decide quem deve receber este pacote
     * @param packet Pacote recebido
     * @param channel Canal de comunicação
     */
    void onPacket( const ProtocolAether::Packet& packet, std::shared_ptr<IResponseChannel> channel ) override
    {
        /// Busca o modulo correto para enviar o pacote
        auto it = std::find_if(
            modules.begin(),
            modules.end(),
            [&](const auto& m)
            {
                return m.handler->moduleId() == packet.module;
            }
        );

        /// Verifica se o modulo está disponível
        if (!it->module->isRunning())
        {
            std::string payload = "Modulo indisponível";
            auto response = ProtocolAether::PacketBuilder::build(
                /* CommandType  */CommandType::MODULE_UNAVAILABLE,
                /* Module */    static_cast<uint16_t>(ModuleId::CORE),
                /* Payload */   std::vector<uint8_t>(payload.begin(), payload.end())
            );

            channel->sendResponse(response);
            return;
        }

        /// Encaminha o pacote para o modulo correspondente
        if (it != modules.end())
        {
            it->handler->onPacket(packet, channel);
        }
        else
        {
            /// Responde ao cliente um ERROR_GENERIC indicando a falta de Identificação
            std::string payload = "Modulo não encontrado";
            auto response = ProtocolAether::PacketBuilder::build(
                /* CommandType  */CommandType::ERROR_GENERIC,
                /* Module */    static_cast<uint16_t>(ModuleId::CORE),
                /* Payload */   std::vector<uint8_t>(payload.begin(), payload.end())
            );

            channel->sendResponse(response);
            std::cout << "[Router] Nenhum módulo para moduleId=" << static_cast<int>(packet.module) << std::endl;

        }
    }

private:
    /// Definição da estrutura do vector modules
    struct RegisteredModule
    {
        std::shared_ptr<IProtocolHandler> handler;
        std::shared_ptr<IModule> module;
    };
    std::vector<RegisteredModule> modules; /// Lista de modulos registrados

    //std::vector<std::shared_ptr<IProtocolHandler>> modules; /// Lista de modulos registrados
};
