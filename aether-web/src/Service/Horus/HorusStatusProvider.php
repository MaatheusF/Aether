<?php

namespace App\Service\Horus;

/**
 * Fonte única do status do módulo Horus para a WEB.
 *
 * IMPORTANTE: dado mock (renderização básica, como o resto do projeto
 * nesta fase). Quando o Core expuser um endpoint HTTP de status do Horus,
 * troque o corpo de obter() por uma chamada HTTP real — DashboardController
 * e ModuloController não precisam mudar, os dois já dependem só desta classe.
 */
final class HorusStatusProvider
{
    public function obter(): HorusStatus
    {
        // TODO: substituir por chamada HTTP ao Core assim que o endpoint existir.
        return new HorusStatus(
            status: 'alert',
            camerasAtivas: 0,
            camerasTotal: 0,
            dispositivosAtivos: 1,
            dispositivosTotal: 1,
            portaoEstado: 'destravado',
        );
    }
}
