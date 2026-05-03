#pragma once
#include "../../../core/eventbus/include/IModule.hpp"
#include "../../../core/eventbus/include/EventBus.hpp"
#include "../../../core/eventbus/include/EventTypes.hpp"
#include "../../../protocols/aether/common/ModuleId.hpp"


#include "../storage/DatabaseConfig.hpp"
#include "../../../core/database/include/ConnectionPool.hpp"

#include "../../../protocols/aether/common/IProtocolHandler.hpp"
#include "../../../protocols/aether/common/IResponseChannel.hpp"
#include "../../../protocols/aether/include/Packet.hpp"
#include "../../../protocols/aether/include/PacketBuilder.hpp"
#include "../../../protocols/aether/include/CommandType.hpp"


#include <iostream>
#include <atomic>
#include <thread>

namespace ProtocolAether
{
    struct Packet;
}

class IResponseChannel;

class ModuleTest : public IModule, public IProtocolHandler
{
public:
    ModuleTest() : running(false) {}

    uint8_t moduleId() const override
    {
        return static_cast<uint8_t>(ModuleId::MODULETEST);
    }

    void onEvent(const Event& event) override {

        if (event.type == Events::CORE_STOP) {
            stop();
            // Executa stop em outra thread para não travar o EventBus
            //std::thread([this]() { stop(); }).detach();
        } else {
            std::cout << "[ModuleTest] Evento recebido: " << event.type
                      << " de " << event.source
                      << " com tipo: " << event.type << std::endl;
        }
    }

    std::string name() const override { return "moduletest"; }

    void start()
    {
        running = true;

        std::cout << "[ModuleTest] Módulo inicializado." << std::endl;
        EventBus::getInstance().subscribe(this); // Inscreve-se para receber eventos
        std::cout << "[ModuleTest] Módulo inscrito para receber eventos." << std::endl;

        ConnectionPool* pool = new ConnectionPool(ModuleTestConfig::DatabaseConfig::connectionString(), 5);

        std::cout << "[ModuleTest] Connection string: " << ModuleTestConfig::DatabaseConfig::connectionString() << std::endl;

        worker = std::thread([this, pool]()
        {
            int test = 0;

            while (running)
            {
                //std::cout << "[ModuleTest] Running..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));

                if (test == 0)
                {
                    test = 1;
                    auto conn = pool->acquire();

                    if (!conn) {
                        std::cout << "[ModuleTest] ERRO: acquire() retornou NULL!" << std::endl;
                        continue;
                    }

                    std::cout << "[ModuleTest] " << PQgetvalue(conn->query("select count(*) from pg_roles"), 0, 0) << std::endl;
                }
            }
        });

    }

    bool isRunning() override
    {
        return running;
    }

    void stop() {
        if (!running) return;
        running = false;

        std::cout << "[ModuleTest] Parando módulo....." << std::endl;

        // Simula limpeza de recursos
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));

        if (worker.joinable()) {
            worker.join();
            std::cout << "[ModuleTest] Módulo parado." << std::endl;
        }

        // Desinscreve do EventBus antes de publicar
        EventBus::getInstance().unsubscribe(this);

        // Notifica EventBus que o módulo parou
        EventBus::getInstance().publish(Event(name(), "", Events::MODULE_STOPPED, ""));
    }

    void onPacket(
        const ProtocolAether::Packet& packet,
        std::shared_ptr<IResponseChannel> channel
    ) override
    {
        std::cout << "[ModuleTest] onPacket DISPARADO" << std::endl;
        std::cout << "[ModuleTest][TCP] Packet recebido\n";
        std::cout << "  Cmd: " << static_cast<int>(packet.type) << "\n";
        std::cout << "  Len: " << packet.payload.size() << "\n";

        if (!packet.payload.empty()) {
            std::cout << "  Payload: ";
            for (auto b : packet.payload)
                std::cout << std::hex << (int)b << " ";
            std::cout << std::dec << "\n";
        }

        // 🔁 resposta simples (ACK)
        //ProtocolAether::Packet response;
        //response.type = 0xFF;              // ACK
        //response.payload = { 0xAA, 0x55 };     // arbitrário

        auto response = ProtocolAether::PacketBuilder::build(
            CommandType::ACK, // ou static_cast<CommandType>(0xFF)
            /* module */ packet.module,
            /* payload */ {  }
        );

        channel->sendResponse(response);

        std::cout << "[ModuleTest][TCP] ACK enviado\n";
    }

private:
    std::thread worker;
    std::atomic<bool> running;
};
