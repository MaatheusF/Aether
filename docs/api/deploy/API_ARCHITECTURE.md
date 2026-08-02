# API - Arquitetura e Documentação

## 📋 Visão Geral

A API do Aether é um servidor **HTTP RESTful de alto desempenho** construído com **Boost.ASIO** e **Boost.Beast**. 

Implementa o padrão **MVC (Model-View-Controller)** com separação clara de responsabilidades:
- **Controllers**: Processam requisições HTTP
- **Services**: Contêm lógica de negócio
- **DTOs**: Transfer de dados entre camadas
- **Transport**: Gerenciamento do protocolo HTTP

## 🏗️ Estrutura de Pastas

```
aether-core/api/
├── CMakeLists.txt              # Build configuration
├── common/                      # Estruturas compartilhadas
│   ├── HttpMethod.hpp          # Enum dos métodos HTTP
│   ├── HttpRequest.hpp         # Struct de requisição interna
│   └── HttpResponse.hpp        # Struct de resposta interna
├── config/
│   └── ApiConfig.hpp           # Configuração (host, porta)
├── controllers/                 # MVC: Recebe requisições
│   ├── core/                    # Controllers que não pertencem a um módulo específico
│   │   └── StatusController.*
│   └── modules/<NomeDoModulo>/  # Controllers específicos de um módulo (ex: Horus, Poseidon)
│       └── CameraController.*   # (exemplo real: modules/Horus/CameraController.*)
├── dto/                         # Data Transfer Objects
│   ├── core/
│   │   └── StatusResponse.hpp
│   └── modules/<NomeDoModulo>/
│       └── CameraSnapshotResponse.hpp
├── services/                    # Lógica de negócio
│   ├── core/
│   │   └── StatusService.*
│   └── modules/<NomeDoModulo>/
│       └── CameraService.*
└── transport/
    └── rest/                    # Implementação HTTP
        ├── HttpServer.hpp/cpp         # Servidor TCP
        ├── HttpSession.hpp/cpp        # Gerenciamento de sessão
        ├── Router.hpp/cpp             # Despachante de rotas
        ├── RouteRegistry.hpp/cpp      # Registro de rotas
        ├── RouterGet.hpp/cpp          # Handlers GET
        ├── RouterPost.hpp/cpp         # Handlers POST
        ├── RouterPut.hpp/cpp          # Handlers PUT
        └── RouterDelete.hpp/cpp       # Handlers DELETE
```

`controllers/`, `dto/` e `services/` seguem a mesma convenção de duas gavetas:
- **`core/`** — endpoints que não pertencem a nenhum módulo do Aether (ex: `/api/core/status`).
- **`modules/<NomeDoModulo>/`** — endpoints específicos de um módulo (ex: `modules/Horus/` para tudo relacionado a câmeras/portão do Horus). O nome do módulo é o mesmo usado em `aether-core/modules/` (`ModuleTest`, `ModulePoseidon`, etc.).

Isso não cria nenhuma fronteira de build (ainda é tudo compilado junto na lib `aether_api`) — é só convenção de arquivos pra deixar claro o que pertence a que módulo. O `Router` continua sendo o único ponto que conhece todos os controllers, de todos os módulos (ver seção de melhorias no fim deste documento).

## 🔄 Fluxo de Uma Requisição HTTP

```
┌─────────────────────────────────────────────────────────────────┐
│  1. Cliente conecta na porta 9001 via TCP                       │
└────────────────────────────┬──────────────────────────────────┘
                             │
                ┌────────────▼──────────────┐
                │  HttpServer::accept()     │
                │  - Cria tcp::socket       │
                │  - Aceita conexão         │
                │  - Cria HttpSession       │
                └────────────┬──────────────┘
                             │
         ┌───────────────────▼──────────────────┐
         │  2. HttpSession::run()               │
         │  - Lê requisição HTTP (Beast)        │
         │  - Converte para HttpRequest         │
         └───────────────┬──────────────────────┘
                         │
        ┌────────────────▼──────────────────┐
        │  3. Router::dispatch()            │
        │  - Verifica método HTTP           │
        │  - Chama dispatchGet/Post/etc     │
        └────────────────┬──────────────────┘
                         │
      ┌──────────────────▼────────────────────┐
      │  4. dispatchGet/Post/Delete/etc       │
      │  - Matching do path                   │
      │  - Localiza Controller apropriado     │
      └──────────────────┬────────────────────┘
                         │
     ┌────────────────────▼───────────────────┐
     │  5. StatusController::get()            │
     │  - Monta HttpResponse                  │
     │  - Chama Service se necessário         │
     └────────────────────┬───────────────────┘
                          │
    ┌─────────────────────▼────────────────────┐
    │  6. Retorna HttpResponse                 │
    │  - Status, body, headers                 │
    └─────────────────────┬────────────────────┘
                          │
   ┌──────────────────────▼───────────────────┐
   │  7. HttpSession::createResponse()        │
   │  - Converte para Boost.Beast             │
   │  - Prepara para transmissão              │
   └──────────────────────┬───────────────────┘
                          │
  ┌───────────────────────▼──────────────────┐
  │  8. http::write() - Envia ao cliente     │
  │  - Via socket TCP                        │
  │  - Socket é fechado gracefully           │
  └────────────────────────────────────────┘
```

## 📝 Componentes Principais

### HttpServer

**Responsabilidade:** Servidor TCP que aceita conexões

```cpp
// Inicialização
ApiConfig config;
config.host = "0.0.0.0";  // Todas as interfaces
config.port = 9001;

HttpServer server(config);
server.start();  // Bloqueante
```

**Características:**
- Usa `boost::asio::io_context` para I/O
- `tcp::acceptor` escuta em host:port
- Loop infinito aceitando conexões
- Cria `HttpSession` para cada conexão

### HttpSession

**Responsabilidade:** Gerencia uma conexão HTTP individual

Ciclo de vida de uma sessão:
1. Recebe socket TCP
2. Lê requisição HTTP com `boost::beast::http::read`
3. Converte para `HttpRequest` interna
4. Passa para `Router::dispatch()`
5. Converte resposta para Beast
6. Escreve resposta com `http::write`
7. Fecha gracefully

### Router

**Responsabilidade:** Despacha requisições para controllers apropriados

```cpp
// Despacha por método HTTP
HttpResponse Router::dispatch(const HttpRequest& request)
{
    switch (request.method) {
        case HttpMethod::GET:
            return dispatchGet(request);
        case HttpMethod::POST:
            return dispatchPost(request);
        // ...
    }
}

// Cada método faz matching de path
HttpResponse Router::dispatchGet(const HttpRequest& request)
{
    if (request.path == "/api/core/status")
        return m_statusController.get(request);
    
    return notFound();
}
```

### RouteRegistry

**Responsabilidade:** Registro centralizado e legível de rotas (novo!)

Fornece API fluente para adicionar rotas:

```cpp
auto registry = std::make_shared<RouteRegistry>();

registry->get("/api/core/status", [&](const HttpRequest& req) {
    return statusController.get(req);
});

registry->post("/api/users", [&](const HttpRequest& req) {
    return userController.create(req);
});

registry->put("/api/users/:id", [&](const HttpRequest& req) {
    return userController.update(req);
});

registry->delete_("/api/users/:id", [&](const HttpRequest& req) {
    return userController.delete(req);
});

// Buscar rota
auto handler = registry->find(HttpMethod::GET, "/api/core/status");
if (handler) {
    response = (*handler)(request);
}
```

### Controllers

**Responsabilidade:** Processar requisições HTTP

```cpp
class StatusController {
public:
    HttpResponse get(const HttpRequest& request);
    
private:
    StatusService m_service;
};

HttpResponse StatusController::get(const HttpRequest& request)
{
    auto dto = m_service.get();  // Chama service
    
    HttpResponse response;
    response.status = 200;
    response.body = "{ \"message\": \"" + dto.message + "\" }";
    response.headers["Content-Type"] = "application/json";
    
    return response;
}
```

### Services

**Responsabilidade:** Contém lógica de negócio

```cpp
class StatusService {
public:
    static Dto::StatusResponse get();
};

Dto::StatusResponse StatusService::get()
{
    // Lógica de negócio aqui
    // Poderia verificar conexão DB, resources, etc
    return {
        true,
        "Aether API Funcionando"
    };
}
```

### DTOs (Data Transfer Objects)

**Responsabilidade:** Transferir dados entre camadas

```cpp
namespace Aether::Api::Dto {
    struct StatusResponse {
        bool success;
        std::string message;
    };
}
```

## 🚀 Como Adicionar Uma Nova Rota

Exemplo abaixo pra uma rota de usuários pertencente a um módulo hipotético
`Auth` — troque `modules/Auth/` por `core/` se a rota não pertencer a nenhum
módulo específico (ver seção "Estrutura de Pastas" acima).

### Passo 1: Criar o DTO (se necessário)

`aether-core/api/dto/modules/Auth/UserResponse.hpp`:
```cpp
#pragma once

namespace Aether::Api::Dto {
    struct UserResponse {
        int id;
        std::string name;
        std::string email;
    };
}
```

### Passo 2: Criar o Service

`aether-core/api/services/modules/Auth/UserService.hpp`:
```cpp
#pragma once

#include "../../../dto/modules/Auth/UserResponse.hpp"

namespace Aether::Api {
    class UserService {
    public:
        static Dto::UserResponse getById(int id);
        static std::vector<Dto::UserResponse> getAll();
    };
}
```

`aether-core/api/services/modules/Auth/UserService.cpp`:
```cpp
#include "UserService.hpp"

namespace Aether::Api {
    Dto::UserResponse UserService::getById(int id)
    {
        // Buscar no banco de dados
        return { id, "John Doe", "john@example.com" };
    }
    
    std::vector<Dto::UserResponse> UserService::getAll()
    {
        return {
            { 1, "John Doe", "john@example.com" },
            { 2, "Jane Smith", "jane@example.com" }
        };
    }
}
```

### Passo 3: Criar o Controller

`aether-core/api/controllers/modules/Auth/UserController.hpp`:
```cpp
#pragma once

#include "../../../services/modules/Auth/UserService.hpp"
#include "../../../common/HttpResponse.hpp"
#include "../../../common/HttpRequest.hpp"

namespace Aether::Api {
    class UserController {
    public:
        HttpResponse getById(const HttpRequest& request);
        HttpResponse getAll(const HttpRequest& request);
        HttpResponse create(const HttpRequest& request);
        
    private:
        UserService m_service;
        
        HttpResponse buildJsonResponse(
            const std::string& json, 
            int status = 200);
    };
}
```

`aether-core/api/controllers/modules/Auth/UserController.cpp`:
```cpp
#include "UserController.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Aether::Api {
    HttpResponse UserController::getById(const HttpRequest& request)
    {
        // Parse ID do path: /api/users/123
        auto user = m_service.getById(123);
        
        json responseJson;
        responseJson["id"] = user.id;
        responseJson["name"] = user.name;
        responseJson["email"] = user.email;
        
        return buildJsonResponse(responseJson.dump(), 200);
    }
    
    HttpResponse UserController::buildJsonResponse(
        const std::string& json, 
        int status)
    {
        HttpResponse response;
        response.status = status;
        response.body = json;
        response.headers["Content-Type"] = "application/json";
        return response;
    }
}
```

⚠️ **Atenção pro include do próprio header** (`#include "UserController.hpp"` no `.cpp`, `#include "UserService.hpp"` no `.cpp`): como o `.hpp` e o `.cpp` moram na mesma pasta, esse include NUNCA leva `../` na frente, não importa quantos níveis de `modules/<Nome>/` o arquivo esteja. Esse foi justamente o bug mais comum ao mover os arquivos existentes pra essa estrutura — ver nota na seção de melhorias no fim deste documento.

### Passo 4: Registrar a Rota

No `Router.hpp`, adicione:
```cpp
private:
    UserController m_userController;
```

No `RouterGet.cpp`, adicione:
```cpp
HttpResponse Router::dispatchGet(const HttpRequest& request)
{
    // ... rotas existentes ...
    
    if (request.path == "/api/users")
    {
        return m_userController.getAll(request);
    }
    
    if (request.path.find("/api/users/") == 0)
    {
        return m_userController.getById(request);
    }
    
    return notFound();
}
```

Ou usando `RouteRegistry` (recomendado):
```cpp
class MyApp {
    void setupRoutes() {
        auto routes = std::make_shared<RouteRegistry>();
        
        routes->get("/api/users", 
            [&](const HttpRequest& req) {
                return m_userController.getAll(req);
            });
        
        routes->get("/api/users/:id",
            [&](const HttpRequest& req) {
                return m_userController.getById(req);
            });
    }
};
```

## 🧪 Testando a API

### Com cURL

```bash
# GET - Obter status
curl http://localhost:9001/api/test

# GET - Com query string
curl "http://localhost:9001/api/users?page=1"

# POST - Criar usuário
curl -X POST http://localhost:9001/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"John Doe","email":"john@example.com"}'

# PUT - Atualizar usuário
curl -X PUT http://localhost:9001/api/users/1 \
  -H "Content-Type: application/json" \
  -d '{"name":"Jane Doe"}'

# DELETE - Remover usuário
curl -X DELETE http://localhost:9001/api/users/1
```

### Resposta esperada

```json
{
  "success": true,
  "message": "Aether API Funcionando"
}
```

## 📊 Performance

- **Assíncrono**: Usa `boost::asio` para I/O não-bloqueante
- **Buffer eficiente**: `beast::flat_buffer` para leitura
- **HTTP Keep-Alive**: Suportado via Boost.Beast
- **Zero-copy**: Movimento de sockets entre sessões
- **Error handling**: Try-catch em conexões para não travar

## 🔧 Configuração

Edite `aether-core/api/config/ApiConfig.hpp`:

```cpp
struct ApiConfig {
    std::string host = "0.0.0.0";       // Interface de escuta
    uint16_t port = 9001;                // Porta TCP
    unsigned int threads = ...;          // Threads do pool (default: nº de cores, piso 2)
    std::string accessLogPath = ...;     // access.log estilo Apache (ver AccessLogger)
    std::string requestDetailsDir = ...; // Diretório com 1 arquivo por requisição (payload+response)
    std::unordered_set<HttpMethod> loggedMethods = ...;   // Métodos que entram no access log
    std::unordered_set<HttpMethod> detailedMethods = ...; // Métodos que também geram arquivo de detalhe
};
```

## 📚 Próximos Passos

### Curto prazo
- [ ] Implementar path parameters (ex: /users/:id)
- [ ] Adicionar query parameter parsing
- [x] Logging centralizado — `AccessLogger` (access log estilo Apache + arquivo de detalhe por requisição, ver `AccessLogger.hpp`)

### Médio prazo
- [ ] Middleware (authentication, logging)
- [ ] Content negotiation (JSON, XML, etc)
- [ ] Error handling centralizado

### Longo prazo
- [x] Async I/O — `HttpServer`/`HttpSession` reescritos com `async_accept`/`async_read`/`async_write` (Boost.Beast), sem coroutines C++20 por enquanto
- [ ] WebSocket support
- [ ] Rate limiting
- [ ] Cache (Redis integration)
- [x] Multithread handling via thread pool — `HttpServer::start()` sobe `ApiConfig::threads` rodando `io_context::run()`

## 📖 Referências Externas

- [Boost.ASIO Documentation](https://www.boost.org/doc/libs/1_78_0/doc/html/boost_asio.html)
- [Boost.Beast Documentation](https://www.boost.org/doc/libs/1_78_0/libs/beast/doc/html/index.html)
- [HTTP/1.1 Specification](https://tools.ietf.org/html/rfc7231)

## 🤝 Contribuindo

Ao adicionar novas rotas/controllers:
1. Seguir padrão MVC
2. Adicionar comentários Doxygen
3. Manter responsabilidades bem definidas
4. Testar com cURL antes de fazer commit

## 🗂️ Notas da Refatoração core/modules

Ao separar `controllers/`, `dto/` e `services/` em `core/` vs `modules/<Nome>/`:

**Bug encontrado e corrigido:** os quatro arquivos `.cpp` movidos (`StatusController.cpp`,
`CameraController.cpp`, `StatusService.cpp`, `CameraService.cpp`) ficaram com o include do
próprio header (`.hpp` irmão, na mesma pasta) apontando pra fora dela — ex:
`#include "../../CameraController.hpp"` em vez de `#include "CameraController.hpp"`.
Como o `.hpp` sempre se move **junto** com o `.cpp` pra mesma pasta nova, esse include nunca
deveria ganhar `../`, mesmo que a pasta em si fique mais aninhada. É fácil cometer esse erro de
novo ao mover mais arquivos pra essa estrutura (ou ao criar um módulo novo) — vale conferir esse
include específico depois de qualquer `git mv`/refactor de IDE.

**Oportunidades de melhoria nessa mesma direção** (organização/separação por módulo):

1. **`Router` ainda centraliza todos os módulos.** `Router.hpp` tem `m_statusController` e
   `m_cameraController` como membros diretos, e `RouterGet.cpp` faz `if (path == ...)` pra cada
   rota manualmente. A separação de pastas não mudou isso — pra adicionar um módulo novo ainda é
   preciso editar o `Router` central, que conhece o Horus e o "core" ao mesmo tempo. O
   `RouteRegistry` (`transport/rest/RouteRegistry.hpp`) já existe com exatamente o mecanismo pra
   resolver isso (registro de rotas via `std::function`), mas **não está sendo usado** — `Router`
   não o consome, é código morto hoje. Ligar o `RouteRegistry` e deixar cada módulo registrar suas
   próprias rotas (ex: um `RegisterHorusRoutes(registry)` dentro de `controllers/modules/Horus/`)
   completaria a separação: o `Router` deixaria de precisar conhecer `CameraController` diretamente.

2. **Nome `controllers/core/` colide conceitualmente com `aether-core/core/`.** Já existe um
   `core/` na raiz do projeto (EventBus, database, network, utils — a camada de infraestrutura).
   Ter também um `api/controllers/core/` (endpoints sem módulo) usa a mesma palavra pra duas coisas
   diferentes. Considerar renomear pra algo como `controllers/system/` ou `controllers/general/`
   evitaria a ambiguidade — troca simples, mas só vale a pena decidir antes de mais módulos se
   acumularem nessa pasta.

3. **A separação é só de arquivos, não de build.** `api/CMakeLists.txt` continua compilando tudo
   (core + todos os módulos) numa única lib `aether_api` via `file(GLOB_RECURSE ...)`. Isso é
   suficiente pra organização visual, mas não impede um controller do "core" incluir algo de um
   módulo por engano, nem permite compilar/testar um módulo isoladamente. Os módulos "de verdade"
   em `aether-core/modules/` (ex: `ModulePoseidon`) já têm CMakeLists.txt próprios — replicar esse
   padrão pra `api/` (um target por módulo, ex: `aether_api_horus`) tornaria a fronteira real, não
   só convencional. Vale considerar se a lista de módulos crescer.

