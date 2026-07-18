<?php

namespace App\Controller;

use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;

class ModuloController extends AbstractController
{
    #[Route('/modulos/poseidon', name: 'app_modulo_poseidon')]
    public function poseidon(): Response
    {
        // Renderização básica por enquanto — sliders, toggles e os dados
        // de telemetria/logs serão alimentados depois pela integração
        // com o módulo Poseidon do Core em C++.
        return $this->render('modulos/poseidon.html.twig');
    }
}
