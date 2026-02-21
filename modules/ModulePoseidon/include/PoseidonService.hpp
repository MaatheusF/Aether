#pragma once

#include "../../../core/eventbus/include/Event.hpp"
#include "../../../protocols/aether/include/Packet.hpp"
#include "../../../protocols/aether/common/IProtocolHandler.hpp"
#include <../../../include/external/json.hpp>

/**
 * @class PoseidonService
 * @brief Gerencia eventos e pacotes recebidos do Core via Callback
 */
class PoseidonService
{
public:
    /**
     * @brief Função que recebe um evento do Aether e processa a logica e regras de negocio
     * @param event dados do evento recebido
     */
    static void handleEvent(const Event& event);
    /**
     * @brief Função que recebe um packet payload e realiza o processamento dos dados do tipó DATA_PUSH
     * @param packet Pacote de dados recebido via TCP
     * @JSON: Json a ser recebido:
     * @JSON: Json a ser recebido:
     * {
     *   "device_name": "ESP32",
     *   "event":
     *     {
     *       "type": "sensor",
     *       "sensor_type": "temperature",
     *       "sensor_external_id": "SensorTemp01",
     *       "value": 1.5,
     *       "read_timestamp": 1700000000
     *     }
     * }
     *
     * {
     *   "device_external_id": "1",
     *   "event":
     *     {
     *       "type": "relay",
     *       "relay_external_id": "relay01",
     *       "state": "true",
     *       "read_timestamp": 1700000000
     *     }
     * }
     * @return retorna um std::pair com um valor bool indicando se foi processado com sucesso e em caso de erro retorna a mensagem de erro, caso contrário retorna uma mensagem de sucesso.
     */
    static std::pair<bool,std::string> processJsonPacketDataPush(const ProtocolAether::Packet& packet);

    /**
     * @brief Função que recebe um JSON contendo dados do sensor, processa e persiste no banco de dados do Poseidon
     * @param json JSON contendo os dados do sensor a ser processado
     * @param event dados do evento recebido
     */
    static std::pair<bool, std::string> processSensorData(auto& json);
    /**
     * @brief Função que recebe um JSON contendo dados da relé, processa e persiste no banco de dados do Poseidon
    * @param json JSON contendo os dados do relay a ser processado
     */
    static std::pair<bool, std::string> processRelayData(auto& json);
    /**
     * @brief Função que recebe um pacote TCP do Aether e processa a logica e regras de negocio
     * @param packet dados do pacote recebido
     */
    static void handlePacket(const ProtocolAether::Packet& packet, const std::shared_ptr<IResponseChannel>& channel);
    /**
     * @brief Função que permite enviar uma dado TCP para um cliente conectado, buscando pelo ID externo definido durante
     * o HANDSHAKE
     * @param deviceId String contendo o nome do dispositivo a ser encontrado
     * @param josn Json a ser disparado no Payload
     * @param targetModule modulo, será descontinuado no futuro e ira virar o modulo de origem e não destino
     */
    static void sendReverseToDevice(const std::string& deviceId, const nlohmann::json& jsonPayload, uint16_t targetModule);
};
