#include "Router.hpp"

namespace Aether::Api
{
    /**
     * Roterador de requisições DELETE
     *
     * @example
     * @code
     *   if (request.path.find("/api/users/") == 0) {
     *       return m_userController.delete_(request);
     *   }
     * @endcode
     */
    HttpResponse Router::dispatchDelete(const HttpRequest&)
    {
        return notFound();
    }

}