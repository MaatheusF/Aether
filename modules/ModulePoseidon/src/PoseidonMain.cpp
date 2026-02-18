#include "../include/PoseidonMain.hpp"

#include "../../../core/eventbus/include/IModule.hpp"
#include "../../../protocols/aether/common/IProtocolHandler.hpp"
#include "../../../protocols/aether/common/ModuleId.hpp"
#include "../../../core/eventbus/include/EventTypes.hpp"
#include "../../../core/eventbus/include/EventBus.hpp"
#include "PoseidonService.hpp"

#include "../../../protocols/aether/include/CommandType.hpp"
#include "../../../protocols/aether/include/PacketBuilder.hpp"

/**
 * @brief Construtor da Classe
 */
ModulePoseidon::ModulePoseidon() : running(false) {}

/**
 * @brief Função chamada quando o modulo é parado
 */
void ModulePoseidon::stop()
{
    if (!running) return;
    running = false;
    std::cout << "[Poseidon] Parando módulo....." << std::endl;

    // Desinscreve do EventBus antes de publicar
    EventBus::getInstance().unsubscribe(this);

    // Notifica EventBus que o módulo parou
    EventBus::getInstance().publish(Event(name(), "", Events::MODULE_STOPPED, ""));
}

/**
 * @brief Função chamada quando o modulo é iniciado
 */
void ModulePoseidon::start()
{
    running = true;
    std::cout << "[Poseidon] Módulo inicializado." << std::endl;
    EventBus::getInstance().subscribe(this); // Inscreve-se para receber eventos do MainBus
}

/**
 * @brief Retorna o identificador do modulo
 * @return
 */
uint8_t ModulePoseidon::moduleId() const
{
    return static_cast<uint8_t>(ModuleId::MODULE_POSEIDON);
}

/**
 * @brief Callback chamado quando um modulo é adicionado no EventBus,
 * chama a classe de serviços para processar os dados
 */
void ModulePoseidon::onEvent(const Event& event) {

    /// Verifica se recebeu um evento do tipo CORE_STOP
    if (event.type == Events::CORE_STOP) {
        stop();
        return;
    }

    /// Verifica se recebeu um evento do tipo CORE_START
    if (event.type == Events::CORE_START)
    {
        ModulePoseidon::start();
        return;
    }

    if (!isRunning()) return;

    /// Chama a classe de serviços para processar os dados recebidos
    PoseidonService::handleEvent(event);
}

/**
 * @brief Callback chamado quando um pacote TCP identificado é recebido via callback
 * chama a classe de serviços para processar os dados
 */
void ModulePoseidon::onPacket(const ProtocolAether::Packet& packet,std::shared_ptr<IResponseChannel> channel)
{
    /// Chama a classe de serviços para processar os dados recebidos
    PoseidonService::handlePacket(packet, channel);
}