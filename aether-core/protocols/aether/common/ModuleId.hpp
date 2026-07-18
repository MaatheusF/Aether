#pragma once
#include <cstdint>

/**
 * Classe enum que lista todos os modulos do Aether com seus respectivos Id's,
 * utilizados principalmente no registro de modulos para aceitar conexões TCP
 *
 * @attention migrar isso para um banco de dados no futuro
 */
enum class ModuleId : uint8_t
{
    CORE  = 0x01,
    MODULETEST  = 0x02,
    MODULE_POSEIDON = 0x03
};