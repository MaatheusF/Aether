#pragma once

#include <string>
#include "Event.hpp"
#include <cstdint>
/**
 * @brief Classe que determina a estrutura padrão dos modulos acoplaveis ao Aehter
 */
class IModule
{
    public:
    virtual ~IModule() = default;                     /// Destruidor do objeto
    virtual void onEvent(const Event& event) = 0;     /// Recebe um evento publicado no EventBus
    virtual std::string name() const = 0;             /// Retorna o nome do módulo
    virtual void start() {}                           /// Inicia o módulo
    virtual bool isRunning() { return false; }        /// Retorna se o módulo está em execução
};