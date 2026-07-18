#pragma once

#include "Router.hpp"

namespace Aether::Api
{
    class HttpServer
    {
        public:
            bool start(int port);

        private:
            Router m_router;
    };
}