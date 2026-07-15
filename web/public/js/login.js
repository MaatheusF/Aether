/**
 * Aether — Tela de Login
 * Gera bolhas subindo continuamente pelo fundo do paludário.
 */
(function () {
    'use strict';

    function initBubbles() {
        var container = document.getElementById('bubbles');
        if (!container) return;

        var total = 8;
        for (var i = 0; i < total; i++) {
            var bubble = document.createElement('div');
            bubble.className = 'bubble';

            var size = 3 + Math.random() * 9;
            bubble.style.width = size + 'px';
            bubble.style.height = size + 'px';
            bubble.style.left = (Math.random() * 100) + '%';

            var duration = 9 + Math.random() * 12;
            bubble.style.animationDuration = duration + 's';
            bubble.style.animationDelay = (-Math.random() * duration) + 's';

            container.appendChild(bubble);
        }
    }

    document.addEventListener('DOMContentLoaded', initBubbles);
})();
