#include "StatusController.hpp"
#include "../../include/external/json.hpp"

namespace Aether::Api
{
    /**
     * Processa requisição GET para /api/status
     *
     * Retorna o status atual da API em formato JSON.
     * Delegada ao StatusService para obter os dados.
     */
    HttpResponse StatusController::get(const HttpRequest&)
    {
        auto dto = m_service.get(); // Chama o serviço para obter o status da API

        HttpResponse response;
        response.status = 200;

        nlohmann::json j;
        j["status"] = dto.status;
        j["name"] = dto.name;
        j["timestamp"] = dto.timestamp;
        j["message"] = dto.message;

        response.body = j.dump();
        response.headers["Content-Type"] = "application/json";

        return response;
    }
}