#include <chrono>
#include <iostream>
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

#define SOCKET_PATH "/tmp/aetherd.socket"

/**Função principal do daemon que inicia o AetherCore, modulos, bibliotecas e serviços, alem de criar um socket UNIX
 *para comunicação local entre os serviços LINUX. Inicia um servidor local socket usando AF_UNIX que recebe comandos,
 *processa e retorna o status para o CLI (TODO: É ideial portar o servidor que response o CLI para outro diretorio)
 */
int main() {
    std::cout << "AETHER daemon starting..." << std::endl;
    unlink(SOCKET_PATH); // Exclui um arquivo de socket Unix do Sistema (Garante que não tenha lixo na memoria)

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // Cria um Socket do tipo UNIX
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); //Copia o caminho do socket para o membro do sockaddr

    //Associa o socket (bind) a um endereço e porta
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    std::cout << "[Aetherd] Daemon iniciado. Socket: " << SOCKET_PATH << "\n";

    // TODO: iniciar core, carregar módulos, adapters etc

    while (true) {
        // Loop do daemon

        int client_fd = accept(server_fd, nullptr, nullptr); //Bloqueia o while e aguarda uma conexão do ciente
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[512];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) -1);

        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            std::string cmd(buffer);

            std::cout << "[Aetherd] Comando recebido: " << cmd << "\n";

            std::string response; //String que armazena o dado de retorno do comando

            if (cmd == "alive") {
                response = "ACK";
            }
            else {
                response = "ERR: comando desconhecido\n" + cmd;
            }

            write(client_fd, response.c_str(), response.size()); //Envia o retorno do comando para o CLI
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
