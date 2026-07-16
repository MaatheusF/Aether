/**
 * Aether — Home / Painel
 * Comportamento leve: abrir/fechar o menu de configurações do sistema.
 */
(function () {
    'use strict';

    function initDropdowns() {
        // Use event delegation so dynamically placed toggles/menus are handled too.
        function getMenuById(id) { return id ? document.getElementById(id) : null; }

        function closeAll(exceptMenu) {
            var openMenus = Array.prototype.slice.call(document.querySelectorAll('.dropdown-menu.open, .settings-menu.open'));
            openMenus.forEach(function (m) {
                if (m === exceptMenu) return;
                m.classList.remove('open');
                var controller = document.querySelector('[aria-controls="' + m.id + '"]');
                if (controller) controller.setAttribute('aria-expanded', 'false');
            });
        }

        document.addEventListener('click', function (event) {
            var toggle = event.target.closest('[aria-haspopup="true"][aria-controls]');
            if (toggle) {
                // clicked on a toggle
                event.stopPropagation();
                var menuId = toggle.getAttribute('aria-controls');
                var menu = getMenuById(menuId);
                 if (!menu) return;
                 var isOpen = menu.classList.contains('open');
                 if (isOpen) {
                    menu.classList.remove('open');
                    toggle.setAttribute('aria-expanded', 'false');
                } else {
                    closeAll(menu);
                    menu.classList.add('open');
                    toggle.setAttribute('aria-expanded', 'true');
                }
                return;
            }

            // click not on toggle: if click is inside an open menu, do nothing
            var clickedInsideMenu = !!event.target.closest('.dropdown-menu.open, .settings-menu.open');
            if (!clickedInsideMenu) closeAll();
        });

        // Escape closes all
        document.addEventListener('keydown', function (event) {
            if (event.key === 'Escape') closeAll();
        });
    }

    // Run initDropdowns immediately if DOM is already ready (script may be loaded at end of body)
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initDropdowns);
    } else {
        initDropdowns();
    }
})();
