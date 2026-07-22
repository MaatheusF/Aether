# Design System — Aether

> https://markdownlivepreview.dev/

> **Propósito deste arquivo:** referência única e auto-suficiente do visual do Aether. Ao colocar este arquivo no início de uma conversa de IA, o objetivo é que dê pra gerar um novo componente/tela consistente com o que já existe, sem precisar reexplicar nada. Alem disso aqui está descrito a folha de estilo para manter o padrão de layout.

> **Stack:** Symfony (Twig) + Tailwind CSS v4 (PostCSS puro, sem Vite/Encore) — ver `aether-web/frontend-build-setup.md` para o pipeline de build.

> **Estética:** Eco-Tech / Biopunk — um painel de controle doméstico que parece um paludário digital: escuro, orgânico, com iluminação de LED de cultivo em vez de alarmes industriais.

---

## 1. Princípios de design

1. **Biológico, não industrial.** Onde um painel de fábrica usaria vermelho/amarelo/verde semáforo, o Aether usa estados que lembram um ecossistema vivo: *Estável* (folha), *Atenção* (seco), *Crítico* (toxina). Ver §5.6.
2. **Vidro sobre musgo.** Toda superfície elevada (card, topbar, sidebar, dropdown) é glassmorphism — nunca uma cor sólida opaca.
3. **Dados sempre em mono.** Qualquer valor numérico/técnico (temperatura, %, logs, timestamps) usa `font-mono`. Texto de interface usa `font-sans`. Essa troca de fonte é o que sinaliza "isto é um dado do hardware" vs "isto é rótulo de UI".
4. **Hardware é assíncrono, e a UI admite isso.** Nenhum toggle finge que o comando foi instantâneo — existe sempre um estado visual "Pendente" entre o clique e a confirmação física (ver §5.5).
5. **Mobile não é o desktop encolhido.** A navegação principal muda de lugar (topo → rodapé) e não só de tamanho — ver §3.

---

## 2. Tokens (`@theme` em `assets/styles/app.css`)

### 2.1 Cores

| Token Tailwind | Hex | Uso |
|---|---|---|
| `aether-bg` | `#0E1310` | Fundo principal (verde musgo profundo) |
| `aether-bg-alt` | `#0D1117` | Alternativa de fundo (reservada, pouco usada ainda) |
| `aether-surface` | `#141C18` | Base de cor das superfícies glass (sempre com alpha, ver §2.3) |
| `aether-border` | `#22332A` | Toda borda sutil — cards, inputs, divisores |
| `aether-broto` | `#4EEE94` | Verde de destaque — positivo/online/estável/foco |
| `aether-agua` | `#00F5FF` | Azul de destaque — dados de água/temperatura, links secundários |
| `aether-ambar` | `#FFB900` | Âmbar de destaque — atenção/pendente/luz |
| `aether-critico` | `#FF5C5C` | Vermelho — crítico/erro/sair |

**Regra de uso:** essas 4 cores de destaque (`broto`/`agua`/`ambar`/`critico`) têm **significado semântico fixo**, não são intercambiáveis por preferência estética:
- `broto` = tudo certo, ativo, confirmado
- `agua` = neutro/informativo, específico de dados aquáticos
- `ambar` = aguardando, atenção, luz/energia
- `critico` = falha, ação destrutiva (logout, deletar)

### 2.2 Tipografia

| Token | Valor | Uso |
|---|---|---|
| `font-sans` | `"Plus Jakarta Sans", sans-serif` | Todo texto de interface (padrão do `<body>`) |
| `font-mono` | `"JetBrains Mono", "Share Tech Mono", monospace` | Dados numéricos, logs, badges de status, timestamps |

Carregadas via Google Fonts `<link>` direto no `<head>` de cada template raiz (`base.html.twig`, `login.html.twig`) — não fazem parte do build CSS.

### 2.3 Sombras

| Token | Valor | Uso |
|---|---|---|
| `shadow-glow-broto` | `0 0 24px -6px rgba(78,238,148,.45)` | Hover de cards/botões em contexto "positivo" |
| `shadow-glow-agua` | `0 0 24px -6px rgba(0,245,255,.35)` | Hover de cards/botões em contexto "água/Poseidon" |

### 2.4 Raio de borda (escala usada, não tokenizada — aplicar direto)

| Classe | Uso |
|---|---|
| `rounded-full` | Pills, badges, botões de ícone, avatar, toggles |
| `rounded-xl` | Botões grandes, containers de input |
| `rounded-2xl` | Cards, seções ("Zona N"), painel de dropdown |

---

## 3. Layout e breakpoints

| Breakpoint Tailwind | Papel no Aether |
|---|---|
| (padrão, <768px) | Mobile: bottom nav fixa, sidebar vira drawer overlay, zonas empilham em 1 coluna |
| `md:` (≥768px) | Menu superior horizontal volta a aparecer, bottom nav some, grids passam a 2-3 colunas |
| `lg:` (≥1024px) | Sidebar esquerda vira fixa (não-drawer) quando a página é de módulo |

### 3.1 Estrutura de página (herdada de `base.html.twig`)

```
<header>          topbar fixa, h-16, z-40, glass-surface
<aside #sidebar>  só existe se layout_mode = 'sidebar'; w-64;
                   mobile = drawer (-translate-x-full + overlay),
                   lg: = fixa, abaixo da topbar
<main>            padding-top 4rem (topbar), padding-bottom 4rem no mobile
                   (bottom nav), lg:pl-64 se tiver sidebar
<nav bottom>      só no mobile (md:hidden), h-16, fixed bottom-0
```

Dois blocos Twig controlam tudo isso e **devem ser sobrescritos por qualquer novo template que estenda `base.html.twig`**:

```twig
{% block layout_mode %}full{% endblock %}     {# 'full' ou 'sidebar' #}
{% block module_active %}{% endblock %}        {# slug usado p/ destacar nav: 'poseidon', 'horus'... #}
```

Se `layout_mode` = `sidebar`, o template também deve preencher `{% block sidebar %}`.

### 3.2 Grids de conteúdo

Padrão usado em toda a listagem de cards (alertas, módulos):
```html
grid grid-cols-1 sm:grid-cols-2 xl:grid-cols-3 gap-4   /* dashboard: módulos */
grid grid-cols-1 md:grid-cols-3 gap-4                  /* dashboard: alertas */
grid grid-cols-1 lg:grid-cols-3 gap-4                  /* zonas internas de módulo */
```
Regra geral: **1 coluna até o breakpoint em que o conteúdo realmente cabe confortável** — nunca forçar 2-3 colunas apertadas no mobile.

### 3.3 Dois tipos de módulo: multi-dispositivo vs. instância única

Nem todo módulo tem a mesma forma. Antes de criar um módulo novo, decida qual dos dois ele é — isso muda rota, sidebar e o card do dashboard:

#### 3.3.1 Multi-dispositivo (padrão Poseidon)

Um módulo pode ter mais de um ESP32 acoplado — ex: vários aquários/paludários rodando o mesmo módulo.

- **Rota:** `/modulos/{modulo}` sem device vira redirect pro primeiro dispositivo conhecido; a página real vive em `/modulos/{modulo}/{slug}` (nome de rota `app_modulo_{modulo}_dispositivo`). Nunca renderizar a página do módulo sem um dispositivo resolvido — se não houver nenhum cadastrado, mostrar o estado vazio (ver `modulos/poseidon.html.twig`), não uma tela quebrada.
- **`slug` do dispositivo:** deve ser o mesmo valor usado como `device_name` no handshake TCP do Core (ver `docs/core/BuildAndRun.md`/`ConnSession`) — é o que amarra "o card que o usuário clicou" a "o ESP32 que o Core está falando".
- **Sidebar:** ganha um bloco de **Seletor de dispositivo** acima dos submenus — lista todos os dispositivos do módulo com um ponto de status (`bg-aether-broto` online / `bg-aether-critico` alerta / `bg-slate-600` offline) e destaca o selecionado com o mesmo padrão visual dos submenus ativos (`bg-{cor}/10 text-{cor} font-medium`). Os submenus abaixo (Visão Geral/Iluminação/etc.) continuam sendo por-módulo, mas exibidos no contexto do dispositivo selecionado.
- **Dashboard:** o card do módulo na grade principal deixa de ser único — vira um `{% for %}` sobre a lista de dispositivos, um card por device, todos linkando pra rota com o `slug` correspondente, dentro da seção do módulo (ver §3.6).
- **Fonte da lista de dispositivos:** nunca duplicar em mais de um controller. Um serviço dedicado (ex: `PoseidonDeviceProvider`) é a única fonte, injetado tanto no controller do dashboard quanto no do módulo — hoje pode retornar dados mock, depois passa a chamar o Core, sem os controllers/templates saberem a diferença.
- **Estado offline/alerta desabilita a Zona 1:** quando `dispositivo.status != 'online'`, mostrar um aviso (`⚠` + texto) acima das zonas e aplicar `opacity-40 pointer-events-none` na seção de Ações Rápidas — nunca deixar toggles/sliders parecendo funcionais para um dispositivo sem conexão confirmada.

#### 3.3.2 Instância única (padrão Horus)

Alguns módulos não têm dispositivos separados do ponto de vista da WEB — câmeras, sensores e atuadores fazem parte de uma única instalação (o Horus é assim: não existe "Horus 1" e "Horus 2", existe UM módulo de segurança com vários componentes dentro dele).

- **Rota:** uma só, `/modulos/{modulo}` (`app_modulo_{modulo}`), sem `{slug}`. Nada de redirect nem de seletor de dispositivo na sidebar.
- **Sidebar:** só os submenus (Visão Geral, e as demais telas específicas do módulo — no Horus: Portão, Câmeras, Configurações, Logs). Sem bloco de seletor.
- **Fonte do status:** em vez de um `{Modulo}DeviceProvider` retornando uma lista, um `{Modulo}StatusProvider` retornando **um objeto só** (ex: `HorusStatus`) com os campos que a UI precisa. Mesmo princípio do Poseidon — fonte única, injetada no dashboard e no controller do módulo, mock hoje e API do Core depois — só que sem a multiplicidade.
- **Dashboard:** o card desse módulo ocupa a **linha inteira** (não entra num grid de 2-3 colunas), porque não existe "mais um card do mesmo módulo" pra formar grade — e a largura extra permite mostrar vários indicadores lado a lado dentro do próprio card (ver §3.6 e o card do Horus em `home/index.html.twig`).
- **Decida isso ANTES de escrever qualquer rota.** Se não tiver certeza se um módulo futuro vai crescer pra multi-dispositivo, pergunte: "faz sentido o usuário ter vários desses ao mesmo tempo, cada um com seu próprio card?". Câmeras de segurança são plural mas pertencem a UMA instalação (Horus); aquários são plural E cada um é uma instalação independente (Poseidon) — é essa segunda característica que define multi-dispositivo, não só "ter mais de um item".

### 3.4 Bottom nav mobile: "Mais" em vez de lotar a barra

A bottom nav fica travada em no máximo **3 itens fixos** (Visão Geral + 1-2 destinos de uso diário) + um botão **"Mais"** (ícone de grade) que abre uma folha deslizando de baixo (`glass-surface`, `rounded-t-2xl`, overlay escuro atrás) listando os demais módulos. Todo módulo novo entra primeiro na folha "Mais" — só ganha um ícone fixo na barra se o uso justificar, e mesmo assim tirando espaço de outro item, nunca só adicionando mais um slot. Isso evita repetir o bug que motivou essa seção: um módulo existir no menu superior (desktop) e não ter nenhum caminho de acesso no mobile.

Padrão de implementação: mesma lógica do drawer da sidebar (`-translate-x-full` ↔ removido, overlay com `hidden` ↔ removido) — só que no eixo vertical (`translate-y-[120%]` ↔ removido) e ancorado acima da própria bottom nav (`bottom-16`), não por cima dela.

### 3.5 Faixa de resumo (dashboard)

Bloco curto (`glass-surface rounded-2xl p-4 flex flex-wrap gap-x-8`) no topo do dashboard, acima dos Cards de Alerta, com 2-4 números-chave em `font-mono` (ex: dispositivos totais, online, alertas ativos). Vem de um cálculo simples no controller (nunca em Twig com filtros de array — mantém a lógica testável em PHP puro), não é um componente decorativo — deve refletir dado real assim que existir fonte pra isso.

### 3.6 Dashboard: uma seção por módulo, nunca um grid único

A área "Módulos" do dashboard é **uma seção por módulo** (eyebrow com o nome do módulo, depois o conteúdo daquele módulo), não um grid único misturando cards de módulos diferentes — sem isso, não há nenhuma pista visual de qual card pertence a qual módulo.

O conteúdo de cada seção muda conforme o tipo do módulo (ver §3.3):
- **Multi-dispositivo (Poseidon):** eyebrow + contagem de dispositivos à direita, depois um grid `sm:grid-cols-2 xl:grid-cols-3` com `{% for %}` sobre a lista, e um `{% else %}` cobrindo "nenhum dispositivo cadastrado ainda" (nunca deixar a seção em branco silenciosamente).
- **Instância única (Horus):** eyebrow sem contagem (não faz sentido contar "1 de 1"), depois **um único card ocupando a linha inteira** (`class="group block rounded-2xl glass-surface p-5 ..."`, sem grid em volta) — a largura extra é usada pra mostrar os indicadores lado a lado dentro do próprio card (`grid grid-cols-1 sm:grid-cols-3 gap-4`), em vez de espremidos numa mini-telemetria de 3 colunas apertada.

**Sobre o controller:** não crie uma interface genérica de "provider de módulo" até existir de fato mais de ~3-4 módulos reais com essa necessidade — com poucos módulos, cada um injetando seu próprio provider (`PoseidonDeviceProvider`, `HorusStatusProvider`, ...) e passando sua própria variável é mais simples de entender e não é uma abstração precisando de manutenção. Se o `DashboardController` começar a ficar repetitivo, esse é o sinal concreto pra generalizar com uma interface comum + `#[AutoconfigureTag]` do Symfony — não antes.

---

## 4. Estrutura de arquivos

```
assets/styles/
├── app.css              ← fonte única: @theme + estilos globais/compartilhados
└── pages/
    └── base.css          ← estilos específicos de base.html.twig (padrão a repetir por template: pages/<nome>.css)

templates/
├── base.html.twig        ← esqueleto mestre (topbar, sidebar, bottom nav, dropdown genérico)
├── login.html.twig       ← tela isolada, não estende base
├── dashboard.html.twig   ← extends base, layout_mode = full
└── modulos/
    ├── poseidon.html.twig ← extends base, layout_mode = sidebar, multi-dispositivo (ver §3.3.1)
    └── horus.html.twig    ← extends base, layout_mode = sidebar, instância única (ver §3.3.2)

src/Controller/
├── SecurityController.php   ← app_login, app_logout
├── DashboardController.php  ← app_dashboard (injeta PoseidonDeviceProvider + HorusStatusProvider)
└── ModuloController.php     ← app_modulo_poseidon (redirect) + app_modulo_poseidon_dispositivo + app_modulo_horus

src/Service/Poseidon/
├── PoseidonDevice.php         ← value object (slug, nome, tipo, status, telemetria)
└── PoseidonDeviceProvider.php ← fonte única da lista de dispositivos (mock hoje, API do Core depois)

src/Service/Horus/
├── HorusStatus.php         ← value object (status, câmeras, dispositivos, portão) — SEM lista, é instância única
└── HorusStatusProvider.php ← fonte única do status (mock hoje, API do Core depois)
```

**Convenção para novo módulo:** primeiro decida se ele é multi-dispositivo ou instância única (§3.3). Multi-dispositivo replica o padrão Poseidon (`{Modulo}DeviceProvider` + rota índice/dispositivo). Instância única replica o padrão Horus (`{Modulo}StatusProvider` + rota única). Nos dois casos: template próprio em `templates/modulos/`, serviço próprio em `src/Service/{Modulo}/` (nunca reaproveitar o provider de outro módulo, mesmo que a forma seja idêntica), e (se precisar de CSS próprio) `assets/styles/pages/{modulo}.css` importado em `app.css`.

---

## 5. Componentes

### 5.1 Superfícies de vidro (glassmorphism)

Duas variantes — **não inventar uma terceira**, escolher entre elas:

| Classe | Blur | Uso |
|---|---|---|
| `.glass-surface` | 14px | Topbar, sidebar, cards de conteúdo (dashboard, zonas de módulo) |
| `.glass-card` | 18px + sombra de profundidade | Só o card central da tela de login (precisa se destacar mais, é o único elemento da tela) |

Ambas usam `background-color: rgba(20,28,24,.6)` + `border: 1px solid #22332A`.

### 5.2 Topbar ("O Dossel")

Fixa, `h-16`, `glass-surface`, 3 zonas:
- **Esquerda:** logo "Aether" + `.vital-dot` (indicador de vitalidade do Core, ver §5.7) — sempre presente, em toda página.
- **Centro** (`hidden md:flex`): nav de módulos globais, com estado ativo via comparação com `module_active`.
- **Direita:** toggle Bio-Dimming (§5.8) → notificações → dropdown de perfil (§5.4).

### 5.3 Sidebar dinâmica ("As Raízes")

Só renderiza quando `layout_mode = sidebar`. Lista de links simples:
```html
<a class="px-3 py-2 rounded-lg {{ ativo ? 'bg-{cor}/10 text-{cor} font-medium' : 'text-slate-400 hover:text-slate-100 hover:bg-white/5' }}">
```
No mobile é um drawer (`-translate-x-full` + overlay `bg-black/60`), aberto por um botão flutuante `#sidebar-open-btn` que só existe quando há sidebar na página.

### 5.4 Dropdown genérico (padrão reutilizável)

**Não criar dropdown novo do zero** — usar sempre este padrão de atributos, o JS em `base.html.twig` (`initDropdowns()`) já cobre qualquer elemento marcado assim:

```html
<div class="relative" data-dropdown>
    <button data-dropdown-trigger aria-haspopup="true" aria-expanded="false" aria-controls="ID_DO_PAINEL">...</button>
    <div id="ID_DO_PAINEL" data-dropdown-panel class="hidden absolute right-0 mt-2 w-48 rounded-xl glass-surface p-1.5 ...">
        <a class="flex items-center gap-2.5 px-3 py-2 rounded-lg text-sm text-slate-300 hover:text-slate-50 hover:bg-white/5">Item</a>
    </div>
</div>
```
Comportamento já pronto: abre/fecha no clique, fecha ao clicar fora, fecha com `Esc`, fecha outros dropdowns abertos automaticamente. Item destrutivo (ex: "Sair") usa `text-aether-critico/90 hover:bg-aether-critico/10` em vez do cinza padrão.

### 5.5 Toggles / switches (com estado "Pendente")

Três estados visuais fixos — usar sempre os três, nunca só on/off:

| Estado | Trilho | Thumb | Extra |
|---|---|---|---|
| **On** | `bg-aether-broto/30 border-aether-broto/50` | à direita, `bg-aether-broto` | — |
| **Off** | `bg-white/5 border-aether-border` | à esquerda, `bg-slate-500` | — |
| **Pendente** | `bg-aether-ambar/20 border-aether-ambar/40 opacity-70 cursor-wait` | — | `disabled`, ícone de spinner (`animate-spin`) ao lado do label, texto `pendente` em `text-aether-ambar` |

O estado Pendente representa o intervalo entre o clique do usuário e a confirmação real do hardware (ACK do Core/ESP32 — ver `docs/payload-telemetria-poseidon.md` §3.4). **Todo novo atuador precisa implementar esse terceiro estado**, não só on/off.

**Acionamento:** `public/js/services/atuador_control.js` cobre TODO toggle de atuador do sistema, de qualquer módulo — não escrever um handler de clique novo por módulo. Um `<button role="switch">` vira interativo automaticamente se tiver:

```
data-modulo="poseidon|horus|..."     — slug do módulo (monta a URL)
data-atuador="cascata|portao|..."     — nome do atuador dentro do módulo
data-dispositivo="{{ slug }}"          — SÓ em módulo multi-dispositivo (§3.3.1); omitir em instância única (§3.3.2)
```

Fluxo: clique → pendente (imediato, sem esperar rede) → `POST /api/modulos/{modulo}[/dispositivos/{slug}]/atuadores/{atuador}` → Core responde 202 → polling no status até resolver → aplica estado real, ou reverte + sinaliza erro se não confirmar em 15s. Um toggle `disabled` no HTML nunca recebe o listener — é assim que o bombeamento do Poseidon fica permanentemente no visual "Pendente" como demonstração, sem interferência do JS.

**Elementos satélite (label, ícone, borda do card):** o JS só atualiza o próprio botão por padrão. Qualquer texto/cor ao redor que também deva mudar com o estado (ex: o texto "Travado"/"Destravado", a cor do ícone do cadeado, a borda âmbar do card do portão) precisa ser marcado explicitamente — não é automático:

```
<div data-atuador-grupo>                         ← ancestral comum (o JS sobe até aqui a partir do botão)
  <span data-classe-on="..." data-classe-off="..." class="...">     ← classe completa troca por estado
  <p data-texto-on="Travado" data-texto-off="Destravado"
     data-classe-on="..." data-classe-off="..." class="...">Travado</p>  ← texto E classe trocam juntos
  <button role="switch" data-modulo="..." data-atuador="...">...</button>
</div>
```

`data-classe-on`/`data-classe-off` sempre a string **completa** da classe (nunca só a parte que muda) — o `className` é substituído por inteiro. Os satélites só atualizam quando o estado é definitivo (`on`/`off`); durante `pending` continuam mostrando o último estado confirmado.

**Erro de acionamento não usa Tailwind:** a classe `.atuador-erro` (flash vermelho no toggle quando a requisição falha ou expira) é CSS puro em `app.css`, não uma combinação de utilities do Tailwind. Motivo: é aplicada/removida dinamicamente via `classList` no JS — uma classe Tailwind só existe no CSS compilado se aparecer *por extenso* em algum arquivo que o Tailwind escaneia, e um `.js` fora do padrão de conteúdo não garante isso. Regra geral: **qualquer classe que o JS vai adicionar/remover em runtime deve ser CSS puro**, nunca uma utility Tailwind nova que só existiria se o JS a "inventasse" (mesma armadilha de `bg-{{ variavel }}` no Twig, ver §5.6 abaixo — só que essa não dá pra resolver com ternária, porque não é renderização server-side).

### 5.6 Cards de Alerta (estados biológicos)

Três estados fixos, cada um com cor + borda esquerda + texto — nunca usar vermelho puro de alarme industrial:

| Estado | Cor | Borda | Exemplo de uso |
|---|---|---|---|
| **Estável** | `aether-broto` | `border-l-2 !border-l-aether-broto` | Tudo dentro do esperado |
| **Atenção** | `aether-ambar` | `border-l-2 !border-l-aether-ambar` | Degradação leve, ainda não é falha |
| **Crítico** | `aether-critico` | `border-l-2 !border-l-aether-critico` | Falha real, precisa de ação |

Estrutura: `glass-surface p-4 flex items-start gap-3`, ponto colorido (`w-2 h-2 rounded-full`) + eyebrow `text-[11px] font-mono uppercase` na cor do estado + texto em `text-sm text-slate-200`. Sempre envolvido pela classe `.bio-dimmable` (ver §5.8).

**Sempre nomear o dispositivo no eyebrow** (`Atenção · Aquário da Sala`, não só `Atenção`) — com um módulo podendo ter vários ESP32 (ver §3.3), um alerta sem o nome do dispositivo é ambíguo.

**Cuidado com classe Tailwind dinâmica:** nunca monte a classe por interpolação (`bg-{{ variavel }}`) — o Tailwind só gera CSS para classes que aparecem por extenso em algum arquivo escaneado. Use sempre uma ternária Twig com a string completa por branch (`{{ nivel == 'critico' ? 'bg-aether-critico' : '...' }}`), como já é feito para `dispositivo.status` em outros componentes.

### 5.7 Indicador de vitalidade (`.vital-dot`)

Ponto colorido com halo pulsante (`@keyframes vital-pulse`, 2.2s). Usado em 3 lugares: topbar (Core geral), tela de login (marca), sidebar de módulo (conexão daquele módulo específico). **Um só padrão visual para "isto está vivo e conectado" em todo o sistema** — não criar variação nova para essa mesma ideia.

### 5.8 Bio-Dimming

Mecanismo global de baixa luminosidade noturna. Qualquer elemento que deva escurecer nesse modo recebe a classe `.bio-dimmable`; o toggle no topbar liga/desliga `[data-bio-dim="true"]` no `<html>` (persistido em `sessionStorage`), que aplica `opacity:.6; filter:saturate(.75) brightness(.85)` a tudo que tiver essa classe. **Regra prática:** qualquer cor de destaque brilhante (broto/agua/ambar) que apareça fora de texto pequeno — pontos, badges, cards de alerta — deve levar `.bio-dimmable`.

### 5.9 Cards de módulo (dashboard)

`glass-surface rounded-2xl p-5`, com:
- Header: eyebrow mono + nome do módulo (`text-lg font-bold`) à esquerda, badge de status à direita (`rounded-full px-2 py-1 text-[11px] font-mono` + ponto, cor = `aether-broto`/10 se online, `aether-critico`/10 se alerta)
- Rodapé: grid de 3 colunas de telemetria rápida (`border-t border-aether-border pt-3`), cada item = label mono uppercase `text-[10px] text-slate-500` + valor mono `text-sm` colorido por significado (água=agua, umidade=broto, luz=ambar)
- Hover: `hover:shadow-glow-{cor} hover:border-{cor}/30 transition-all duration-300`

### 5.10 Seções "Zona N" (páginas de módulo)

`glass-surface rounded-2xl p-5 md:p-6`, sempre com um eyebrow no topo: `text-[11px] font-mono uppercase tracking-widest text-slate-500`, texto `Zona N · Nome`. É o container padrão para agrupar qualquer bloco funcional dentro de uma página de módulo (ações, telemetria, logs, e futuros).

### 5.11 Placeholder de gráfico (Chart.js ainda não plugado)

`rounded-xl border border-dashed border-aether-border h-48 flex flex-col items-center justify-center gap-2 text-slate-600`, com ícone + texto indicando o `id` do canvas esperado. Usar sempre que uma feature depender de dado ainda não integrado — deixa claro no próprio HTML o que falta plugar.

### 5.12 Console de logs

`bg-black/30 border border-aether-border rounded-xl p-4` com `font-mono text-xs`, cada linha `[HH:MM:SS] namespace.evento :: valor`, cor do "namespace" conforme o tipo de evento (broto=heartbeat ok, agua=leitura de sensor, ambar=aguardando ack).

### 5.13 Inputs de linha inferior (login)

Sem caixa fechada — só uma borda inferior que acende em foco. Classes `.line-input` + `.line-label`, label flutua para cima em `:focus`, `:not(:placeholder-shown)` **e** `:autofill`/`:-webkit-autofill` (os três precisam estar cobertos, autofill de navegador não dispara os dois primeiros de forma confiável). Cor de destaque (`aether-broto`) só aparece durante o foco ativo; preenchido-sem-foco fica em cinza.

### 5.14 Botões

| Tipo | Classe base | Uso |
|---|---|---|
| **Primário (CTA)** | `rounded-xl border border-aether-broto/40 bg-aether-broto/10 text-aether-broto` + hover `bg-aether-broto/20 shadow-glow-broto` | Ação principal de uma tela (ex: "Acessar Ecossistema") |
| **Ícone** | `w-9 h-9 rounded-full border border-aether-border text-slate-400 hover:text-slate-100` | Ações secundárias da topbar (bio-dimming, notificações) |
| **Pill de nav** | `px-3 py-1.5 rounded-full text-sm` | Links de navegação (topbar, sidebar) |

### 5.15 Ícones

Sempre **Heroicons outline**, `viewBox="0 0 24 24"`, `stroke-width="1.8"` (não usar a versão "solid"). Tamanho varia por contexto: `w-4`/`w-4.5` em badges e labels pequenos, `w-5` em nav, `w-6` em placeholders/estados vazios.

---

## 6. Convenções de nomenclatura

| Prefixo/padrão | Significado |
|---|---|
| `aether-*` | Cor do design system (`@theme`) |
| `glass-*` | Superfície glassmorphism |
| `bio-*` | Relacionado ao mecanismo de Bio-Dimming ou aos sliders biomórficos |
| `line-*` | Padrão de input de linha inferior (só usado no login por enquanto) |
| `vital-*` | Indicador de vitalidade/pulso |
| `data-dropdown*` | Hook de JS do dropdown genérico — nunca dar `id`/classe custom pra reimplementar esse comportamento |
| Blocos Twig `layout_mode`, `module_active`, `sidebar`, `content` | Contrato entre `base.html.twig` e qualquer template filho |

---

## 7. Checklist para criar um novo componente/tela

1. A tela estende `base.html.twig`? Definir `layout_mode` (`full` ou `sidebar`) e `module_active`.
2. Tem alguma superfície elevada? Usar `.glass-surface` (ou `.glass-card` só se for tela isolada tipo login).
3. Tem valor numérico/técnico? `font-mono`, nunca `font-sans`.
4. Tem uma cor de destaque "brilhante" fora de texto pequeno? Adicionar `.bio-dimmable`.
5. Tem uma ação que depende de confirmação de hardware? Implementar o terceiro estado "Pendente" (§5.5), não só on/off.
6. Tem um status/alerta? Mapear pro vocabulário biológico (Estável/Atenção/Crítico), não pra semáforo genérico.
7. Precisa de um menu suspenso? Usar o padrão `data-dropdown` (§5.4), não escrever JS novo.
8. Vai ter CSS próprio da tela? Criar `assets/styles/pages/<nome>.css` e importar em `app.css` — nunca `<style>` inline no template nem link direto no `<head>`.
9. Testar em mobile: a navegação principal deveria estar acessível pela bottom nav, e qualquer grid de 3 colunas deve colapsar pra 1 coluna.

---

## 8. Referências relacionadas

- `docs/api-contract-core.md` — contrato HTTP Core C++ (formato de dados que alimenta os componentes de telemetria/status)
- `docs/payload-telemetria-poseidon.md` — protocolo TCP do ESP32 (origem do estado "Pendente" dos toggles)
- `docs/troubleshooting-conexao.md` — diagnóstico de queda de conexão entre as 3 camadas
- `docs/frontend-build-setup.md` — pipeline PostCSS + Tailwind v4
