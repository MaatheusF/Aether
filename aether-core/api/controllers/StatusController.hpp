#pragma once

#include "../services/StatusService.hpp"
#include "../common/HttpResponse.hpp"

namespace Aether::Api
{
    class StatusController
    {
        public:
            HttpResponse get();
        private:
            StatusService service;
    };
}