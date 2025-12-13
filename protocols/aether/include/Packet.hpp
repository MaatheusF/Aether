#pragma once
#include <cstdint>
#include <vector>

/**
    * @brief Representa um pacote do protocolo Aether em formato estruturado.
    *
    * Esta estrutura contém todos os campos necessários para montar ou interpretar
    * um pacote enviado via TCP entre cliente e servidor.
    * Ela é utilizada pelo `Parser::encode()` e `Parser::decode()` para realizar
    * a serialização/deserialização dos dados.
    *
    * +---------+---------+---------+---------+--------------------------+
    * | Byte(s) | Campo   | Descrição                                    |
    * +---------+---------+----------------------------------------------+
    * |  0..1   | MAGIC   | Numero de identificação do protocolo         |
    * |   2     | version | Versão do protocolo                          |
    * | 3..4    | type    | Código do comando (CommandCode2)             |
    * | 5..6    | module  | Módulo de destino do pacote.                 |
    * | 7..10   | length  | Tamanho do payload (uint32_t, big-endian)    |
    * | 11..N   | payload | Dados brutos                                 |
    * | Futuro  | crc     | Campo de CRC (ainda não implementado)        |
    * +---------+---------+----------------------------------------------+
    */
namespace ProtocolAether
{
    struct Packet {

        static constexpr uint8_t MAGIC_1 = 0xAA; /// Magic | Numero de identificação do protocolo
        static constexpr uint8_t MAGIC_2 = 0x55; /// Magic | Numero de identificação do protocolo

        uint8_t  magic1 = MAGIC_1;              /// Magic | Numero de identificação do protocolo
        uint8_t  magic2 = MAGIC_2;              /// Magic | Numero de identificação do protocolo
        std::uint8_t version;                   /// Version | Versão do protocolo
        uint16_t type;                          /// Type | Tipo do pacote (comando).
        uint16_t module;                        /// Module | Módulo de destino do pacote.
        uint32_t length;                        /// Length | Tamanho do payload em bytes.
        std::vector<uint8_t> payload;           /// Payload | Dados do pacote.
    };
}
