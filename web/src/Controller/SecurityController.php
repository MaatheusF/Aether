<?php

namespace App\Controller;

use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Attribute\Route;
use Symfony\Component\Security\Http\Authentication\AuthenticationUtils;

class SecurityController extends AbstractController
{
    #[Route('/login', name: 'app_login')]
    #[Route('/')]
    public function login(AuthenticationUtils $authenticationUtils): Response
    {
        // Redireciona para home se o usuario ja estiver logado
        if ($this->getUser()) {
            return $this->redirectToRoute('app_dashboard');
        }

        // Busca o nome de usuario utilizado no ultimo login
        $lastUsername = $authenticationUtils->getLastUsername();
        // Busca a mensagem de erro gerada durante o processo de login
        $error = $authenticationUtils->getLastAuthenticationError();

        return $this->render('security/login.html.twig', [
            'last_username' => $lastUsername,
            'error' => $error,
        ]);
    }

    #[Route('/logout', name: 'app_logout')]
    public function logout(): void
    {
        // Interceptado pelo firewall do Symfony Security (logout: em security.yaml).
        throw new \LogicException('Este método pode ficar vazio - será interceptado pela chave de logout configurada em security.yaml.');
    }
}
