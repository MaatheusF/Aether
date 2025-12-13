#pragma once

#include <memory>

#include "../../protocols/aether/common/IResponseChannel.hpp"
#include "../../protocols/aether/include/Packet.hpp"

class TcpConnection;

/**
 * @brief Canal de resposta TCP para enviar pacotes de resposta.
 *
 * A classe TcpResponseChannel herda de IResponseChannel e é responsável por
 * enviar pacotes de resposta através de uma conexão TCP. Ela encapsula uma
 * instância de TcpConnection para gerenciar a comunicação.
 */
class TcpResponseChannel : public IResponseChannel
{
    public:
        /**
         * @brief Construtor que inicializa o canal de resposta com uma conexão TCP.
         * @param conn Ponteiro compartilhado para a conexão TCP.
         */
        explicit TcpResponseChannel(std::shared_ptr<TcpConnection> conn);

        /**
         * @brief Envia um pacote de resposta através da conexão TCP.
         * @param pkt Pacote a ser enviado.
         */
        void sendResponse(const ProtocolAether::Packet& pkt) override;

        /**
         * @brief Retorna o identificador do canal de resposta.
         * @return Identificador do canal.
         */
        virtual uint16_t id() const override;
    private:
        std::shared_ptr<TcpConnection> connection;  /// Conexão TCP utilizada para enviar respostas.
};