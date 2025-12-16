#pragma once
#include <string>

/**
 * @brief Lista de tipos de eventos padrão do Aether
 *
 * Esses tipos são usados para identificar eventos no EventBus.
 */
namespace Events
{
    // --- Core do Aether ---
    inline const std::string CORE_START       = "core.start";   /// Evento de inicialização do Core / Modulos / Threads
    inline const std::string CORE_STOP        = "core.stop";    /// Evento para inrerrupção completa do Core / Modulos e / Threads
    inline const std::string CORE_STATUS      = "core.status";  /// Evento que verifica o status do Daemon

    inline const std::string MODULE_STOPPED   = "MODULE_STOPPED";   /// Evento de Callback interno indicando que o modulo foi parado com sucesso
}
