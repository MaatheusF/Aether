#include "../include/daemon.hpp"

/**Função principal do daemon que inicia o AetherCore, modulos, bibliotecas e serviços, alem de criar um socket UNIX
 *para comunicação local entre os serviços LINUX. Inicia um servidor local socket usando AF_UNIX que recebe comandos,
 *processa e retorna o status para o CLI (TODO: É ideial portar o servidor que response o CLI para outro diretorio)
 */
int main() {

    // ======================================
    //      INICIALIZAÇÃO DO DAEMON
    // ======================================

    AetherDaemon daemon;
    return daemon.initializeAetherDaemon();
}
