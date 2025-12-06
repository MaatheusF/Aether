#pragma once
#include <string>
#include <vector>

/**
 * @brief Commando "help" que retorna dados de ajuda e a lista completa de comandos disponíveis
 * @param arg Argumentos do comando <subcomandos>
 */
int cmd_help(const std::vector<std::string>& arg);