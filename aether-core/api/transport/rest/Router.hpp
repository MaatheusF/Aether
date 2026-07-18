#pragma once

#include "../../common/HttpResponse.hpp"
#include "../../controllers/StatusController.hpp"

#include <string>

namespace Aether::Api
{
    class Router
    {
        public:
            HttpResponse route(const std::string& method, const std::string& path);
        private:
            StatusController m_statusController;
    };
}