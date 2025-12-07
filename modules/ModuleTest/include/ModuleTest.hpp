#pragma once
#include "../../../core/eventbus/include/IModule.hpp"
#include "../../../core/eventbus/include/EventBus.hpp"
#include "../../../core/eventbus/include/EventTypes.hpp"

#include <iostream>
#include <atomic>
#include <thread>

class ModuleTest : public IModule {
public:
    ModuleTest() : running(false) {}

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

    void start()
    {
        running = true;

        std::cout << "[ModuleTest] Módulo inicializado." << std::endl;
        EventBus::getInstance().subscribe(this); // Inscreve-se para receber eventos
        std::cout << "[ModuleTest] Módulo inscrito para receber eventos." << std::endl;

        worker = std::thread([this]()
        {
            while (running)
            {
                std::cout << "[ModuleTest] Running..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

    }

    /*
    void initialize() {
        running = true;

        worker = std::thread([this]()
        {
            while (running)
            {
                std::cout << "[ModuleTest] Running........" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }aaa
        });

        std::cout << "[ModuleTest] Módulo inicializado." << std::endl;
        EventBus::getInstance().subscribe(this); // Inscreve-se para receber eventos
        std::cout << "[ModuleTest] Módulo inscrito para receber eventos." << std::endl;
    }*/

    bool isRunning() override
    {
        return running;
    }

    void stop() {
        if (!running) return;
        running = false;
        std::cout << "[ModuleTest] Parando módulo..." << std::endl;
        // Simula limpeza de recursos
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (worker.joinable()) {
            worker.join();
            std::cout << "[ModuleTest] Módulo parado." << std::endl;
        }

        // Notifica EventBus que o módulo parou
        EventBus::getInstance().publish(Event(name(), "", Events::MODULE_STOPPED, ""));
    }

private:
    std::thread worker;
    std::atomic<bool> running;
};
