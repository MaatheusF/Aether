#include "StatusController.hpp"

namespace Aether::Api
{
    HttpResponse StatusController::get()
    {
        auto dto = service.getStatus();

        HttpResponse response;
        response.status = 200;
        response.body =
            "{"
            "\"success\":true,"
            "\"message\":\"" + dto.message + "\""
            "}";

        return response;
    }

}