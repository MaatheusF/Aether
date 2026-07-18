#include "Router.hpp"

namespace Aether::Api
{
    HttpResponse Router::route(const std::string& method, const std::string& path)
    {
        if (method == "GET" && path == "/api/test")
        {
            return m_statusController.get();
        }

        return {
            404,
            R"({"success":false, "message":"Method Not Found"})"
        };
    }
}