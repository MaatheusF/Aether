<?php

namespace App\Security;

use Symfony\Component\EventDispatcher\Attribute\AsEventListener;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Symfony\Component\Routing\Generator\UrlGeneratorInterface;
use Symfony\Component\Security\Http\Event\LogoutEvent;

/**
 * Substitui o redirect padrão de logout do Symfony, que monta uma URL
 * absoluta (Request::getUriForPath) a partir do Host da requisição —
 * nesse ambiente (Nginx em porta não-padrão) essa reconstrução perde a
 * porta (ver mesmo problema em LoginFormAuthenticator::onAuthenticationSuccess).
 * Um redirect relativo evita a reconstrução: o navegador resolve contra a
 * origem atual (porta incluída) sozinho.
 */
#[AsEventListener]
final class LogoutSuccessHandler
{
    public function __construct(
        private readonly UrlGeneratorInterface $urlGenerator,
    ) {
    }

    public function __invoke(LogoutEvent $event): void
    {
        $event->setResponse(new RedirectResponse($this->urlGenerator->generate('app_login')));
    }
}
