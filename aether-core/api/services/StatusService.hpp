#pragma once

#include "../dto/StatusResponse.hpp"

namespace Aether::Api
{
    class StatusService{
    public:
        static Dto::StatusResponse getStatus();
    };
}