#include "TcpResponseChannel.hpp"
#include "TcpConnection.hpp"
#include "../../protocols/aether/include/PacketBuilder.hpp"

#include <iomanip>
#include <iostream>

/**
 * @brief Construtor da classe TcpResponseChannel.
 * @param conn Ponteiro compartilhado para a conexão TCP associada.
 */
TcpResponseChannel::TcpResponseChannel(std::shared_ptr<TcpConnection> conn) : connection(std::move(conn)) {}

/**
 * @brief Envia uma resposta através do canal TCP. (Serializa o pacote e envia os bytes pela conexão)
 *
 * Servidor -> Cliente
 *
 * Sobrescreve a função pura da classe base ResponseChannel.
 * @param pkt Pacote a ser enviado.
 */
void TcpResponseChannel::sendResponse(const ProtocolAether::Packet& pkt)
{
    /// Serializa o pacote em bytes usando o PacketBuilder
    auto bytes = ProtocolAether::PacketBuilder::encode(pkt);


    /// Debugger para mostrar os bytes enviados
    std::cout << "[TcpResponseChannel] Enviando " << bytes.size() << " bytes: ";

    for (auto b : bytes)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    std::cout << std::dec << std::endl;

    /// Envia os bytes pela conexão TCP
    connection->sendBytes(bytes);
}

/**
 * @brief Obtém o identificador único do canal TCP.
 * @return Identificador único do canal.
 */
uint16_t TcpResponseChannel::id() const
{
    return static_cast<uint16_t>(
        reinterpret_cast<uintptr_t>(connection.get()) & 0xFFFF  /// Usa os últimos 16 bits do ponteiro da conexão como ID
    );
}
