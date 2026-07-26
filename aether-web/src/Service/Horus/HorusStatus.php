<?php

namespace App\Service\Horus;

/**
 * Representa o estado consolidado do módulo Horus.
 *
 * Ao contrário do Poseidon, o Horus não tem múltiplos dispositivos
 * independentes na WEB — câmeras, sensores e o portão são todos partes
 * do mesmo módulo/instalação. Por isso não existe um "HorusDevice" por
 * item; um único HorusStatus resume tudo que o dashboard/página do
 * módulo precisam mostrar.
 */
final readonly class HorusStatus
{
    /**
     * @param string $status         "online" | "offline" | "alert"
     * @param int    $camerasAtivas
     * @param int    $camerasTotal
     * @param int    $dispositivosAtivos
     * @param int    $dispositivosTotal
     * @param string $portaoEstado   "travado" | "destravado"
     */
    public function __construct(
        public string $status,
        public int $camerasAtivas,
        public int $camerasTotal,
        public int $dispositivosAtivos,
        public int $dispositivosTotal,
        public string $portaoEstado,
    ) {
    }
}
