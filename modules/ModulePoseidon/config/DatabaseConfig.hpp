#pragma once
#include <string>

/**
 * @brief Configuração estática de banco de dados para o modulo Poseidon
 */
class DatabaseConfig {
public:
    static constexpr const char* HOST     = "192.168.0.133";
    static constexpr int         PORT     = 5432;
    static constexpr const char* USER     = "POSEIDON_APPLICATION";
    static constexpr const char* PASSWORD = "favero10";
    static constexpr const char* DATABASE = "aetherdb";
    static constexpr const char* SCHEMA   = "poseidon";

    /// Gera a connection string pronta
    static std::string connectionString() {
        return "host=" + std::string(HOST)
            + " port=" + std::to_string(PORT)
            + " user=" + USER
            + " password=" + PASSWORD
            + " dbname=" + DATABASE
            + " options='-c search_path=" + SCHEMA + "'";
    }
};