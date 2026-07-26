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
     * - GET /api/cameras/:channel/snapshot -> CameraController::getSnapshot()
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

        if (request.path.find("/api/cameras/") == 0)
        {
            return m_cameraController.getSnapshot(request);
        }

        return notFound();
    }

}