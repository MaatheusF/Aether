#include "../include/PoseidonService.hpp"

#include "../include/PoseidonMain.hpp"
#include "../../../protocols/aether/include/CommandType.hpp"
#include "../../../protocols/aether/include/PacketBuilder.hpp"
#include "../../../protocols/aether/common/IResponseChannel.hpp"

#include "../config/DatabaseConfig.hpp"
#include "../../../core/database/include/ConnectionPool.hpp"

#include <../../../include/external/json.hpp>

/**
 * @brief Callback chamado quando um modulo é adicionado no EventBus,
 * chama a classe de serviços para processar os dados
 */
void PoseidonService::handleEvent(const Event& event)
{

}

/**
 * @brief Função que recebe um packet payload e realiza o processamento dos dados do tipó DATA_PUSH
 * @param packet
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
 *   "device_name": "1",
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
std::pair<bool,std::string> PoseidonService::processJsonPacketDataPush(const ProtocolAether::Packet& packet)
{
    // Biblioteca para manipulação de JSON
    using json = nlohmann::json;
    json j;

    // Tenta realizar o parse do payload recebido (Espera sempre um JSON)
    try
    {
        j = json::parse(packet.payload);
    }
    catch (const std::exception& e)
    {
        return std::make_pair(false, "JSON invalido");
    }

    // Verifica se a TAG "device_name" existe no Json
    if (!j.contains("device_name") || !j["device_name"].is_string())
    {
        return std::make_pair(false, "Elemento 'device_name' inválido ou ausente");
    }

    // Verifica se a TAG "event" existe no Json
    if (!j.contains("event"))
    {
        return std::make_pair(false, "Objeto 'event' inválido ou ausente");
    }

    // Verifica se a TAG "type" existe no Json
    if (!j["event"].contains("type") || !j["event"]["type"].is_string())
    {
        return std::make_pair(false, "Elemento 'type' inválido ou ausente");
    }

    /// Roteamento
    if (j["event"]["type"] == "sensor")
    {
        return processSensorData(j);
    }

    if (j["event"]["type"] == "relay")
    {
        return processRelayData(j);
    }

    return std::make_pair(false, "Ocorreu um erro ao processar o evento enviado ou o tipo de evento é desconhecido");
}

std::pair<bool,std::string> PoseidonService::processSensorData(auto& json)
{
    // Biblioteca para manipulação de JSON
    using jsonData = nlohmann::json;
    jsonData j;
    j = json;

    if (!j["event"].contains("sensor_type") || !j["event"]["sensor_type"].is_string())
    {
        return std::make_pair(false, "Elemento 'sensor_type' inválido ou ausente");
    }

    if (!j["event"].contains("sensor_external_id") || !j["event"]["sensor_external_id"].is_string())
    {
        return std::make_pair(false, "Elemento 'sensor_external_id' inválido ou ausente");
    }

    if (!j["event"].contains("value") || !j["event"]["value"].is_number())
    {
        return std::make_pair(false, "Elemento 'value' inválido ou ausente");
    }

    if (!j["event"].contains("read_timestamp") || !j["event"]["read_timestamp"].is_number())
    {
        return std::make_pair(false, "Elemento 'read_timestamp' inválido ou ausente");
    }

    // Gerencia a conexão com o banco de dados
    ConnectionPool pool(DatabaseConfig::connectionString(), 5);
    auto conn = pool.acquire();

    if (!conn) {
        std::cout << "[Poseidon] ERRO: acquire() retornou NULL!" << std::endl;
        return std::make_pair(false, "Erro interno no Modulo Poseidon do Aether durante a conexão com o banco de dados!");
    }

    // Persiste os dados no banco de dados
    const char* paramValues[4];
    std::string deviceName = j["device_name"];
    std::string sensorExternalId = j["event"]["sensor_external_id"];
    std::string sensorValue = std::to_string(j["event"]["value"].get<double>());
    std::string timestampStr = std::to_string(j["event"]["read_timestamp"].get<long>());

    paramValues[0] = deviceName.c_str();
    paramValues[1] = sensorExternalId.c_str();
    paramValues[2] = sensorValue.c_str();
    paramValues[3] = timestampStr.c_str();

    auto& sql = R"(
        INSERT INTO poseidon.dsrd_data_sensor_received
	        (sensor_id, device_id, data_value, read_date)
        VALUES (
	        (SELECT id FROM poseidon.sens_sensor WHERE external_id = $2),
	        (SELECT id FROM poseidon.devc_device WHERE device_name = $1),
	        $3,
	        to_timestamp($4)
        )
    )";

    auto res = conn->queryParams(sql, 4, paramValues);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::string errorMessage = PQerrorMessage(conn->get());
        PQclear(res);
        std::cout << "[Poseidon] ERRO ao inserir dados no banco de dados: " << errorMessage << std::endl;
        return std::make_pair(false, "Erro ao inserir dados no banco de dados: " + errorMessage);
    }

    PostgresDriver::freeResult(res);

    return std::make_pair(true, "Dado inserido com sucesso!");
}

std::pair<bool,std::string> PoseidonService::processRelayData(auto& json)
{
    return std::make_pair(false, "Não implementado");
}

/**
 * @brief Callback chamado quando um pacote TCP identificado é recebido via callback
 * chama a classe de serviços para processar os dados
 */
void PoseidonService::handlePacket(const ProtocolAether::Packet& packet, const std::shared_ptr<IResponseChannel>& channel)
{
    nlohmann::json json;

    if (packet.type == static_cast<uint16_t>(CommandType::DATA_PUSH))
    {
        // Realiza o processamento dos dados recebidos
        auto returnValue = processJsonPacketDataPush(packet);

        json["data"] = nullptr;
        json["message"] = returnValue.second;
        json["success"] = returnValue.first;

        // Converte o JSON em uint8_t
        std::string jsonString = json.dump();
        const std::vector<uint8_t> payload(jsonString.begin(), jsonString.end());

        CommandType responseType;
        if (returnValue.first)
        {
            responseType = CommandType::ACK;
        } else
        {
            responseType = CommandType::ERROR_GENERIC;
        }

        // Envia o retorno ao cliente
        const auto response = ProtocolAether::PacketBuilder::build(
            responseType,
            packet.module,
            payload
        );
        channel->sendResponse(response);
    } else
    {
        json["success"] = false;
        json["message"] = "Comando enviado é invalido e não foi processado pelo modulo Poseidon";
        json["data"] = nullptr;


        std::string jsonString = json.dump();
        const std::vector<uint8_t> payload(jsonString.begin(), jsonString.end());

        const auto response = ProtocolAether::PacketBuilder::build(
            CommandType::ERROR_GENERIC,
            packet.module,
            payload
        );
        channel->sendResponse(response);
    }
}