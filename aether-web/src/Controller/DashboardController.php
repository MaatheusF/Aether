<?php

namespace App\Controller;

use App\Service\Horus\HorusStatusProvider;
use App\Service\Poseidon\PoseidonDevice;
use App\Service\Poseidon\PoseidonDeviceProvider;
use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;

final class DashboardController extends AbstractController
{
    /**
     * Ordem de severidade dos dispositivos Poseidon na grade da /home —
     * quem precisa de atenção sobe pro topo. 'pending' (nunca conectou)
     * fica antes de 'online' mas depois de 'offline' (que já esteve
     * funcionando e parou de responder): é um estado esperado durante
     * o setup de um dispositivo novo, não uma falha.
     *
     * @var array<string, int>
     */
    private const ORDEM_SEVERIDADE_DISPOSITIVO = [
        'alert' => 0,
        'offline' => 1,
        'pending' => 2,
        'online' => 3,
    ];

    /**
     * Mesma ideia para os alertas: crítico sempre antes de atenção.
     *
     * @var array<string, int>
     */
    private const ORDEM_SEVERIDADE_ALERTA = [
        'critico' => 0,
        'atencao' => 1,
    ];

    public function __construct(
        private readonly PoseidonDeviceProvider $poseidonDevices,
        private readonly HorusStatusProvider $horusStatus,
    ) {
    }

    #[Route('/home', name: 'app_dashboard')]
    public function index(): Response
    {
        $dispositivos = $this->poseidonDevices->listar();

        // 'estavel' não é um alerta (nada a fazer) — não entra na linha de
        // alertas nem no contador do resumo, que conta direto de 'alertas'
        // no template via alertas|length. O filtro fica aqui, e não só no
        // mock, porque continua valendo quando os alertas reais do Core
        // chegarem (ver DashboardController::montarAlertasMock).
        $alertas = array_values(array_filter(
            $this->montarAlertasMock($dispositivos),
            static fn (array $a): bool => $a['nivel'] !== 'estavel',
        ));
        $alertas = $this->ordenarArrayPorSeveridade($alertas, 'nivel', self::ORDEM_SEVERIDADE_ALERTA);

        return $this->render('/home/index.html.twig', [
            'dispositivos_poseidon' => $this->ordenarDispositivosPorSeveridade($dispositivos),
            'horus' => $this->horusStatus->obter(),
            'resumo' => $this->montarResumo($dispositivos),
            'alertas' => $alertas,
        ]);
    }

    /**
     * @param PoseidonDevice[] $dispositivos
     * @return array{dispositivos_total: int, dispositivos_online: int}
     */
    private function montarResumo(array $dispositivos): array
    {
        return [
            'dispositivos_total' => count($dispositivos),
            'dispositivos_online' => count(array_filter(
                $dispositivos,
                static fn ($d) => $d->status === 'online',
            )),
        ];
    }

    /**
     * @param PoseidonDevice[] $dispositivos
     * @return PoseidonDevice[]
     */
    private function ordenarDispositivosPorSeveridade(array $dispositivos): array
    {
        $ordenados = $dispositivos;
        usort(
            $ordenados,
            fn (PoseidonDevice $a, PoseidonDevice $b): int => $this->rank($a->status, self::ORDEM_SEVERIDADE_DISPOSITIVO)
                <=> $this->rank($b->status, self::ORDEM_SEVERIDADE_DISPOSITIVO),
        );

        return $ordenados;
    }

    /**
     * @param array<int, array<string, mixed>> $itens
     * @param array<string, int>               $ordem
     * @return array<int, array<string, mixed>>
     */
    private function ordenarArrayPorSeveridade(array $itens, string $chave, array $ordem): array
    {
        usort(
            $itens,
            fn (array $a, array $b): int => $this->rank($a[$chave], $ordem) <=> $this->rank($b[$chave], $ordem),
        );

        return $itens;
    }

    /**
     * Valor fora do mapa de severidade (não deveria acontecer, mas não
     * pode quebrar a página se acontecer) vai pro fim da lista.
     *
     * @param array<string, int> $ordem
     */
    private function rank(string $valor, array $ordem): int
    {
        return $ordem[$valor] ?? PHP_INT_MAX;
    }

    /**
     * @param PoseidonDevice[] $dispositivos
     * @return array<int, array{nivel: string, dispositivo: string, mensagem: string}>
     */
    private function montarAlertasMock(array $dispositivos): array
    {
        $nomes = array_map(static fn ($d) => $d->nome, $dispositivos);

        return [
            [
                'nivel' => 'atencao',
                'dispositivo' => $nomes[0] ?? 'Poseidon',
                'mensagem' => 'Nível da água 6% abaixo do ideal (evaporação).',
            ],
            [
                'nivel' => 'critico',
                'dispositivo' => $nomes[1] ?? 'Poseidon',
                'mensagem' => 'Sem resposta do Core há mais de 5 minutos.',
            ],
        ];
    }
}
