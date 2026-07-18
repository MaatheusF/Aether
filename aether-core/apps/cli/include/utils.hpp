#pragma once
#include <string>

namespace Utils
{
    /**
     * @brief Obtém o timestamp atual formatado.
     * @return std::string Timestamp formatado "YYYY-MM-DD HH:MM:SS".
     */
    std::string returnCurrentTimeStamp();

    /**
     * @brief Função que printa no console a mensagem de boas vindas do AetherCLI
     * @return retorna 0 se concluido
     */
    int printWelcomeMessage();
}
