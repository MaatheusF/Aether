#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>

#include "../../protocols/aether/common/IResponseChannel.hpp"

/**
 * @file SessionManager.hpp
 * @brief Gerencia mapeamentos entre canais de resposta e deviceExternalId.
 *
 * SessionManager provê uma forma simples e thread-safe de associar o
 * identificador lógico do dispositivo (deviceExternalId) a um canal de
 * resposta (IResponseChannel). Isso permite que módulos que só recebem
 * um std::shared_ptr<IResponseChannel> consultem metadados da sessão
 * (por exemplo, para saber qual dispositivo enviou um pacote).
 *
 * Design e regras principais:
 * - É um singleton de processo (SessionManager::instance()).
 * - Usa `IResponseChannel::id()` (uint16_t) como chave no mapa interno.
 *   Isso pressupõe que cada canal tem um id único durante sua vida útil.
 * - Protegido por mutex para leituras/escritas concorrentes.
 * - Chamadas típicas:
 *     - `registerChannel(channel, deviceId)` — chamada quando o handshake
 *       é concluído e o deviceExternalId é conhecido.
 *     - `unregisterChannel(channel)` — chamada quando a conexão/sessão
 *       é encerrada para evitar vazamento do mapa.
 *     - `getDeviceExternalId(channel)` — consulta (retorna std::optional).
 *     - `getChannelByDeviceExternalId(deviceId)` — retorna o canal associado
 *       caso exista e ainda esteja vivo (útil para threads que iniciam envio reverso).
 *
 * Observações adicionais sobre robustez:
 * - Se `IResponseChannel::id()` não for único, considere trocar a chave
 *   para outro identificador (por exemplo endereço do shared_ptr ou um
 *   UID gerado ao criar a conexão).
 * - Para evitar race conditions na remoção, sempre chamar `unregisterChannel`
 *   do mesmo contexto onde a sessão/conexão é finalizada.
 */
class SessionManager
{
public:
    /**
     * @brief Obtém a instância singleton do SessionManager.
     * Uso: SessionManager::instance().getDeviceExternalId(channel);
     * @return SessionManager& referência para o singleton.
     */
    static SessionManager& instance();

    /**
     * @brief Registra um canal associado a um deviceExternalId.
     *
     * Normalmente chamado pelo código de handshake (ex.: ConnSession) quando
     * a identificação do dispositivo é concluída com sucesso. A função grava
     * o par (channel->id() -> deviceExternalId) no mapa interno.
     *
     * Se já existir um valor para a mesma chave, o deviceExternalId será
     * sobrescrito (comportamento intencional para refletir re-identificação).
     *
     * @param channel Canal associado (não nulo).
     * @param deviceExternalId Identificador externo do dispositivo.
     */
    void registerChannel(const std::shared_ptr<IResponseChannel>& channel, const std::string& deviceExternalId);

    /**
     * @brief Remove o registro do canal.
     *
     * Deve ser chamado quando a conexão/sessão é encerrada. Se o canal não
     * estiver registrado, a operação é silenciosa.
     *
     * @param channel Canal a remover do mapa.
     */
    void unregisterChannel(const std::shared_ptr<IResponseChannel>& channel);

    /**
     * @brief Consulta o deviceExternalId associado a um canal.
     *
     * Retorna std::nullopt quando não existe um mapeamento para o canal
     * (p.ex. handshake não concluído ou registro removido).
     *
     * @param channel Canal a consultar.
     * @return std::optional<std::string> deviceExternalId quando presente.
     */
    std::optional<std::string> getDeviceExternalId(const std::shared_ptr<IResponseChannel>& channel) const;

    /**
     * @brief Procura e retorna o canal associado a um deviceExternalId.
     *
     * Retorna nullptr quando não existe mapeamento ou quando o canal já expirou.
     * Função útil para threads que precisam enviar pacotes "reversos" a um
     * dispositivo identificado por seu deviceExternalId.
     *
     * Nota: esta função pode limpar entradas expiradas no mapa interno, por
     * isso não é const.
     */
    std::shared_ptr<IResponseChannel> getChannelByDeviceExternalId(const std::string& deviceExternalId);

private:
    SessionManager() = default;
    ~SessionManager() = default;
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    mutable std::mutex mutex_; /**< Protege acesso ao mapa_ para thread-safety */

    struct Entry {
        std::weak_ptr<IResponseChannel> channel; /**< weak ref para evitar ciclos de ownership */
        std::string deviceExternalId;            /**< id do dispositivo associado */
    };

    // Usa o id() do IResponseChannel como chave (uint16_t).
    std::unordered_map<uint16_t, Entry> map_;
};
