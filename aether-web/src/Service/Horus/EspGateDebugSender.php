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
    private const float TIMEOUT_SECONDS = 10.0;

    public function abrir(): void
    {
        // pfsockopen mantém o socket aberto entre requisições dentro do
        // MESMO worker PHP-FPM (não fecha ao final do request como
        // stream_socket_client). Não é uma conexão única global — cada
        // worker tem a sua — mas evita reconectar a cada clique.
        $conexao = @pfsockopen(
            sprintf('tcp://%s', self::HOST),
            self::PORT,
            $codigoErro,
            $mensagemErro,
            self::TIMEOUT_SECONDS,
        );

        if ($conexao === false) {
            throw new RuntimeException(sprintf('Falha ao conectar no ESP32 (%s): %s', self::HOST, $mensagemErro));
        }

        // Conexão persistente pode ter caído do lado do ESP32 (idle
        // timeout, reboot) sem o PHP notar até tentar escrever. Se a
        // escrita falhar, descarta o socket morto e tenta reconectar
        // uma vez antes de desistir.
        if (@fwrite($conexao, '1') === false) {
            fclose($conexao);

            $conexao = @pfsockopen(
                sprintf('tcp://%s', self::HOST),
                self::PORT,
                $codigoErro,
                $mensagemErro,
                self::TIMEOUT_SECONDS,
            );

            if ($conexao === false) {
                throw new RuntimeException(sprintf('Falha ao reconectar no ESP32 (%s): %s', self::HOST, $mensagemErro));
            }

            if (@fwrite($conexao, '1') === false) {
                throw new RuntimeException('Falha ao enviar comando ao ESP32 mesmo após reconectar.');
            }
        }
    }
}
