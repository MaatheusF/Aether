#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <optional>
#include "Packet.hpp"

class IResponseChannel;
class IProtocolHandler;

namespace ProtocolAether
{
    /**
     * @brief Classe responsável por analisar e interpretar pacotes do protocolo Aether.
     *
     * A classe Parser é responsável por receber dados brutos, analisar e interpretar
     * pacotes do protocolo Aether, e acionar callbacks apropriados quando pacotes válidos
     * são detectados.
     */
    class Parser
    {
    public:
        /**
         * @brief Tipo de função de callback para pacotes analisados.
         * A função recebe um pacote analisado e um canal de resposta associado.
         */
        using OnPacket = std::function<void(const Packet&, std::shared_ptr<IResponseChannel>)>;

        /**
         * @brief Construtor padrão da classe Parser.
         */
        Parser();

        /**
         * @brief Alimenta o parser com dados brutos para análise.
         * @param buffer Vetor de bytes contendo os dados brutos a serem analisados.
         * @param channel Canal de resposta associado aos dados recebidos.
         */
        void feed(std::vector<uint8_t>& buffer, std::shared_ptr<IResponseChannel> channel);

        /**
         * @brief Define o callback a ser acionado quando um pacote válido for analisado.
         * @param cb Função de callback que será chamada com o pacote analisado e o canal de resposta.
         */
        void setOnPacket(OnPacket cb);

        /**
         * @brief Define o manipulador de protocolo associado ao parser.
         * @param handler Ponteiro para o manipulador de protocolo.
         */
        void setHandler(IProtocolHandler* handler);
    private:
        /**
         * @brief Tenta analisar um pacote a partir do buffer fornecido.
         * @param buffer Vetor de bytes contendo os dados a serem analisados.
         * @param outPacket Referência para o pacote onde o resultado da análise será armazenado.
         * @return true se um pacote válido foi analisado, false caso contrário.
         */
        bool tryParsePacket(std::vector<uint8_t>& buffer, Packet& outPacket);
        OnPacket onPacket;                      /// Callback para pacotes analisados
        IProtocolHandler* handler = nullptr;    /// Manipulador de protocolo associado
    };
}