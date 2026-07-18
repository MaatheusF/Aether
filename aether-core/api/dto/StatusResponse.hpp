#pragma once
#include <string>

namespace Aether::Api::Dto
{
    struct StatusResponse
    {
        bool success;
        std::string message;
    };
}
