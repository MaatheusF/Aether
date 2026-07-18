#include "logger.hpp"

#include <chrono>
#include <iomanip>
#include <string>

// Definição da variável estática
std::ofstream AetherCoreLogger::logFile;

/**
 * @brief Retorna o timestamp atual no formato "YYYY-MM-DD HH:MM:SS".
 * @return std::string Timestamp formatado.
 */
std::string AetherCoreLogger::returnCurrentTimeStamp()
{
    const auto now = std::chrono::system_clock::now();                          // Obtém o horário atual
    const std::time_t time_now = std::chrono::system_clock::to_time_t(now);     // Converte converte de time_point para time_t

    std::tm local_tm{};                                                          // Estrutura para tempo local
    localtime_r(&time_now, &local_tm);                                           // Converte para horário local de forma thread-safe

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");                       // Formata para o formato esperado

    return oss.str();
}