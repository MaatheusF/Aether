# ⚡ Quick Start - API HTTP Aether

## 🚀 Inicializar em 1 Minuto

### 1. Compilar

```bash
cd C:\projetos\Aether\cmake-build-debug
cmake --build . --config Debug
```

### 2. Executar

```cpp
// Em aetherd.cpp ou main.cpp
#include "api/config/ApiConfig.hpp"
#include "api/transport/rest/HttpServer.hpp"

using namespace Aether::Api;

ApiConfig config;
config.host = "0.0.0.0";
config.port = 9001;

HttpServer server(config);
server.start();  // Inicia servidor
```

### 3. Testar

```bash
curl http://localhost:9001/api/test
```

**Resposta esperada:**
```json
{
  "success": true,
  "message": "Aether API Funcionando"
}
```

✅ **Pronto!** API está rodando!

---

## 📚 Documentação

| Documento | Para... |
|-----------|---------|
| [`README.md`](README.md) | Índice e guia rápido |
| [`API_USAGE.md`](API_USAGE.md) | 📝 Aprender a usar a API |
| [`API_ARCHITECTURE.md`](API_ARCHITECTURE.md) | 🏗️ Entender como funciona |
| [`API_CODE_REFERENCE.md`](API_CODE_REFERENCE.md) | 📚 Referência técnica |
| [`VISUAL_SUMMARY.md`](VISUAL_SUMMARY.md) | 📊 Ver antes/depois |

---

## 🔧 Adicionar Uma Nova Rota em 5 Passos

### Passo 1: Criar DTO
`aether-core/api/dto/UserResponse.hpp`:
```cpp
namespace Aether::Api::Dto {
    struct UserResponse {
        int id;
        std::string name;
    };
}
```

### Passo 2: Criar Service
`aether-core/api/services/UserService.hpp`:
```cpp
namespace Aether::Api {
    class UserService {
    public:
        static Dto::UserResponse getById(int id);
    };
}
```

### Passo 3: Criar Controller
`aether-core/api/controllers/UserController.hpp`:
```cpp
namespace Aether::Api {
    class UserController {
    public:
        HttpResponse getById(const HttpRequest& request);
    private:
        UserService m_service;
    };
}
```

### Passo 4: Registrar Rota (RouteRegistry)
```cpp
auto routes = std::make_shared<RouteRegistry>();
routes->get("/api/users/:id", [&](const HttpRequest& req) {
    return userController.getById(req);
});
```

### Passo 5: Testar
```bash
curl http://localhost:9001/api/users/123
```

✅ **Pronto!** Nova rota funcionando!

---

## 🎯 Padrão Recomendado

```
Requisição HTTP
     ↓
HttpServer (aceita conexão)
     ↓
HttpSession (lê HTTP com Beast)
     ↓
HttpRequest (conversão interna)
     ↓
Router.dispatch() (escolhe handler)
     ↓
RouteRegistry.find() (busca rota)
     ↓
Controller (processa)
     ↓
Service (lógica de negócio)
     ↓
DTO (retorna dados)
     ↓
HttpResponse (monta resposta)
     ↓
Envia ao cliente
```

---

## 🛡️ Headers HTTP Úteis

### Enviar (Request)
```bash
curl -H "Content-Type: application/json" \
     -H "User-Agent: MeuApp/1.0" \
     http://localhost:9001/api/users
```

### Receber (Response)
```cpp
response.headers["Content-Type"] = "application/json";
response.headers["X-Custom"] = "valor";
response.status = 200;
```

---

## 🔍 Erros Comuns

| Erro | Solução |
|------|---------|
| **Connection refused** | Servidor não está rodando: `netstat -an \| grep 9001` |
| **404 Not Found** | Rota não registrada no Router |
| **Resposta vazia** | `response.body` não foi preenchido |
| **Bad request** | Validar `Content-Type: application/json` |

---

## 📊 Estrutura MVC

```
Model = DTO (StatusResponse)
   ↓
View = HttpResponse (JSON body)
   ↓
Controller = StatusController (processa HTTP)
   ↓
Service = StatusService (lógica)
```

Exemplo real:
```
Requisição GET /api/status
   ↓
StatusController::get(HttpRequest)
   ↓
StatusService::get() → DTO::StatusResponse
   ↓
Monta HttpResponse com JSON
   ↓
Resposta 200 OK
```

---

## 📝 Exemplo Completo de Controller

```cpp
// controllers/ProductController.hpp
class ProductController {
public:
    HttpResponse list(const HttpRequest& request);
    HttpResponse create(const HttpRequest& request);
    
private:
    ProductService m_service;
};

// controllers/ProductController.cpp
HttpResponse ProductController::list(const HttpRequest& request)
{
    auto products = m_service.list();
    
    // Montar JSON (em produção use nlohmann::json)
    std::string json = "[";
    for (auto& p : products) {
        json += "{\"id\":" + std::to_string(p.id) + "}";
    }
    json += "]";
    
    HttpResponse response;
    response.status = 200;
    response.body = json;
    response.headers["Content-Type"] = "application/json";
    return response;
}
```

---

## 🧪 Teste com cURL

```bash
# GET
curl http://localhost:9001/api/status

# GET com header
curl -H "Accept: application/json" http://localhost:9001/api/users

# POST com JSON
curl -X POST http://localhost:9001/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"John","email":"john@example.com"}'

# PUT
curl -X PUT http://localhost:9001/api/users/1 \
  -H "Content-Type: application/json" \
  -d '{"name":"Jane"}'

# DELETE
curl -X DELETE http://localhost:9001/api/users/1
```

---

## 🚀 Próximos Passos

1. ✅ Compilar e testar servidor
2. ✅ Testar rota padrão com curl
3. 📖 Ler `/docs/api/README.md`
4. 🏗️ Ler `/docs/api/API_ARCHITECTURE.md`
5. 💻 Implementar primeira rota customizada
6. 🧪 Testar com curl

---

## 📞 Referência Rápida

**Classes principais:**
- `HttpServer` - Servidor TCP
- `HttpSession` - Gerencia conexão
- `Router` - Despachante de rotas
- `RouteRegistry` - Registro de rotas
- `Controllers` - Processam HTTP

**Estruturas:**
- `HttpRequest` - Requisição interna
- `HttpResponse` - Resposta interna
- `HttpMethod` - Enum (GET, POST, etc)
- `ApiConfig` - Configuração (host, port)

**Namespaces:**
- `Aether::Api` - API HTTP
- `Aether::Api::Dto` - DTOs

---

**Versão:** 1.0  
**Data:** 2026-07-18  
**Status:** ✅ Pronto para uso

