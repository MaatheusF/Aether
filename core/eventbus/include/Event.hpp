#pragma once
#include <string>
#include <any>
#include <utility>

/**
 * @brief Estrutura base para eventos no Aether EventBus.
 *
 * Cada evento possui um tipo (identificador) e dados associados.
 * Os dados podem ser de qualquer tipo, utilizando std::any.
 */
struct Event
{
    std::string source;  /// Nome do modulo ou do processo que publicou o evento
    std::string target;  /// Nome do modulo ou do processo de destino do evento, caso tenha
    std::string type;    /// Tipo do evento (ex: "CLI_COMMAND", "DAEMON_STATUS")
    std::string data;    /// Dados associados ao evento. Pode ser qualquer tipo.

    /**
     * @brief Construtor do evento
     * @param source Nome do modulo de origem do evento
     * @param target Nome do modulo de destino do evento (null para geral)
     * @param type Nome/identificador do evento
     * @param data Dados do evento, armazenados como std::any
     */
    Event(std::string source, std::string target, std::string type, std::string data ) : source(std::move(source)), target(std::move(target)), type(std::move(type)), data(std::move(data)) {}
};
