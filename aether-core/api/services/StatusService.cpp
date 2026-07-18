#include "../services/StatusService.hpp"

namespace Aether::Api
{
    /**
     * Retorna um DTO com o status atual da API e do Aether_Core
     */
    Dto::StatusResponse StatusService::get()
    {
        return {
            true,
            "Aether operando com sucesso!"
        };
    }
}