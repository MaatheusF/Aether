#include "Router.hpp"

namespace Aether::Api
{
    /**
     * Roteador de requisições GET
     *
     * Faz roteamento simples por path string matching:
     *
     * Routes:
     * - GET /api/status -> StatusController::get()
     *
     * Para sistemas com muitas rotas, considere usar RouteRegistry
     * que permite registro de rotas de forma centralizada e legível.
     */
    HttpResponse Router::dispatchGet(const HttpRequest& request)
    {
        if (request.path == "/api/status")
        {
            return m_statusController.get(request);
        }

        return notFound();
    }

}