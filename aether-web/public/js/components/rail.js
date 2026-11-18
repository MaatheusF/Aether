/* Aether — abre/fecha o rail como gaveta no mobile (<960px). */
(function () {
  document.addEventListener('DOMContentLoaded', function () {
    var rail = document.getElementById('rail');
    var overlay = document.getElementById('rail-overlay');
    var toggle = document.getElementById('rail-toggle');
    if (!rail || !toggle) return;

    function open() { rail.classList.add('open'); if (overlay) overlay.classList.add('show'); }
    function close() { rail.classList.remove('open'); if (overlay) overlay.classList.remove('show'); }

    toggle.addEventListener('click', function () {
      rail.classList.contains('open') ? close() : open();
    });
    if (overlay) overlay.addEventListener('click', close);
  });
})();
