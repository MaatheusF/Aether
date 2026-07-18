// --- Dropdowns genéricos (menu do usuário, e qualquer outro futuro) ---
// Basta marcar o wrapper com [data-dropdown], o botão com [data-dropdown-trigger]
// e o painel com [data-dropdown-panel]. Fecha ao clicar fora, ao pressionar
// Esc, ou ao clicar em outro dropdown-trigger.

(function initDropdowns() {
    const dropdowns = document.querySelectorAll('[data-dropdown]');

    function closeDropdown(root) {
        const trigger = root.querySelector('[data-dropdown-trigger]');
        const panel = root.querySelector('[data-dropdown-panel]');
        panel?.classList.add('hidden');
        trigger?.setAttribute('aria-expanded', 'false');
    }

    function openDropdown(root) {
        const trigger = root.querySelector('[data-dropdown-trigger]');
        const panel = root.querySelector('[data-dropdown-panel]');
        panel?.classList.remove('hidden');
        trigger?.setAttribute('aria-expanded', 'true');
    }

    function closeAll(except) {
        dropdowns.forEach((root) => { if (root !== except) closeDropdown(root); });
    }

    dropdowns.forEach((root) => {
        const trigger = root.querySelector('[data-dropdown-trigger]');
        trigger?.addEventListener('click', function (event) {
            event.stopPropagation();
            const isOpen = trigger.getAttribute('aria-expanded') === 'true';
            closeAll(root);
            isOpen ? closeDropdown(root) : openDropdown(root);
        });
    });

    document.addEventListener('click', () => closeAll());
    document.addEventListener('keydown', (event) => {
        if (event.key === 'Escape') closeAll();
    });
})();
