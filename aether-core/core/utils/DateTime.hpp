#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace Aether::Core::Utils
{
    class DateTime
    {
    public:
        /**
         * @brief Retorna o timestamp atual no formato "Y-m-dTH:M:S.Z".
         * @return std::string Timestamp formatado.
         */
        static std::string now_Iso8601();

        /**
         * @brief Retorna o timestamp atual no formato "YYYY-MM-DD HH:MM:SS".
         * @return std::string Timestamp formatado.
         */
        static std::string now();
    };
}
