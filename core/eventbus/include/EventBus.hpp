#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <thread>
#include <unordered_map>

#include "IModule.hpp"
#include "Event.hpp"


/**
 * @brief Classe responsavel por gerenciar assinaturas e disparo de eventos para módulos.
 * Garante que todos os modulos compartilhem a mesma instancia
 */
class EventBus
{
public:
    /**
     * @brief Função que retorna a instancia do EventBus
     * @return retorna um ponteiro para a instancia
     */
    static EventBus& getInstance()
    {
        static EventBus instance;
        return instance;
    }

    /**
     * @brief Inscreve o modulo para receber os comandos registrados
     * @param module ponteiro do tipo IModule
     */
    void subscribe(IModule* module)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers.push_back(module);
    }

    /**
     * @brief Remove módulo de todos os tipos de eventos
     * @param module ponteiro do tipo IModule
     */
    void unsubscribe(IModule* module)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), module), subscribers.end());
    }

    /**
     * @brief Publica um evento na lista de eventos
     * @param event estrutura do evento a ser publicado
     */
    void publish(const Event& event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* sub : subscribers)
        {
            if (event.target.empty() || sub->name() == event.target)
            {
                //sub->onEvent(event);
                // Processa os callbacks de forma assincrona sem travar o EventBus
                std::thread([sub, event]() { sub->onEvent(event); }).detach();
            }
        }
    }

private:
    EventBus() = default;                   /// Construtor da classe
    std::vector<IModule*> subscribers;      /// Lista de modulos inscritos
    std::mutex mutex_;                      /// Mutex para Thread-safe

};