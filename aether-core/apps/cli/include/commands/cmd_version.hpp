#pragma once
#include <string>
#include <vector>

/**
 * @brief Commando "version" que retorna a versão atual do Aether e mais informações basicas
 * @param arg Argumentos do comando <subcomandos>
 */
int cmd_version(const std::vector<std::string>& arg);