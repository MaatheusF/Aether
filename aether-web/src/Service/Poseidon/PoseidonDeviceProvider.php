<?php

namespace App\Service\Poseidon;

/**
 * Fonte única da lista de dispositivos do módulo Poseidon para a WEB.
 *
 * IMPORTANTE: os dados aqui são mock (renderização básica, como o resto do
 * projeto nesta fase). Quando o Core expuser um endpoint HTTP de listagem
 * de dispositivos (ex: GET /api/modulos/poseidon/dispositivos), troque o
 * corpo de listar() por uma chamada HTTP real — DashboardController e
 * ModuloController não precisam mudar, os dois já dependem só desta classe.
 *
 * Ter uma única fonte evita o que aconteceria se cada controller montasse
 * sua própria lista: dashboard e página do módulo mostrando dispositivos
 * diferentes por terem sido editados em momentos diferentes.
 */
final class PoseidonDeviceProvider
{
    /**
     * @return PoseidonDevice[]
     */
    public function listar(): array
    {
        // TODO: substituir por chamada HTTP ao Core assim que o endpoint existir.
        // O 'slug' de cada device abaixo deve corresponder ao device_name usado
        // no handshake TCP (ver core/network/session/ConnSession.hpp).
        return [
            new PoseidonDevice(
                slug: 'aquario-sala',
                nome: 'Aquário da Sala',
                tipo: 'aquario',
                status: 'online',
                telemetria: ['temp_agua' => '25.4°C', 'umidade' => '78%', 'luz' => '6500K'],
            ),
            new PoseidonDevice(
                slug: 'paludario-escritorio',
                nome: 'Paludário do Escritório',
                tipo: 'paludario',
                status: 'online',
                telemetria: ['temp_agua' => '24.8°C', 'umidade' => '82%', 'luz' => '6500K'],
            ),
            new PoseidonDevice(
                slug: 'aquario-varanda',
                nome: 'Aquário da Varanda',
                tipo: 'aquario',
                status: 'offline',
                telemetria: ['temp_agua' => '--', 'umidade' => '--', 'luz' => '--'],
            ),
        ];
    }

    /**
     * Busca um dispositivo pelo slug usado na URL.
     */
    public function buscarPorSlug(string $slug): ?PoseidonDevice
    {
        foreach ($this->listar() as $dispositivo) {
            if ($dispositivo->slug === $slug) {
                return $dispositivo;
            }
        }

        return null;
    }
}
