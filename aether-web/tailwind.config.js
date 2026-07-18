/**
 * ============================================================================
 * AETHER — tailwind.config.js
 * Registra a paleta Eco-Tech/Biopunk e as fontes usadas nos templates Twig,
 * além dos paths de conteúdo para o Tailwind escanear as classes usadas em
 * templates/ (Twig) e assets/ (JS/TS que manipulam classes dinamicamente).
 * ============================================================================
 */

/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    // Todos os templates Twig do Symfony, incluindo subpastas (ex: modulos/)
    './templates/**/*.html.twig',

    // Assets JS/TS — necessário caso alguma classe Tailwind seja montada
    // dinamicamente em string dentro do JS (ex: toggle de estados, bio-dimming)
    './assets/**/*.{js,ts,jsx,tsx}',

    // Caso use componentes/partials fora de templates/, adicione aqui:
    // './src/**/*.php',
  ],

  theme: {
    extend: {
      colors: {
        // ---- Base do sistema (fundo musgo profundo / superfícies) ----
        'aether-bg':      '#0E1310', // fundo principal
        'aether-surface':  '#141C18', // superfície dos cards (usar com /60 para o efeito glass)
        'aether-border':  '#22332A', // bordas finas

        // ---- Cores de destaque (iluminação de calhas de LED) ----
        'accent-green':   '#4EEE94', // verde broto
        'accent-blue':    '#00F5FF', // azul água fluorescente
        'accent-amber':   '#FFB900', // âmbar quente
      },

      fontFamily: {
        // Fonte principal da UI (limpa e orgânica)
        sans: ['"Plus Jakarta Sans"', 'ui-sans-serif', 'sans-serif'],

        // Fontes para dados numéricos e logs técnicos
        mono: ['"JetBrains Mono"', '"Share Tech Mono"', 'ui-monospace', 'monospace'],
      },

      // Suporte ao pulso do indicador de status vital do Core
      // (a keyframe "core-pulse" em si está definida via <style> no base.html.twig;
      // caso prefira centralizar aqui, descomente e remova do template):
      // keyframes: {
      //   'core-pulse': {
      //     '0%, 100%': { opacity: 1, boxShadow: '0 0 0 0 rgba(78, 238, 148, 0.5)' },
      //     '50%': { opacity: 0.6, boxShadow: '0 0 0 6px rgba(78, 238, 148, 0)' },
      //   },
      // },
      // animation: {
      //   'core-pulse': 'core-pulse 2.4s ease-in-out infinite',
      // },
    },
  },

  // TODO: adicionar plugins conforme a necessidade (ex: @tailwindcss/forms
  // para estilizar os inputs nativos além do padrão "underline" do login)
  plugins: [],
};
