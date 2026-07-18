#include "Router.hpp"

namespace Aether::Api
{
    /**
     * Roteador de requisições POST
     *
     * @example
     * @code
     *   if (request.path == "/api/users") {
     *       return m_userController.create(request);
     *   }
     * @endcode
     */
    HttpResponse Router::dispatchPost(const HttpRequest&)
    {
        return notFound();
    }

}