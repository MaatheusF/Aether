#pragma once

#include <string>

namespace Aether::Api
{
    struct HttpResponse
    {
        int status;
        std::string body;
    };
}