<?php

namespace App\Controller;

use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;

final class DashboardController extends AbstractController
{
    #[Route('/home', name: 'app_dashboard')]
    public function index(): Response
    {
        // Renderização básica por enquanto — os dados de telemetria e status
        // dos módulos (Poseidon, Horus) serão injetados aqui depois,
        // vindos da integração com o Core em C++.
        return $this->render('/home/index.html.twig');
    }
}
