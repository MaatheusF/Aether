#include "SessionManager.hpp"
#include <iostream>

/**
 * @file SessionManager.cpp
 * @brief Implementação do SessionManager.
 *
 * A implementação é pequena e intencionalmente explícita para facilitar
 * entendimento e manutenção. Ela usa o id() do IResponseChannel como chave
 * para o mapa interno (uint16_t). Uma alternativa seria usar endereço do
 * ponteiro (pointer value) ou um UID gerado ao criar a conexão; a escolha
 * por channel->id() é baseada na suposição de que cada implementação de
 * IResponseChannel fornece um identificador único por conexão.
 *
 * Thread-safety:
 * - Todas as operações que acessam `map_` protegem a região crítica com
 *   um std::mutex (mutex_). Isso garante que leituras e escritas concorrentes
 *   sejam seguras.
 * - getDeviceExternalId realiza lock de curta duração e retorna um copy do
 *   string armazenado (através do optional), evitando referências a dados
 *   internos que poderiam ser invalidos após unlock.
 *
 * Edge-cases / Considerações:
 * - registerChannel com um channel nulo é uma operação no-op.
 * - Se registerChannel for chamado múltiplas vezes com a mesma chave, o
 *   último deviceExternalId sobrescreverá o anterior. Isso permite re-identificação
 *   da mesma conexão sem complicação adicional.
 * - unregisterChannel é silenciosa se a entrada não existir.
 * - Não há timeout automático para entradas esquecidas; é responsabilidade
 *   do code path de fechamento de conexão chamar unregisterChannel. Se desejar,
 *   podemos adicionar um mecanismo de TTL/garbage-collection futuro.
 */

SessionManager& SessionManager::instance()
{
    static SessionManager inst;
    return inst;
}

void SessionManager::registerChannel(const std::shared_ptr<IResponseChannel>& channel, const std::string& deviceExternalId)
{
    if (!channel) return;
    uint16_t key = channel->id();
    Entry e;
    e.channel = channel;
    e.deviceExternalId = deviceExternalId;
    {
        std::lock_guard<std::mutex> lk(mutex_);
        map_[key] = std::move(e);
    }
    std::cout << "[SessionManager] registered channel id=" << key << " device=" << deviceExternalId << std::endl;
}

void SessionManager::unregisterChannel(const std::shared_ptr<IResponseChannel>& channel)
{
    if (!channel) return;
    uint16_t key = channel->id();
    {
        std::lock_guard<std::mutex> lk(mutex_);
        map_.erase(key);
    }
    std::cout << "[SessionManager] unregistered channel id=" << key << std::endl;
}

std::optional<std::string> SessionManager::getDeviceExternalId(const std::shared_ptr<IResponseChannel>& channel) const
{
    if (!channel) return {};
    uint16_t key = channel->id();
    std::lock_guard<std::mutex> lk(mutex_);
    auto it = map_.find(key);
    if (it == map_.end()) return {};
    // Retorna cópia
    return it->second.deviceExternalId;
}

std::shared_ptr<IResponseChannel> SessionManager::getChannelByDeviceExternalId(const std::string& deviceExternalId)
{
    std::lock_guard<std::mutex> lk(mutex_);
    std::shared_ptr<IResponseChannel> result;
    // Procuramos o primeiro canal com o deviceExternalId desejado.
    // Durante a iteração, limpamos entradas cujo weak_ptr expirou.
    for (auto it = map_.begin(); it != map_.end(); )
    {
        auto &entry = it->second;
        auto ch = entry.channel.lock();
        if (!ch)
        {
            // Canal expirou; remove a entrada para manter o mapa limpo
            uint16_t key = it->first;
            ++it; // avança antes de apagar
            auto erased = map_.erase(key);
            (void)erased;
            continue;
        }

        if (entry.deviceExternalId == deviceExternalId)
        {
            result = ch;
            break;
        }
        ++it;
    }
    return result; // nullptr se não encontrado
}
