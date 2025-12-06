#include <chrono>
#include <iostream>
#include <thread>

int main() {
    std::cout << "AETHER daemon starting..." << std::endl;

    // TODO: iniciar core, carregar módulos, adapters etc

    while (true) {
        // Loop do daemon (futuro scheduler)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
