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
        // Renderização básica por enquanto — os dados de telemetria e status
        // dos demais módulos (Horus) ainda são fixos no template; Poseidon já
        // vem de PoseidonDeviceProvider (hoje mock, futuramente API do Core).
        return $this->render('/home/index.html.twig', [
            'dispositivos_poseidon' => $this->poseidonDevices->listar(),
        ]);
    }
}
