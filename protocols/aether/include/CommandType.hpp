#pragma once
#include <cstdint>

/**
 * @brief Códigos de comando utilizados no protocolo Aether.
 *
 * Esta enumeração define os códigos de comando que são utilizados para identificar
 * o tipo de operação ou mensagem em pacotes enviados via TCP entre cliente e servidor.
 *
 * Os códigos são organizados em faixas para diferentes categorias de comandos:
 * - 0x0001 a 0x00FF: Comandos do CORE / controle de conexão / handshake
 * - 0x0100 a 0x01FF: Mensagens genéricas de dados (bidirecionais)
 * - 0x1000 a 0xFFFF: Reservado para módulos específicos
 *
 * [Magic:2][Version:1][CmdType:2][ModuleId:2][Length:4][Payload:N]
 */
enum class CommandType : uint16_t {
    /// ----------------------------------------------------------------------
    /// 0x0001 – 0x00FF → Comandos do CORE / controle de conexão / handshake
    /// ----------------------------------------------------------------------

    PING                = 0x0001,       /// Envia ou recebe um pacote de ping para verificar a conectividade.
    PONG                = 0x0002,       /// Resposta ao pacote de ping.
    ERROR_GENERIC       = 0x0003,       /// Indica que ocorreu um erro generico ao processar o pacote recebido.
    HELLO               = 0x0004,       /// Inicia o processo de handshake entre cliente e servidor.
    HELLO_ACK           = 0x0005,       /// Confirma o recebimento do handshake.
    HEARTBEAT           = 0x0006,       /// Pacote de heartbeat para manter a conexão ativa.
    ACK                 = 0x0007,       /// Confirma o recebimento de um pacote específico.
    MODULE_UNAVAILABLE  = 0x0008,       /// Indica que o modulo de comunicação está indisponível ou parado, não podendo responder no momento

    /// ----------------------------------------------------------------------
    /// 0x0100 – 0x01FF → Mensagens genéricas de dados (bidirecionais)
    /// ----------------------------------------------------------------------

    DATA_REQUEST        = 0x0100,       /// Solicita dados específicos do servidor ou de um módulo.
    DATA_RESPONSE       = 0x0101,       /// Responde a uma solicitação de dados com as informações requisitadas.
    DATA_PUSH           = 0x0102,       /// Envia dados do cliente para o servidor sem solicitação prévia.

    // ----------------------------------------------------------------------
    // 0x1000 – 0xFFFF → Reservado para módulos específicos
    //
    //  0x1xxx – Módulo de Arquivos
    //  0x2xxx – Módulo de Rede
    //  0x3xxx – Módulo de CLI
    //  0x4xxx – Módulo de Jobs
    // ----------------------------------------------------------------------
};