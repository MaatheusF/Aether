/**
 * Aether — Home / Painel
 * Comportamento leve: abrir/fechar o menu de configurações do sistema.
 */
(function () {
    'use strict';

    function initSettingsMenu() {
        var toggle = document.getElementById('settings-toggle');
        var menu = document.getElementById('settings-menu');
        if (!toggle || !menu) return;

        function closeMenu() {
            menu.classList.remove('open');
            toggle.setAttribute('aria-expanded', 'false');
        }

        function openMenu() {
            menu.classList.add('open');
            toggle.setAttribute('aria-expanded', 'true');
        }

        toggle.addEventListener('click', function (event) {
            event.stopPropagation();
            var isOpen = menu.classList.contains('open');
            isOpen ? closeMenu() : openMenu();
        });

        document.addEventListener('click', function (event) {
            if (!menu.contains(event.target)) closeMenu();
        });

        document.addEventListener('keydown', function (event) {
            if (event.key === 'Escape') closeMenu();
        });
    }

    document.addEventListener('DOMContentLoaded', initSettingsMenu);
})();
