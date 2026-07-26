<?php

namespace App\Service\Horus;

use RuntimeException;

/**
 * Envia o comando de abertura pro servidor de debug do ESP32 do portão.
 *
 * TEMPORÁRIO: uso exclusivo para testes com o firmware de debug
 * (192.168.0.138:8888), que espera receber o byte "1" via TCP puro
 * (sem HTTP). Quando o Core assumir esse fluxo (ver GateController
 * no aether-core), este service e a rota que o usa devem ser removidos.
 */
final class EspGateDebugSender
{
    private const string HOST = '192.168.0.138';
    private const int PORT = 8888;
    private const float TIMEOUT_SECONDS = 3.0;

    public function abrir(): void
    {
        // Conexão nova a cada chamada (não persistente): com o WiFi até o
        // portão instável, uma conexão reaproveitada (pfsockopen) pode
        // cair sem o PHP notar no momento do fwrite() — o TCP só detecta
        // isso na escrita SEGUINTE, fazendo o comando atual "desaparecer"
        // silenciosamente. Reconectar sempre custa uns milissegundos, mas
        // garante que cada clique valida a conexão do zero.
        $conexao = @stream_socket_client(
            sprintf('tcp://%s:%d', self::HOST, self::PORT),
            $codigoErro,
            $mensagemErro,
            self::TIMEOUT_SECONDS,
        );

        if ($conexao === false) {
            throw new RuntimeException(sprintf('Falha ao conectar no ESP32 (%s): %s', self::HOST, $mensagemErro));
        }

        try {
            if (@fwrite($conexao, '1') === false) {
                throw new RuntimeException('Falha ao enviar comando ao ESP32.');
            }
        } finally {
            fclose($conexao);
        }
    }
}
