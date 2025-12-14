#pragma once
#include <iostream>
#include <unordered_map>

#include "IProtocolHandler.hpp"
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
     * @param module
     */
    void registerModule(const std::shared_ptr<IProtocolHandler>& module)
    {

        std::cout << "[Router] Registrando módulo com ID="
          << static_cast<int>(module->moduleId())
          << " (" << typeid(*module).name() << ")"
          << std::endl;

        modules.push_back(module);
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
    void onPacket(
        const ProtocolAether::Packet& packet,
        std::shared_ptr<IResponseChannel> channel
        ) override
    {
        /// Busca o modulo correto para enviar o pacote
        auto it = std::find_if(
            modules.begin(),
            modules.end(),
            [&](const auto& m)
            {
                return m->moduleId() == packet.module;
            }
        );

        /// Encaminha o pacote para o modulo correspondente
        if (it != modules.end())
        {
            (*it)->onPacket(packet, channel);
        }
        else
        {
            std::cout << "[Router] Nenhum módulo para moduleId=" << static_cast<int>(packet.module) << std::endl;
        }
    }

private:
    std::vector<std::shared_ptr<IProtocolHandler>> modules; /// Lista de modulos registrados
};
