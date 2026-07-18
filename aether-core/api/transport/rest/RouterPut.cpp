#include "Router.hpp"

namespace Aether::Api
{
    /**
     * Roteador de requisições PUT
     *
     * @example
     * @code
     *   if (request.path.find("/api/users/") == 0) {
     *       return m_userController.update(request);
     *   }
     * @endcode
     */
    HttpResponse Router::dispatchPut(const HttpRequest&)
    {
        return notFound();
    }

}