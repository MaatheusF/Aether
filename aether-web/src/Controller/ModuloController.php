<?php

namespace App\Controller;

use App\Service\Poseidon\PoseidonDeviceProvider;
use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpKernel\Exception\NotFoundHttpException;
use Symfony\Component\Routing\Attribute\Route;

class ModuloController extends AbstractController
{
    public function __construct(
        private readonly PoseidonDeviceProvider $poseidonDevices,
    ) {
    }

    /**
     * /modulos/poseidon — sem dispositivo especificado.
     * Redireciona pro primeiro dispositivo conhecido, pra sempre existir
     * uma URL válida e "compartilhável" mesmo quando o link não vem com slug
     * (ex: clique vindo do menu superior, que não sabe qual dispositivo
     * o usuário estava vendo por último).
     */
    #[Route('/modulos/poseidon', name: 'app_modulo_poseidon')]
    public function poseidonIndex(): Response
    {
        $dispositivos = $this->poseidonDevices->listar();

        if (empty($dispositivos)) {
            // Nenhum dispositivo cadastrado ainda — renderiza a própria
            // página do módulo sem dispositivo selecionado (ela já sabe
            // lidar com esse estado, ver bloco 'sem_dispositivo' no template).
            return $this->render('modulos/poseidon.html.twig', [
                'dispositivos' => [],
                'dispositivo' => null,
            ]);
        }

        return $this->redirectToRoute('app_modulo_poseidon_dispositivo', [
            'slug' => $dispositivos[0]->slug,
        ]);
    }

    /**
     * /modulos/poseidon/{slug} — página completa do módulo, escopada a um
     * dispositivo (ESP32) específico.
     */
    #[Route('/modulos/poseidon/{slug}', name: 'app_modulo_poseidon_dispositivo')]
    public function poseidonDispositivo(string $slug): Response
    {
        $dispositivo = $this->poseidonDevices->buscarPorSlug($slug);

        if ($dispositivo === null) {
            throw new NotFoundHttpException(sprintf('Dispositivo Poseidon "%s" não encontrado.', $slug));
        }

        return $this->render('modulos/poseidon.html.twig', [
            'dispositivos' => $this->poseidonDevices->listar(),
            'dispositivo' => $dispositivo,
        ]);
    }
}
