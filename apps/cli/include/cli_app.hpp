#pragma once
#include <vector>
#include <string>

/**
 * @brief Classe principal do CLI Aether Daemon
 *
 * - Responsavel por receber os argumentos digitados no console.
 * - Decide se vai executar em modo one-shot ou shell
 * - Delega trabalho para outros componentes do CLI
 */
class CliApp
{
    public:
        /**
         * @brief Função que inicializa a execução do CLI
         * @param argc Quantidade de argumentos fornecidos
         * @param argv Array de argumentos
         */
        int run(int argc, char** argv);
        /**
         * @brief Função que envia comando ao AtherD Daemon
         * @param command Comandos do tipo std::string que será enviado
         * @return retorna um comando do tipo std::String de retorno
         */
        std::string sendCommand(std::string command);

    private:
        /**
         * @brief Função responsavel por processar comandos diretos / sem --shel
         * @param args argumentos do comando
         * @deprecated Não mais utilizado, apenas via Shell
         */
        int runOneShot(const std::vector<std::string>& args);
        /**
         * @brief Função responsavel por iniciar um Shell com o comando "aether"
         */
        int runShell();

        /**
         * @brief Implementa o comando core stop | core start para o Daemon, parando os modulos do Sistema
         * @param args argumentos fornecidos no Shell
         */
        void handleCoreCommand(const std::vector<std::string>& args);

        /**
         * @brief Implementa uma função para exibir os logs do Aether (tail -f)
         * @param args argumentos fornecidos no Shell
         */
        static void handleLogsCommand(const std::vector<std::string>& args);

        /**
         * @brief Quebra as linha do terminal em Tokens (Faz split por espaço) e transforma em args
         * @param line
         */
        static std::vector<std::string> splitArgs(const std::string& line);
};