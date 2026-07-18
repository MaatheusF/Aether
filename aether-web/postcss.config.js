// postcss.config.js
// Pipeline do Tailwind v4 via PostCSS puro (sem Vite/Webpack).
// @tailwindcss/postcss já inclui o motor Lightning CSS por baixo dos panos.

export default {
    plugins: {
        '@tailwindcss/postcss': {},
        // Minificação só no build de produção (NODE_ENV=production).
        // Ver script "css:build:prod" no package.json.
        ...(process.env.NODE_ENV === 'production' ? { cssnano: { preset: 'default' } } : {}),
    },
};
