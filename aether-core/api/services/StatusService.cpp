#include "../services/StatusService.hpp"

namespace Aether::Api
{
    Dto::StatusResponse StatusService::getStatus()
    {
        return {
            true,
            "Aether API Funcionando"
        };
    }
}