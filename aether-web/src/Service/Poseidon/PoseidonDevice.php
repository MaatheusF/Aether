<?php

namespace App\Service\Poseidon;

/**
 * Representa um dispositivo (ESP32) do módulo Poseidon do ponto de vista da WEB.
 *
 * Hoje é montado com dados mock por PoseidonDeviceProvider — quando o Core
 * expuser um endpoint de listagem de dispositivos, esta classe passa a ser
 * preenchida a partir da resposta HTTP em vez de dados fixos, sem precisar
 * mudar nenhum template que já consome PoseidonDevice.
 */
final readonly class PoseidonDevice
{
    /**
     * @param string $slug       Identificador usado na URL (/modulos/poseidon/{slug})
     *                           e que deve corresponder ao device_name usado no
     *                           handshake TCP do ESP32 com o Core.
     * @param string $nome       Nome amigável exibido na UI (ex: "Aquário da Sala").
     * @param string $tipo       "aquario" ou "paludario" — usado só para o rótulo/ícone.
     * @param string $status     "online" | "offline" | "alert" | "pending"
     * @param array<string,mixed> $telemetria Últimas leituras conhecidas (mock por enquanto).
     */
    public function __construct(
        public string $slug,
        public string $nome,
        public string $tipo,
        public string $status,
        public array $telemetria = [],
    ) {
    }
}
