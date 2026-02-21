#pragma once
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../../protocols/aether/common/IResponseChannel.hpp"
#include "../../../protocols/aether/include/PacketBuilder.hpp"
#include "../../../protocols/aether/include/CommandType.hpp"
#include "../../../protocols/aether/common/ModuleId.hpp"
#include "../../../core/network/SessionManager.hpp"

/**
 * @brief Representa uma sessão de conexão com um dispositivo remoto.
 *
 * ConnSession gerencia o handshake inicial (HELLO) e, em seguida,
 * encaminha os pacotes da aplicação para o callback registrado.
 * Também administra o estado da sessão (handshaking, pronta, encerrando)
 * e fornece callbacks simples de sucesso/falha para a fase de handshake.
 *
 * A classe é intencionalmente pequena e não copiável (contém um
 * std::shared_ptr para um IResponseChannel). Ela espera que o chamador
 * alimente os bytes recebidos por meio da função feed() conforme eles
 * chegam da camada de rede.
 */
class ConnSession
{
public:
    /**
     * @brief Informações de sessão relacionadas a uma sessão estabelecida.
     *
     * Contém o canal de resposta utilizado para enviar respostas,
     * o identificador externo do dispositivo obtido durante o handshake
     * e um indicador que informa se o dispositivo remoto foi identificado.
     */
    /*
    struct SessionInfo {
        std::shared_ptr<IResponseChannel> channel; /// Canal utilizado para enviar respostas
        std::string deviceExternalId;              /// Identificador externo fornecido pelo dispositivo
        bool isIdentify = false;                   /// Indica se a identificação foi concluída
    };*/

    /**
     * @brief Estado atual do ciclo de vida da sessão.
     */
    enum class SessionState {
        Handshaking, /**< Aguardando HELLO e conclusão do handshake */
        Ready,       /**< Handshake concluído, pronto para encaminhar pacotes */
        Closing      /**< Sessão marcada para encerramento; novos bytes recebidos serão ignorados */
    };

    using OnHandshakeComplete = std::function<void(std::vector<uint8_t>&)>; /**< Chamado quando o handshake é concluído com sucesso. Recebe os bytes restantes (se houver). */
    using OnHandshakeFailed   = std::function<void()>;                      /**< Chamado quando o handshake falha e a sessão será encerrada. */

    /**
     * @brief Constrói uma nova ConnSession.
     * @param channel Canal de resposta utilizado para enviar respostas ao par remoto.
     */
    explicit ConnSession(std::shared_ptr<IResponseChannel> channel)
        : channel_(std::move(channel)), state_(SessionState::Handshaking) {}

    /**
     * @brief Destrutor da ConnSession.
     *
     * Garante que o canal seja desregistrado do SessionManager ao destruir a sessão,
     * evitando entradas obsoletas em caso de falha no handshake ou encerramento da sessão.
     */
    ~ConnSession()
    {
        if (channel_)
            SessionManager::instance().unregisterChannel(channel_);
    }

    /**
     * @brief Alimenta a sessão com bytes recebidos.
     *
     * Se a sessão estiver no estado de handshake, os bytes serão armazenados
     * em buffer e processados até que o handshake HELLO seja concluído.
     * Quando a sessão estiver no estado Ready, os bytes serão encaminhados
     * para o callback OnHandshakeComplete (se configurado), permitindo que
     * o chamador processe os pacotes da aplicação.
     *
     * @param bytes Bytes recebidos da rede
     */
    void feed(std::vector<uint8_t>& bytes)
    {
        if (state_ == SessionState::Handshaking)
        {
            handleHandshake(bytes);
            return;
        }

        if (state_ == SessionState::Closing)
            return; // Conexão marcada para fechar, ignora novos dados

        if (state_ == SessionState::Ready && onHandshakeComplete_)
            onHandshakeComplete_(bytes);
    }

    /** @brief Obtém o estado atual da sessão. */
    SessionState getState() const { return state_; }
    /** @brief Obtém o identificador externo do dispositivo descoberto durante o handshake. */
    const std::string& getDeviceId() const { return deviceExternalId_; }

    /** @brief Registra o callback invocado quando o handshake é concluído com sucesso. */
    void setOnHandshakeComplete(const OnHandshakeComplete& cb) { onHandshakeComplete_ = cb; }
    /** @brief Registra o callback invocado quando o handshake falha. */
    void setOnHandshakeFailed(const OnHandshakeFailed& cb)     { onHandshakeFailed_ = cb; }

private:
    static constexpr uint16_t MAGIC       = 0xAA55; /**< Valor mágico esperado no cabeçalho (big-endian) */
    static constexpr uint16_t CMD_HELLO   = 0x0004; /**< Identificador de comando para HELLO */
    static constexpr uint8_t  VERSION     = 0x01;   /**< Versão do protocolo suportada */
    static constexpr size_t   HEADER_SIZE = 11;     /**< Tamanho do cabeçalho: 2 (magic) + 1 (versão) + 2 (cmd) + 2 (origem) + 4 (tamanho do payload) */

    /**
     * @brief Processador interno de handshake.
     *
     * Esta função acumula bytes em um buffer interno até que um pacote
     * completo de handshake esteja disponível. Ele valida o valor mágico,
     * o tipo de comando e o tamanho do payload. Em caso de sucesso,
     * extrai o identificador do dispositivo, limpa o buffer, altera o
     * estado para Ready e envia um ACK.
     * Em caso de falha, envia uma resposta de erro e marca a sessão
     * como Closing.
     *
     * Isso é uma copia do protocolo para dentro do ConnSession, daria pra chamar o parser por fora e remover essa função,
     * mas achei mais simples manter aqui para evitar acoplamento do parser com o ciclo de vida da sessão.
     *
     * @param bytes Novos bytes recebidos para processamento.
     */
    void handleHandshake(std::vector<uint8_t>& bytes)
    {
        buffer_.insert(buffer_.end(), bytes.begin(), bytes.end());

        if (buffer_.size() < HEADER_SIZE)
            return;

        uint16_t magic = (static_cast<uint16_t>(buffer_[0]) << 8) | buffer_[1];
        if (magic != MAGIC)
        {
            rejectHandshake("Magic inválido");
            return;
        }

        uint16_t cmd = (static_cast<uint16_t>(buffer_[3]) << 8) | buffer_[4];
        if (cmd != CMD_HELLO)
        {
            rejectHandshake("Primeiro pacote deve ser HELLO");
            return;
        }

        uint32_t payloadLen =
            (static_cast<uint32_t>(buffer_[7])  << 24) |
            (static_cast<uint32_t>(buffer_[8])  << 16) |
            (static_cast<uint32_t>(buffer_[9])  << 8)  |
            (static_cast<uint32_t>(buffer_[10]));

        if (buffer_.size() < HEADER_SIZE + payloadLen)
            return;

        deviceExternalId_ = std::string(
            buffer_.begin() + HEADER_SIZE,
            buffer_.begin() + HEADER_SIZE + payloadLen
        );

        buffer_.clear();
        state_ = SessionState::Ready;

        // Registra o canal no SessionManager para que outros módulos possam consultar o deviceExternalId
        SessionManager::instance().registerChannel(channel_, deviceExternalId_);

        auto response = ProtocolAether::PacketBuilder::build(
            CommandType::ACK,
            static_cast<uint16_t>(ModuleId::CORE),
            {}
        );
        channel_->sendResponse(response);

        std::cout << "[ConnSession] Handshake OK - deviceId=" << deviceExternalId_ << std::endl;
    }

    /**
     * @brief Rejeita o handshake, notifica o par remoto e marca a sessão para encerramento.
     *
     * Envia um pacote ERROR_GENERIC contendo o motivo em formato legível,
     * limpa o buffer interno e altera o estado para Closing. Também
     * invoca o callback OnHandshakeFailed, caso esteja registrado.
     */
    void rejectHandshake(const std::string& reason)
    {
        std::cout << "[ConnSession] Handshake FALHOU - " << reason << std::endl;

        auto response = ProtocolAether::PacketBuilder::build(
            CommandType::FAIL_HANDSHAKE,
            static_cast<uint16_t>(ModuleId::CORE),
            std::vector<uint8_t>(reason.begin(), reason.end())
        );
        channel_->sendResponse(response);

        buffer_.clear();

        // Desregistra do SessionManager (idempotente)
        if (channel_)
            SessionManager::instance().unregisterChannel(channel_);

        state_ = SessionState::Closing;

        if (onHandshakeFailed_)
        {
            onHandshakeFailed_();
        }
    }

    std::shared_ptr<IResponseChannel> channel_; /**< Canal utilizado para enviar respostas ao cliente remoto */
    SessionState state_;                        /**< Estado atual do ciclo de vida */
    std::string deviceExternalId_;              /**< Identificador do dispositivo obtido no handshake */
    std::vector<uint8_t> buffer_;               /**< Buffer utilizado durante o handshake */
    OnHandshakeComplete onHandshakeComplete_;   /**< Callback invocado quando o handshake é concluído */
    OnHandshakeFailed   onHandshakeFailed_;     /**< Callback invocado quando o handshake falha */
};

