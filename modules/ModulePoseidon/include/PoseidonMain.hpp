#pragma once
#include <atomic>
#include <iostream>
#include <ostream>
#include <memory>

#include "../../../core/eventbus/include/IModule.hpp"
#include "../../../protocols/aether/common/IProtocolHandler.hpp"
#include "PoseidonService.hpp"

class ModulePoseidon : public IModule, public IProtocolHandler
{
public:
    /**
     * @brief Construtor da classe
     */
    ModulePoseidon();

    /**
     *  @brief Define o nome do modulo
     */
    std::string name() const override
    {
        return "ModulePoseidon";
    }

    /**
     * @brief Retorna o identificador do modulo
     * @return
     */
    uint8_t moduleId() const override;

    /**
     * @brief Função que inicia o modulo quando chamado
     */
    void start() override;

    /**
     * @brief Callback chamado quando um modulo é adicionado no EventBus
     * @param event
     */
    void onEvent(const Event& event) override;

    /**
     * @brief Função chamada quando o modulo é parado
     */
    void stop();

    /**
     * @brief Retorna o status atual do modulo
     */
    bool isRunning() override
    {
        return running;
    }

    /**
     * @brief Função que recebe um callback quando uma conexão (TCP por padrão) recebe um pacote identificado via NetWork
     * @param packet Dados do pacote recebido no formato packet
     * @param channel Protocolo
     */
    void onPacket(const ProtocolAether::Packet& packet, std::shared_ptr<IResponseChannel> channel) override;

private:
    std::atomic<bool> running;  /// Define o status do modulo (Rodando ou parado)
    PoseidonService service;    /// Define a classe de servico do modulo
};