#pragma once
#include "../eventbus/include/IModule.hpp"
#include "../eventbus/include/EventBus.hpp"
#include "../eventbus/include/EventTypes.hpp"
#include <iostream>
#include <atomic>
#include <thread>

class ModuleTest : public IModule {
public:
    ModuleTest() : running(true) {}

    void onEvent(const Event& event) override {
        if (event.type == Events::CORE_STOP) {
            stop();
            // Executa stop em outra thread para não travar o EventBus
            //std::thread([this]() { stop(); }).detach();
        } else {
            std::cout << "[ModuleTest] Evento recebido: " << event.type
                      << " de " << event.source
                      << " com dados: " << event.data << std::endl;
        }
    }

    std::string name() const override { return "moduletest"; }

    void initialize() {
        EventBus::getInstance().subscribe(this);
    }

    void stop() {
        if (!running) return;
        running = false;
        std::cout << "[ModuleTest] Parando módulo..." << std::endl;
        // Simula limpeza de recursos
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "[ModuleTest] Módulo parado." << std::endl;

        // Notifica EventBus que o módulo parou
        EventBus::getInstance().publish(Event(name(), "", Events::MODULE_STOPPED, ""));
    }

private:
    std::atomic<bool> running;
};
