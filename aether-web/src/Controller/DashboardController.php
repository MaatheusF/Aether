<?php

namespace App\Controller;

use App\Service\Poseidon\PoseidonDeviceProvider;
use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;

final class DashboardController extends AbstractController
{
    public function __construct(
        private readonly PoseidonDeviceProvider $poseidonDevices,
    ) {
    }

    #[Route('/home', name: 'app_dashboard')]
    public function index(): Response
    {
        $dispositivos = $this->poseidonDevices->listar();

        return $this->render('/home/index.html.twig', [
            'dispositivos_poseidon' => $dispositivos,
            'resumo' => $this->montarResumo($dispositivos),
            // TODO: substituir por alertas reais vindos do Core assim que a
            // avaliação de limiar (temperatura/umidade fora do ideal) existir
            // do lado do Core — ver docs/api/methods (a definir). Por ora,
            // mock referenciando os próprios dispositivos cadastrados, pra já
            // deixar claro que um alerta pertence a UM dispositivo específico.
            'alertas' => $this->montarAlertasMock($dispositivos),
        ]);
    }

    /**
     * @param \App\Service\Poseidon\PoseidonDevice[] $dispositivos
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
     * @param \App\Service\Poseidon\PoseidonDevice[] $dispositivos
     * @return array<int, array{nivel: string, dispositivo: string, mensagem: string}>
     */
    private function montarAlertasMock(array $dispositivos): array
    {
        $nomes = array_map(static fn ($d) => $d->nome, $dispositivos);

        return [
            [
                'nivel' => 'estavel',
                'dispositivo' => $nomes[0] ?? 'Poseidon',
                'mensagem' => 'Fotoperíodo dentro do ciclo programado.',
            ],
            [
                'nivel' => 'atencao',
                'dispositivo' => $nomes[1] ?? 'Poseidon',
                'mensagem' => 'Nível da água 6% abaixo do ideal (evaporação).',
            ],
            [
                'nivel' => 'critico',
                'dispositivo' => $nomes[2] ?? 'Poseidon',
                'mensagem' => 'Sem resposta do Core há mais de 5 minutos.',
            ],
        ];
    }
}
