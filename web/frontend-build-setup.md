# Setup do Front-end — Tailwind v4 via PostCSS

> **Substitui:** o Tailwind Play CDN (`cdn.tailwindcss.com`, v3) usado nos rascunhos iniciais.
> **Não usa:** Vite nem Webpack Encore — só PostCSS puro, chamado via `postcss-cli`.

---

## 1. Estrutura de arquivos

```
aether/
├── assets/
│   └── styles/
│       └── app.css          ← fonte única do Tailwind (config via @theme)
├── public/
│   └── build/
│       └── app.css          ← gerado pelo build, não editar à mão
├── templates/
│   ├── base.html.twig       ← <link href="{{ asset('build/app.css') }}">
│   └── login.html.twig      ← idem
├── postcss.config.js
└── package.json
```

## 2. Instalação (uma vez)

```bash
npm install
```

Isso instala `tailwindcss@4`, `@tailwindcss/postcss` (o plugin oficial que roda a v4 dentro do PostCSS), `postcss`, `postcss-cli` e `cssnano` (minificação só no build de produção).

## 3. Comandos do dia a dia

| Comando | Quando usar |
|---|---|
| `npm run css:watch` | durante o desenvolvimento — recompila `public/build/app.css` a cada salvamento em `assets/styles/app.css` ou em qualquer template escaneado |
| `npm run css:build` | build único, sem watch, sem minificação (útil pra conferir o CSS gerado) |
| `npm run css:build:prod` | build de produção, minificado via `cssnano` |

> Rode `npm run css:watch` numa aba de terminal separada enquanto trabalha nos `.html.twig` — a v4 detecta as classes usadas automaticamente (não precisa configurar `content: [...]` como na v3).

## 4. Onde a paleta do projeto vive agora

Antes, a paleta (`aether-broto`, `aether-agua`, etc.) estava duplicada em dois blocos `<script>tailwind.config = {...}</script>`, um em `base.html.twig` e outro em `login.html.twig`. Agora ela vive **em um único lugar**: o bloco `@theme` no topo de `assets/styles/app.css`. Qualquer cor, fonte ou sombra nova do design system entra ali, nunca direto num template.

## 5. Integração com o Symfony

- `{{ asset('build/app.css') }}` funciona com o helper padrão do Twig (`asset()`), sem precisar de Webpack Encore nem AssetMapper — é só um arquivo estático dentro de `public/`.
- **`public/build/app.css` é gerado** — não deve ser editado manualmente nem versionado com edições diretas. Considere adicionar ao `.gitignore` se preferir gerá-lo só no deploy (nesse caso, o passo `npm run css:build:prod` precisa entrar no seu processo de deploy/CI).
- Fontes (Google Fonts) continuam carregadas via `<link>` direto no `<head>` dos templates — isso é independente da versão do Tailwind, não precisa de build.

## 6. Diferenças herdadas da migração v3 → v4 (o que já foi resolvido aqui)

| Cuidado da v4 | Como foi tratado no Aether |
|---|---|
| Cor padrão de borda muda de `gray-200` para `currentColor` | Todas as bordas do projeto já usam classes explícitas (`border-aether-border`, `border-aether-broto/40`, etc.), então esse breaking change não afeta os templates atuais |
| `tailwind.config.js` não é mais o padrão | Substituído por `@theme` em `assets/styles/app.css` |
| v4 exige navegadores mais recentes (Safari 16.4+, Chrome 111+, Firefox 128+) | Aceitável para um painel doméstico de uso pessoal — reavaliar se algum dia precisar suportar navegador antigo |

## 7. TODO / decisões em aberto

- [ ] Decidir se `public/build/app.css` entra no controle de versão ou é gerado só em CI/deploy.
- [ ] Se o projeto crescer muito, avaliar separar `app.css` em múltiplos arquivos de origem (ex: um por módulo) importados via `@import` dentro do próprio `assets/styles/app.css`.
