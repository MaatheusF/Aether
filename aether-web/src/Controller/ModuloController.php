<?php

namespace App\Controller;

use App\Service\Horus\HorusStatusProvider;
use App\Service\Poseidon\PoseidonDeviceProvider;
use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpKernel\Exception\NotFoundHttpException;
use Symfony\Component\Routing\Attribute\Route;

class ModuloController extends AbstractController
{
    public function __construct(
        private readonly PoseidonDeviceProvider $poseidonDevices,
        private readonly HorusStatusProvider $horusStatus,
    ) {
    }

    /**
     * /modulos/poseidon — lista todos os dispositivos do módulo.
     *
     * Antes redirecionava pro primeiro dispositivo conhecido (não existia
     * uma "lista" de verdade, só o detalhe com um seletor lateral). Isso
     * mudou: clicar num card leva pro detalhe em
     * app_modulo_poseidon_dispositivo, com um link "← Poseidon" pra voltar
     * (ver templates/modulos/poseidon/lista.html.twig e dispositivo.html.twig).
     */
    #[Route('/modulos/poseidon', name: 'app_modulo_poseidon')]
    public function poseidonIndex(): Response
    {
        return $this->render('modulos/poseidon/lista.html.twig', [
            'dispositivos' => $this->poseidonDevices->listar(),
        ]);
    }

    /**
     * /modulos/poseidon/{slug} — detalhe de um dispositivo (ESP32) específico.
     */
    #[Route('/modulos/poseidon/{slug}', name: 'app_modulo_poseidon_dispositivo')]
    public function poseidonDispositivo(string $slug): Response
    {
        $dispositivo = $this->poseidonDevices->buscarPorSlug($slug);

        if ($dispositivo === null) {
            throw new NotFoundHttpException(sprintf('Dispositivo Poseidon "%s" não encontrado.', $slug));
        }

        return $this->render('modulos/poseidon/dispositivo.html.twig', [
            'dispositivo' => $dispositivo,
        ]);
    }

    /**
     * /modulos/horus — módulo de segurança (portão + câmeras).
     * Sem seletor de dispositivo: ao contrário do Poseidon, o Horus é uma
     * instância única por instalação (ver HorusStatus).
     */
    #[Route('/modulos/horus', name: 'app_modulo_horus')]
    public function horus(): Response
    {
        return $this->render('modulos/horus.html.twig', [
            'horus' => $this->horusStatus->obter(),
        ]);
    }
}
