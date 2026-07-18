#include "StatusController.hpp"

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
        auto dto = m_service.get();

        HttpResponse response;
        response.status = 200;
        response.body =
            "{"
            "\"success\":true,"
            "\"message\":\"" + dto.message + "\""
            "}";
        response.headers["Content-Type"] = "application/json";

        return response;
    }

}