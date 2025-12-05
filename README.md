# AETHER

A seguir está um **template inicial** para o projeto AETHER, incluindo a estrutura de diretórios, descrição detalhada de cada parte e orientações de implementação.

Você pode expandir e ajustar conforme o projeto evolui.

---

# 📁 Estrutura Geral do Projeto

```
AETHER/
│
├── core/
│   ├── adapters/
│   ├── assets/
│   ├── config/
│   ├── diagnostics/
│   ├── eventbus/
│   ├── events/
│   ├── hw/
│   ├── ipc/
│   ├── kernel/
│   ├── network/
│   ├── runtime/
│   ├── security/
│   ├── storage/
│   ├── utils/
│   └── CMakeLists.txt
│
├── modules/
│   └── (subrepos de módulos)
│
├── protocols/
│   └── (subrepos de protocolos)
│
├── apps/
│   ├── aetherd/
│   ├── cli/
│   └── dashboard/
│
├── tests/
│
├── scripts/
│
└── README.md
```

---

# 📦 core/

O núcleo do AETHER. Nunca depende de módulos.

## `core/adapters/`

Drivers de entrada e saída.

* tcp_adapter
* zigbee_adapter
* serial_adapter
* lora_adapter

## `core/assets/`

Arquivos estáticos:

* templates
* exemplos de configs
* certificados placeholder

## `core/config/`

Carregamento, parsing e validação de configs.

## `core/diagnostics/`

Ferramentas de saúde do sistema:

* watchdog
* perf
* thread monitor
* event inspector

## `core/eventbus/`

Implementação do sistema Pub/Sub interno:

* fila
* publisher
* subscriber registry

## `core/events/`

Onde vivem as **estruturas dos eventos**:

* EventBase
* EventType enum
* ConnectionEvent
* DataEvent
* ModuleEvent

## `core/hw/`

Abstração de hardware para um futuro bare-metal.

## `core/ipc/`

Comunicação interna entre processos:

* Unix sockets
* Pipes
* Ponte entre CLI e daemon

## `core/kernel/`

Camada avançada (opcional):

* scheduler
* lifecycle manager
* services

## `core/network/`

Apenas baixo nível de rede.

* sockets crus
* accept
* bind
* send/recv

## `core/runtime/`

Gerencia o ciclo de vida dos módulos:

* carregar módulos
* expor APIs internas
* eventos de lifecycle

## `core/security/`

Segurança geral:

* tokens
* chaves
* verificação de integridade
* anti-spoofing

## `core/storage/`

Banco de dados:

* migrations
* schemas
* abstração do driver (PostgreSQL)

## `core/utils/`

Funções gerais:

* logs
* json
* time helpers
* string helpers

---

# 📁 modules/

Cada módulo vive isolado em seu próprio subrepo.

Exemplos:

* `modules/poseidon/`
* `modules/irrigation/`
* `modules/cameras/`
* `modules/survival/`

Cada módulo tem:

```
modules/example_module/
├── src/
├── include/
├── scripts/
├── migrations/
└── CMakeLists.txt
```

---

# 📁 protocols/

Protocolo ≠ Módulo.

Exemplo:

* `esp32_proto`
* `ngl_proto` (Next Generation LoRa)
* `zigbee_proto`
* `aether_internal_proto`

Cada protocolo contém:

```
include/<proto>/
src/
CMakeLists.txt
```

---

# 📁 apps/

Aplicações externas que usam o Aether.

### `aetherd/`

O daemon principal. Aqui fica o `main()`.

### `cli/`

Ferramenta administrativa tipo:

```
aetherctl list modules
aetherctl send event MODULE_NAME {...}
aetherctl restart core
```

### `dashboard/`

Interface web (PHP, Laravel, React, Vue, etc).

Pode viver em outro repo ou aqui.

---

# 📁 tests/

Testes automáticos:

* core_tests
* module_tests
* protocol_tests

---

# 📁 scripts/

Scripts úteis:

* build.sh
* deploy.sh
* migrate.sh
* generate_module.sh

---

# 📘 README.md (raiz)

Deve conter:

* visão geral do Aether
* como compilar
* como rodar
* arquitetura do Core
* como criar módulos
* como criar protocolos

---

Se quiser, posso agora:
✔ gerar todos os CMakeLists base
✔ criar READMEs automáticos para cada pasta
✔ gerar skeleton de código C++ (EventBus, Adapter, Module)
✔ gerar servidor TCP e loop do daemon
✔ criar CLI inicial (`aetherctl`)

É só pedir!
