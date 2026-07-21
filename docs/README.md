# Documentação — Aether

> Este índice reflete os arquivos que existem no repositório.
> Seções com apenas 1-2 documentos ainda estão em construção — os demais
> arquivos planejados serão adicionados conforme o projeto avança.

```
docs/
│
├── README.md                        # Este índice
│
├── core/
│   └── BuildAndRun.md                # Setup do ambiente (CLion + servidor remoto) e build do Aether Core
│
├── api/
│   ├── deploy/                       # Documentação da API HTTP do Aether Core
│   │   ├── README.md                 # Índice / ponto de entrada da doc da API
│   │   ├── QUICKSTART.md             # Subir a API em 1 minuto
│   │   ├── API_USAGE.md              # Guia prático de uso
│   │   ├── API_ARCHITECTURE.md       # Arquitetura interna da API
│   │   ├── API_CODE_REFERENCE.md     # Referência de código
│   │   └── API_REST.md               # Referência dos endpoints REST
│   └── methods/
│       └── status.md                 # GET /api/status
│
└── web/
    ├── Deploy.md                     # Setup do ambiente de desenvolvimento da interface Web
    └── frontend/
        └── DesignSystem.md           # Design System (Eco-Tech / Biopunk) — referência de UI

database/                             # Scripts SQL (core + web) — ver database/README.md
```

## Pendências identificadas

- `docs/api/deploy/` concentra 8 arquivos sobre a mesma API — vale considerar
  consolidar em menos arquivos ou mover para `docs/api/` diretamente, já que
  o nível `deploy/` não corresponde ao conteúdo (é documentação de uso da
  API, não de deploy).
- Não há documentação própria dos módulos (`Poseidon`, `Horus`) nem do
  protocolo entre Core e Web — apenas menções indiretas dentro de outros
  documentos.
- Não há documentação de schema/ER do banco — ver `database/README.md` para
  a convenção dos scripts, mas falta um documento descrevendo o modelo de
  dados em si.