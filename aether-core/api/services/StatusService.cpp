#include "../services/StatusService.hpp"
#include "../core/utils/DateTime.hpp"

namespace Aether::Api
{
    /**
     * Retorna um DTO com o status atual da API e do Aether_Core
     */
    Dto::StatusResponse StatusService::get()
    {
        return {
            "UP",
            "Aether",
            Aether::Core::Utils::DateTime::now_Iso8601(),
            "Core is operational"
        };
    }
}