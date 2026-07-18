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
│   └── Arquvios controllers dos Métodos
├── dto/                         # Data Transfer Objects
│   └── DTO dos Métodos
├── services/                    # Lógica de negócio
│   └── Arquvios services dos Métodos
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
    if (request.path == "/api/status")
        return m_statusController.get(request);
    
    return notFound();
}
```

### RouteRegistry

**Responsabilidade:** Registro centralizado e legível de rotas (novo!)

Fornece API fluente para adicionar rotas:

```cpp
auto registry = std::make_shared<RouteRegistry>();

registry->get("/api/status", [&](const HttpRequest& req) {
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
auto handler = registry->find(HttpMethod::GET, "/api/status");
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

### Passo 1: Criar o DTO (se necessário)

`aether-core/api/dto/UserResponse.hpp`:
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

`aether-core/api/services/UserService.hpp`:
```cpp
#pragma once

#include "../dto/UserResponse.hpp"

namespace Aether::Api {
    class UserService {
    public:
        static Dto::UserResponse getById(int id);
        static std::vector<Dto::UserResponse> getAll();
    };
}
```

`aether-core/api/services/UserService.cpp`:
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

`aether-core/api/controllers/UserController.hpp`:
```cpp
#pragma once

#include "../services/UserService.hpp"
#include "../common/HttpResponse.hpp"
#include "../common/HttpRequest.hpp"

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

`aether-core/api/controllers/UserController.cpp`:
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
    std::string host = "0.0.0.0";  // Interface de escuta
    uint16_t port = 9001;           // Porta TCP
};
```

## 📚 Próximos Passos

### Curto prazo
- [ ] Implementar path parameters (ex: /users/:id)
- [ ] Adicionar query parameter parsing
- [ ] Logging centralizado

### Médio prazo
- [ ] Middleware (authentication, logging)
- [ ] Content negotiation (JSON, XML, etc)
- [ ] Error handling centralizado

### Longo prazo
- [ ] Async I/O com coroutines C++20
- [ ] WebSocket support
- [ ] Rate limiting
- [ ] Cache (Redis integration)
- [ ] Multithread handling via thread pool

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

