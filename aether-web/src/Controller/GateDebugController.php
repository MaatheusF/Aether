<?php

namespace App\Controller;

use App\Service\Horus\EspGateDebugSender;
use Psr\Log\LoggerInterface;
use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\Routing\Attribute\Route;
use Throwable;

/**
 * TEMPORÁRIO: rota de debug pro portão do Horus, ver EspGateDebugSender.
 */
class GateDebugController extends AbstractController
{
    public function __construct(
        private readonly EspGateDebugSender $espGateDebugSender,
        private readonly LoggerInterface $logger,
    ) {
    }

    #[Route('/modulos/horus/portao/debug-abrir', name: 'app_horus_portao_debug_abrir', methods: ['POST'])]
    public function abrir(): JsonResponse
    {
        try {
            $this->espGateDebugSender->abrir();

            return $this->json(['success' => true]);
        } catch (Throwable $e) {
            $this->logger->error('Falha ao enviar comando de debug ao portao: {mensagem}', ['mensagem' => $e->getMessage()]);

            return $this->json(['success' => false, 'message' => $e->getMessage()], 502);
        }
    }
}
