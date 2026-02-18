#include "../include/Parser.hpp"
#include "common/IProtocolHandler.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>

namespace ProtocolAether
{
    static constexpr uint16_t MAGIC = 0xAA55;                   /// Valor mágico para identificar o início do pacote
    static constexpr size_t HEADER_SIZE = 2 + 1 + 2 + 2 + 4;    /// Tamanho do cabeçalho do pacote

    Parser::Parser() = default; /// Construtor padrão

    /**
     * Define o handler para processar os pacotes recebidos
     * @param h Ponteiro para o handler
     */
    void Parser::setHandler(IProtocolHandler* h)
    {
        handler = h;
    }

    /**
     * Define o callback para processar os pacotes recebidos
     * @param cb Função de callback
     */
    void Parser::setOnPacket(OnPacket cb)
    {
        onPacket = std::move(cb);
    }

    /**
     * Alimenta o parser com dados recebidos
     * @param buffer Buffer de dados recebidos
     * @param channel Canal de resposta para enviar respostas, se necessário
     */
    void Parser::feed(std::vector<uint8_t>& buffer, std::shared_ptr<IResponseChannel> channel)
    {
        std::cout << "[Parser] feed() buffer size=" << buffer.size() << std::endl;
        while (true)
        {
            Packet packet;  // Cria um novo pacote para armazenar os dados parseados

            /// Tenta parsear um pacote do buffer
            if (!tryParsePacket(buffer, packet))
            {
                break; /// Sai do loop se não houver pacotes completos
            }

            if (handler)
            {
                handler->onPacket(packet, channel); /// Chama o handler se estiver definido
            } else
            {
                std::cout << "[Parser] onPacket NULL" << std::endl;
            }
        }
    }

    /**
     * Tenta parsear um pacote do buffer
     * @param buffer Buffer de dados
     * @param outPacket Referência para armazenar o pacote parseado
     * @return true se um pacote foi parseado, false caso contrário
     */
    bool Parser::tryParsePacket(std::vector<uint8_t>& buffer, Packet& outPacket)
    {
        if (buffer.size() < HEADER_SIZE)
        {
            return false;
        }

        size_t offset = 0;  // Offset para leitura no buffer

        /// Funções auxiliares para ler dados do buffer
        auto read16 = [&](uint16_t& v) {
            v = (buffer[offset] << 8) | buffer[offset + 1];
            offset += 2;
        };

        /// Lê um valor de 32 bits do buffer
        auto read32 = [&](uint32_t& v) {
            v = (buffer[offset] << 24) |
                (buffer[offset + 1] << 16) |
                (buffer[offset + 2] << 8) |
                buffer[offset + 3];
            offset += 4;
        };

        /// Lê o valor mágico do pacote
        read16(outPacket.magic);

        /// Verifica se o valor mágico é válido
        if (outPacket.magic != MAGIC)
        {
            std::cerr << "[Parser] Magic inválido, descartando 1 byte\n";
            buffer.erase(buffer.begin());
            return true; // tenta de novo
        }

        /// Lê o restante do cabeçalho do pacote
        outPacket.version = buffer[offset++];

        /// Lê o tipo, módulo e comprimento do payload
        read16(outPacket.type);
        read16(outPacket.module);
        read32(outPacket.length);

        /// Verifica se o buffer contém o payload completo
        if (buffer.size() < HEADER_SIZE + outPacket.length)
        {
            return false; /// Aguarda mais dados
        }

        /// Lê o payload do pacote
        outPacket.payload.resize(outPacket.length);
        /// Copia os dados do payload para o pacote
        std::memcpy(outPacket.payload.data(), buffer.data() + HEADER_SIZE, outPacket.length);
        /// Remove os dados processados do buffer
        buffer.erase(buffer.begin(), buffer.begin() + HEADER_SIZE + outPacket.length);

        return true;
    }
}
