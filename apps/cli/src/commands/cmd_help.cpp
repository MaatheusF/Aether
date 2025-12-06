#include "../../include/commands/cmd_help.hpp"
#include <string>
#include <iostream>
#include <vector>
#include "../../include/utils.hpp"

/**
 * @brief Commando "help" que retorna dados de ajuda e a lista completa de comandos disponíveis
 * @param arg Argumentos do comando <subcomandos>
 */
int cmd_help(const std::vector<std::string>& arg)
{
    Utils::printWelcomeMessage(); //Printa o logo do AETHER

    std::cout << "\033[37mCommands:" << std::endl;
    std::cout << "  help     -  Exibe dados essenciais de apoio do CLI ou do Aether alem da lista de comandos disponíveis." << std::endl;
    std::cout << "  aether   -  Inicia o modo Shell" << std::endl;
    std::cout << "  version  -  Exibe a versão atual do Aether." << std::endl;
    std::cout << "  alive    -  Verifica se o Daemon está respondendo." << std::endl;

    std::cout << "\n\n" << std::endl;

    return 0;
}