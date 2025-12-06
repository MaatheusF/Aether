#include "../include/utils.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace Utils
{
    /**
  * @brief Retorna o timestamp atual formatado como string.
  *
  * Esta função obtém o horário atual do sistema, converte para uma
  * estrutura de tempo local e retorna uma string no formato:
  * "YYYY-MM-DD HH:MM:SS".
  *
  * @return std::string Timestamp formatado.
  */
    std::string returnCurrentTimeStamp()
    {
        const auto now = std::chrono::system_clock::now();                          // Obtém o horário atual
        const std::time_t time_now = std::chrono::system_clock::to_time_t(now);     // Converte converte de time_point para time_t

        std::tm local_tm{};                                                         // Estrutura para tempo local
        localtime_r(&time_now, &local_tm);                                          // Converte para horário local de forma thread-safe

        std::ostringstream oss;
        oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");                       // Formata para o formato esperado

        return oss.str();
    }

    /**
     * @brief Função que printa no console a mensagem de boas vindas do AetherCLI
     * @return retorna 0 se concluido
     */
    int printWelcomeMessage()
    {
        std::cout << "\033[2J\033[H";   //Limpa o console
        std::cout <<
            "\n\n"
            "\033[96m"
            "     .oo .oPYo. ooooo  o    o .oPYo.  .oPYo. \n"
            "    .P 8 8.       8    8    8 8.      8   `8 \n"
            "   .P  8 `boo     8   o8oooo8 `boo   o8YooP' \n"
            "  oPooo8 .P       8    8    8 .P      8   `b \n"
            " .P    8 8        8    8    8 8       8    8 \n"
            ".P     8 `YooP'   8    8    8 `YooP'  8    8 \n"
            "..:::::..:.....:::..:::..:::..:.....::..:::..\n"
            ":::::::::::::::::::::::::::::::::::::::::::::\n"
            //":::::::::::::::::::::::::::::::::::::::::::::\033[0m\n\n\n"
            "\033[96m::::: \033[94m~ AETHER Command Line Interface ~ \033[96m:::::\033[0m\n\n\n"
        ;

        return 0;
    }
}
