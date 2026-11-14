/* Aether — alternância de tema claro/escuro.
   Carregue no <head>, SEM defer: a parte de aplicar o tema salvo roda
   imediatamente (antes da primeira pintura, sem "piscar" no tema errado);
   só o clique no botão #theme espera o DOM ficar pronto.
   Ordem de decisão: escolha salva > preferência do sistema > claro. */
(function () {
  var root = document.documentElement;
  var STORAGE_KEY = 'aether-theme';

  var saved = null;
  try { saved = window.localStorage ? window.localStorage.getItem(STORAGE_KEY) : null; } catch (e) { saved = null; }

  if (saved === 'light' || saved === 'dark') {
    root.setAttribute('data-theme', saved);
  } else if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
    root.setAttribute('data-theme', 'dark');
  } else {
    root.setAttribute('data-theme', 'light');
  }

  document.addEventListener('DOMContentLoaded', function () {
    var btn = document.getElementById('theme');
    if (!btn) return;
    btn.addEventListener('click', function () {
      var next = root.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
      root.setAttribute('data-theme', next);
      try { if (window.localStorage) window.localStorage.setItem(STORAGE_KEY, next); } catch (e) { /* ignora */ }
    });
  });
})();
