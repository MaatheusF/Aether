# Guia de Uso da API HTTP

## 🚀 Inicialização Rápida

### 1. Configurar e Iniciar o Servidor

No seu `main.cpp` ou `aetherd.cpp`:

```cpp
#include "api/config/ApiConfig.hpp"
#include "api/transport/rest/HttpServer.hpp"

int main()
{
    using namespace Aether::Api;
    
    // Criar configuração
    ApiConfig config;
    config.host = "0.0.0.0";  // Escuta em todas as interfaces
    config.port = 9001;        // Na porta 9001
    
    // Criar e iniciar servidor
    HttpServer server(config);
    server.start();  // Método bloqueante
    
    return 0;
}
```

### 2. Testar a Rota Padrão

```bash
curl http://localhost:9001/api/test
```

Resposta esperada:
```json
{
  "success": true,
  "message": "Aether API Funcionando"
}
```

## 📋 Estrutura de Requisição/Resposta HTTP

### Requisição HTTP

Uma requisição HTTP completa para a API segue este formato:

```
METHOD /path HTTP/1.1
Host: localhost:9001
Content-Type: application/json
Content-Length: 123

{
  "dados": "do corpo"
}
```

Internamente, é convertida para `HttpRequest`:

```cpp
struct HttpRequest {
    HttpMethod method;                              // GET, POST, PUT, DELETE, etc
    std::string path;                               // "/api/users"
    std::string body;                               // Corpo em JSON/texto
    std::unordered_map<string, string> headers;     // Headers HTTP
    std::unordered_map<string, string> query;       // ?foo=bar&baz=qux
};
```

### Resposta HTTP

```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 456

{
  "success": true,
  "data": {...}
}
```

Internamente:

```cpp
struct HttpResponse {
    int status;                                     // 200, 404, 500, etc
    std::string body;                               // JSON/HTML/texto
    std::unordered_map<string, string> headers;     // Headers customizados
};
```

## 🎯 Padrões de Roteamento

### Simple Path Matching

Atualmente, o roteamento é feito com string matching simples:

```cpp
HttpResponse Router::dispatchGet(const HttpRequest& request)
{
    // Exact match
    if (request.path == "/api/status")
    {
        return m_statusController.get(request);
    }
    
    // Prefix match para IDs
    if (request.path.find("/api/users/") == 0)
    {
        // /api/users/123 → 123 pode ser extraído
        // TODO: implementar parser de path params
        return m_userController.getById(request);
    }
    
    return notFound();
}
```

### RouteRegistry (Recomendado para novos projetos)

Para uma abordagem mais limpa e legível, use `RouteRegistry`:

```cpp
#include "transport/rest/RouteRegistry.hpp"

std::shared_ptr<RouteRegistry> setupRoutes(
    StatusController& statusCtrl,
    UserController& userCtrl)
{
    auto registry = std::make_shared<RouteRegistry>();
    
    // GET routes
    registry->get("/api/status", 
        [&](const HttpRequest& req) {
            return statusCtrl.get(req);
        });
    
    registry->get("/api/users",
        [&](const HttpRequest& req) {
            return userCtrl.getAll(req);
        });
    
    // POST routes
    registry->post("/api/users",
        [&](const HttpRequest& req) {
            return userCtrl.create(req);
        });
    
    // PUT routes
    registry->put("/api/users/:id",
        [&](const HttpRequest& req) {
            return userCtrl.update(req);
        });
    
    // DELETE routes
    registry->delete_("/api/users/:id",
        [&](const HttpRequest& req) {
            return userCtrl.delete_(req);
        });
    
    return registry;
}
```

## 📝 Implementando um Controller

### Step 1: Header

```cpp
// controllers/ProductController.hpp
#pragma once

#include "../common/HttpRequest.hpp"
#include "../common/HttpResponse.hpp"
#include "../services/ProductService.hpp"

namespace Aether::Api
{
    /**
     * @brief Controller de Produtos
     */
    class ProductController
    {
    public:
        HttpResponse list(const HttpRequest& request);
        HttpResponse getById(const HttpRequest& request);
        HttpResponse create(const HttpRequest& request);
        HttpResponse update(const HttpRequest& request);
        HttpResponse delete_(const HttpRequest& request);
        
    private:
        ProductService m_service;
        HttpResponse buildJsonResponse(
            const std::string& json, 
            int status = 200);
    };
}
```

### Step 2: Implementation

```cpp
// controllers/ProductController.cpp
#include "ProductController.hpp"

namespace Aether::Api
{
    HttpResponse ProductController::list(const HttpRequest& request)
    {
        // Buscar todos os produtos
        auto products = m_service.list();
        
        // Montar JSON (usar biblioteca como nlohmann/json é recomendado)
        std::string json = "[";
        for (size_t i = 0; i < products.size(); ++i) {
            json += "{\"id\":" + std::to_string(products[i].id);
            json += ",\"name\":\"" + products[i].name + "\"}";
            if (i < products.size() - 1) json += ",";
        }
        json += "]";
        
        return buildJsonResponse(json);
    }
    
    HttpResponse ProductController::getById(const HttpRequest& request)
    {
        // request.path = "/api/products/123"
        // TODO: implementar parser de path params
        
        auto product = m_service.getById(123);
        
        std::string json = "{\"id\":" + std::to_string(product.id);
        json += ",\"name\":\"" + product.name + "\"}";
        
        return buildJsonResponse(json);
    }
    
    HttpResponse ProductController::create(const HttpRequest& request)
    {
        // Request com dados em JSON no body
        // TODO: implementar JSON parser
        
        auto newProduct = m_service.create("");
        
        std::string json = "{\"id\":" + std::to_string(newProduct.id) + "}";
        
        return buildJsonResponse(json, 201);  // 201 Created
    }
    
    HttpResponse ProductController::buildJsonResponse(
        const std::string& json, 
        int status)
    {
        HttpResponse response;
        response.status = status;
        response.body = json;
        response.headers["Content-Type"] = "application/json";
        response.headers["Access-Control-Allow-Origin"] = "*";
        return response;
    }
}
```

## 🔌 Estrutura de Headers HTTP

### Headers Comuns em Requisições

| Header | Exemplo | Propósito |
|--------|---------|-----------|
| `Content-Type` | `application/json` | Tipo do corpo da requisição |
| `Content-Length` | `1234` | Tamanho do corpo |
| `User-Agent` | `curl/7.68.0` | Cliente HTTP |
| `Accept` | `application/json` | Tipo aceito em resposta |

### Headers Comuns em Respostas

| Header | Exemplo | Propósito |
|--------|---------|-----------|
| `Content-Type` | `application/json` | Tipo do corpo da resposta |
| `Content-Length` | `456` | Tamanho do corpo |
| `Connection` | `close` | Keep-alive ou encerrar |

### Como Adicionar Headers na Resposta

```cpp
HttpResponse response;
response.status = 200;
response.body = "{\"message\":\"OK\"}";

// Adicionar headers customizados
response.headers["Content-Type"] = "application/json";
response.headers["X-Custom-Header"] = "custom-value";
response.headers["Access-Control-Allow-Origin"] = "*";

return response;
```

## 🛡️ Códigos de Status HTTP

| Status | Significado | Quando usar |
|--------|-------------|-------------|
| `200` | OK | Sucesso na operação GET/PUT |
| `201` | Created | Sucesso em POST (novo recurso) |
| `204` | No Content | Sucesso mas sem corpo (ex: DELETE) |
| `400` | Bad Request | Dados inválidos do cliente |
| `401` | Unauthorized | Autenticação necessária |
| `403` | Forbidden | Acesso negado |
| `404` | Not Found | Rota/recurso não existe |
| `409` | Conflict | Conflito (ex: ID duplicado) |
| `500` | Internal Server Error | Erro no servidor |
| `503` | Service Unavailable | Servidor indisponível |

### Exemplos

```cpp
// Sucesso
HttpResponse successResponse;
successResponse.status = 200;
successResponse.body = "{\"data\":\"OK\"}";

// Criado
HttpResponse createdResponse;
createdResponse.status = 201;
createdResponse.body = "{\"id\":123}";

// Erro cliente
HttpResponse badRequest;
badRequest.status = 400;
badRequest.body = "{\"error\":\"Missing required field\"}";

// Erro servidor
HttpResponse serverError;
serverError.status = 500;
serverError.body = "{\"error\":\"Internal server error\"}";
```

## 📩 Exemplos de Requisições cURL

### GET - Obter lista

```bash
curl http://localhost:9001/api/products
```

### GET - Com headers customizados

```bash
curl -H "Accept: application/json" \
     -H "User-Agent: MyApp/1.0" \
     http://localhost:9001/api/products
```

### POST - Criar recurso

```bash
curl -X POST http://localhost:9001/api/products \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Novo Produto",
    "price": 99.99,
    "category": "Eletrônicos"
  }'
```

### PUT - Atualizar recurso

```bash
curl -X PUT http://localhost:9001/api/products/123 \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Nome Atualizado",
    "price": 129.99
  }'
```

### DELETE - Remover recurso

```bash
curl -X DELETE http://localhost:9001/api/products/123
```

### HEAD - Verificar existência (sem corpo)

```bash
curl -I http://localhost:9001/api/products/123
```

## 🔍 Debugging

### Verbose cURL (ver headers)

```bash
# -v mostra request e response completos
curl -v http://localhost:9001/api/test

# Saída:
# > GET /api/test HTTP/1.1
# > Host: localhost:9001
# > User-Agent: curl/7.68.0
# >
# < HTTP/1.1 200 OK
# < Content-Type: application/json
# < Content-Length: 45
# <
# {"success":true,"message":"Aether API Funcionando"}
```

### Testar conexão

```bash
# Testar se servidor está rodando
curl -i http://localhost:9001/api/test

# -i mostra headers e corpo
```

## ⚙️ Troubleshooting

### "Connection refused"

```
curl: (7) Failed to connect to localhost port 9001: Connection refused
```

**Solução:**
- Verificar se servidor está rodando: `netstat -an | grep 9001`
- Verificar se não há outro processo na porta
- Iniciar servidor com `HttpServer server(config); server.start();`

### "404 Not Found"

```json
{"success":false,"message":"Not Found"}
```

**Solução:**
- Verificar se rota está registrada em Router
- Verificar se path está correto
- Adicionar debug em dispatchGet/Post/etc

### Resposta vazia

**Solução:**
- Verificar se `response.body` foi preenchido
- Verificar se status HTTP está correto
- Adicionar headers Content-Type

## 🚀 Production Tips

1. **Use RouteRegistry** para gerenciar rotas centralizadamente
2. **Implemente logging** em cada controller/service
3. **Valide entrada** em todos os controllers
4. **Trate exceções** com try-catch
5. **Implemente rate limiting** para evitar abuse
6. **Use HTTPS** em produção (considerar certificados)
7. **Configure CORS** adequadamente
8. **Monitore performance** com logs de tempo

## 📚 Recursos Adicionais

- Veja `/docs/api/API_ARCHITECTURE.md` para arquitetura detalhada
- Veja `/docs/api/API_CODE_REFERENCE.md` para referência de código

