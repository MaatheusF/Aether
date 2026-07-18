#include "Router.hpp"

namespace Aether::Api
{
    /**
     * Roteador principal que inspeciona o método HTTP
     * e delega para o handler apropriado.
     *
     * Fluxo:
     * 1. Recebe HttpRequest
     * 2. Verifica request.method
     * 3. Chama dispatchGet, dispatchPost, etc
     * 4. Retorna HttpResponse
     */
    HttpResponse Router::dispatch(const HttpRequest& request)
    {
        switch (request.method)
        {
            case HttpMethod::GET:
                return dispatchGet(request);
            case HttpMethod::POST:
                return dispatchPost(request);
            case HttpMethod::PUT:
                return dispatchPut(request);
            case HttpMethod::DELETE_:
                return dispatchDelete(request);
            default:
                return notFound();
        }
    }

    /**
     * Retorna resposta 404 Not Found em JSON
     */
    HttpResponse Router::notFound() const
    {
        HttpResponse response;

        response.status = 404;
        response.body =
            R"({"success":false,"message":"Not Found"})";
        response.headers["Content-Type"] =
            "application/json";

        return response;
    }
}