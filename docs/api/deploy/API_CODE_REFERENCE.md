# Referência de Código - API HTTP

## 📚 Core Classes

### HttpRequest

**Arquivo:** `common/HttpRequest.hpp`

Representa uma requisição HTTP decodificada.

```cpp
struct HttpRequest {
    HttpMethod method;                              // Método HTTP
    std::string path;                               // Caminho (ex: /api/users)
    std::string body;                               // Corpo da requisição
    std::unordered_map<std::string, std::string> headers;  // Headers HTTP
    std::unordered_map<std::string, std::string> query;    // Query params
};
```

**Exemplo de uso:**

```cpp
// Verificar método
if (request.method == HttpMethod::GET) {
    // Processar GET
}

// Acessar path
std::cout << request.path;  // "/api/users/123"

// Acessar header
auto contentType = request.headers["Content-Type"];

// Acessar query param
auto page = request.query["page"];
```

### HttpResponse

**Arquivo:** `common/HttpResponse.hpp`

Representa uma resposta HTTP pronta para enviar.

```cpp
struct HttpResponse {
    int status = 200;                               // Status HTTP
    std::string body;                               // Corpo da resposta
    std::unordered_map<std::string, std::string> headers;  // Headers
};
```

**Exemplo de uso:**

```cpp
HttpResponse response;
response.status = 200;
response.body = "{\"success\":true}";
response.headers["Content-Type"] = "application/json";
response.headers["X-Custom"] = "value";
return response;
```

### HttpMethod

**Arquivo:** `common/HttpMethod.hpp`

Enumeração dos métodos HTTP suportados.

```cpp
enum class HttpMethod {
    GET,        // Obter recurso
    POST,       // Criar recurso
    PUT,        // Atualizar recurso
    PATCH,      // Atualização parcial
    DELETE_,    // Remover recurso (underscore evita conflito)
    UNKNOWN     // Método não suportado
};
```

### Router

**Arquivo:** `transport/rest/Router.hpp`

Despachante de requisições HTTP para handlers apropriados.

```cpp
class Router {
public:
    /**
     * Despachanta uma requisição para o handler apropriado
     * baseado no método HTTP
     */
    HttpResponse dispatch(const HttpRequest& request);

private:
    // Métodos especializados por tipo HTTP
    HttpResponse dispatchGet(const HttpRequest& request);
    HttpResponse dispatchPost(const HttpRequest& request);
    HttpResponse dispatchPut(const HttpRequest& request);
    HttpResponse dispatchDelete(const HttpRequest& request);
    
    // Resposta padrão para rotas não encontradas
    HttpResponse notFound() const;
};
```

**Exemplo de uso:**

```cpp
Router router;
HttpRequest req;
req.method = HttpMethod::GET;
req.path = "/api/status";

HttpResponse response = router.dispatch(req);
```

### RouteRegistry

**Arquivo:** `transport/rest/RouteRegistry.hpp`

Registro centralizado e legível de rotas HTTP.

```cpp
class RouteRegistry {
public:
    // Métodos para registrar handlers
    void get(const std::string& path, RouteHandler handler);
    void post(const std::string& path, RouteHandler handler);
    void put(const std::string& path, RouteHandler handler);
    void patch(const std::string& path, RouteHandler handler);
    void delete_(const std::string& path, RouteHandler handler);
    
    // Procurar uma rota registrada
    RouteHandler* find(HttpMethod method, const std::string& path);
};

// Type alias para handler de rota
using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;
```

**Exemplo de uso:**

```cpp
auto registry = std::make_shared<RouteRegistry>();

// Registrar GET /api/health
registry->get("/api/health", [](const HttpRequest& req) {
    HttpResponse res;
    res.status = 200;
    res.body = "OK";
    return res;
});

// Registrar POST /api/data
registry->post("/api/data", [](const HttpRequest& req) {
    // Processar POST
    HttpResponse res;
    res.status = 201;
    return res;
});

// Buscar rota
auto handler = registry->find(HttpMethod::GET, "/api/health");
if (handler) {
    response = (*handler)(request);
}
```

### HttpServer

**Arquivo:** `transport/rest/HttpServer.hpp`

Servidor TCP que aceita conexões HTTP.

```cpp
class HttpServer {
public:
    /**
     * Construtor - inicializa com configuração
     * @param config ApiConfig com host e porta
     */
    explicit HttpServer(const ApiConfig& config);
    
    /**
     * Inicia servidor (bloqueante)
     * Loop infinito aceitando conexões
     */
    void start();

private:
    void accept();  // Aceita uma conexão
};
```

**Exemplo de uso:**

```cpp
ApiConfig config;
config.host = "0.0.0.0";
config.port = 9001;

HttpServer server(config);
server.start();  // Bloqueante
```

### HttpSession

**Arquivo:** `transport/rest/HttpSession.hpp`

Gerencia uma conexão HTTP individual.

```cpp
class HttpSession {
public:
    /**
     * Construtor
     * @param socket Socket TCP da conexão
     * @param router Router para processar requisições
     */
    HttpSession(
        boost::asio::ip::tcp::socket socket,
        Router& router);
    
    /**
     * Executa a sessão
     * Lê requisição, processa, envia resposta
     */
    void run();

private:
    // Converte Beast request para HttpRequest interna
    HttpRequest createRequest(
        const boost::beast::http::request<
            boost::beast::http::string_body>& request);
    
    // Converte HttpResponse interna para Beast response
    boost::beast::http::response<
        boost::beast::http::string_body> createResponse(
        const HttpResponse& response);
};
```

**Exemplo de uso (automaticamente via HttpServer):**

```cpp
// HttpSession é criada automaticamente por HttpServer
// Normalmente não precisa usar direto
tcp::socket socket(ioContext);
acceptor.accept(socket);

Router router;
HttpSession session(std::move(socket), router);
session.run();  // Processa requisição
```

### ApiConfig

**Arquivo:** `config/ApiConfig.hpp`

Configuração do servidor HTTP.

```cpp
struct ApiConfig {
    std::string host = "0.0.0.0";  // Interface de escuta
    uint16_t port = 9001;           // Porta TCP
};
```

**Exemplo de uso:**

```cpp
ApiConfig config1;  // Usa defaults

ApiConfig config2;
config2.host = "127.0.0.1";  // Localhost apenas
config2.port = 8080;

HttpServer server(config2);
```

## 🎮 Padrão MVC na Prática

### Service (Negócio)

```cpp
namespace Aether::Api {
    class UserService {
    public:
        static Dto::User getById(int id);
        static std::vector<Dto::User> getAll();
        static Dto::User create(const std::string& name);
    };
}
```

### DTO (Transfer)

```cpp
namespace Aether::Api::Dto {
    struct User {
        int id;
        std::string name;
        std::string email;
    };
}
```

### Controller (HTTP)

```cpp
namespace Aether::Api {
    class UserController {
    private:
        UserService m_service;
    
    public:
        // Controller recebe requisição HTTP
        HttpResponse get(const HttpRequest& request) {
            // Delega ao service
            auto user = m_service.getById(123);
            
            // Monta resposta
            HttpResponse response;
            response.status = 200;
            response.body = "...JSON do user...";
            return response;
        }
    };
}
```

### Router (Despachante)

```cpp
class Router {
private:
    UserController m_userController;

public:
    HttpResponse dispatchGet(const HttpRequest& request) {
        if (request.path == "/api/users/123") {
            return m_userController.get(request);
        }
        return notFound();
    }
};
```

## 🔄 Fluxo de Conversão de Dados

```
Cliente TCP
    ↓ [envia JSON]
┌──────────────────────┐
│ HttpSession::run()   │
│ lê com beast::http   │
└──────────────────────┘
    ↓
┌──────────────────────────────┐
│ HttpSession::createRequest() │
│ Beast → HttpRequest          │
└──────────────────────────────┘
    ↓ [HttpRequest]
┌──────────────────────┐
│ Router::dispatch()   │
│ → controller         │
└──────────────────────┘
    ↓ [HttpResponse]
┌─────────────────────────────┐
│ HttpSession::createResponse │
│ HttpResponse → Beast        │
└─────────────────────────────┘
    ↓
┌──────────────────────┐
│ http::write()        │
│ envia JSON ao cliente│
└──────────────────────┘
```

## 📝 Snippets de Código Úteis

### Montar resposta JSON simples

```cpp
HttpResponse buildJsonResponse(
    bool success, 
    const std::string& message)
{
    HttpResponse response;
    response.status = 200;
    response.body = "{\"success\":" + std::string(success ? "true" : "false")
                  + ",\"message\":\"" + message + "\"}";
    response.headers["Content-Type"] = "application/json";
    return response;
}
```

### Extrair informação da requisição

```cpp
// Verificar método
if (request.method != HttpMethod::POST) {
    return buildJsonResponse(false, "Method not allowed");
}

// Verificar Content-Type
auto contentType = request.headers["Content-Type"];
if (contentType != "application/json") {
    return buildJsonResponse(false, "Content-Type must be JSON");
}

// Verificar corpo vazio
if (request.body.empty()) {
    return buildJsonResponse(false, "Body required");
}
```

### Verificar caminho

```cpp
// Exact match
if (request.path == "/api/users") {
    return m_userController.list(request);
}

// Prefix match
if (request.path.find("/api/users/") == 0) {
    // Path começa com /api/users/
    std::string remainder = request.path.substr(11);  // Após /api/users/
    return m_userController.getById(request);
}
```

### Retornar diferentes status

```cpp
// Sucesso
HttpResponse ok;
ok.status = 200;

// Criado
HttpResponse created;
created.status = 201;

// Bad request
HttpResponse badReq;
badReq.status = 400;

// Not found
HttpResponse notFound;
notFound.status = 404;

// Server error
HttpResponse error;
error.status = 500;
```

## 🧪 Testing

### Unit Testing com Controllers

```cpp
#include <gtest/gtest.h>
#include "UserController.hpp"

TEST(UserControllerTest, GetReturnsOK) {
    UserController controller;
    HttpRequest request;
    request.method = HttpMethod::GET;
    request.path = "/api/users";
    
    HttpResponse response = controller.get(request);
    
    EXPECT_EQ(response.status, 200);
    EXPECT_NE(response.body.find("users"), std::string::npos);
}
```

### Integration Testing

```cpp
TEST(ApiIntegrationTest, FullRequestCycle) {
    ApiConfig config{"0.0.0.0", 9001};
    HttpServer server(config);
    
    // Em thread separada
    std::thread serverThread([&]() {
        server.start();
    });
    
    // Fazer requisição
    // Verificar resposta
    
    serverThread.join();
}
```

## 🔗 Dependências

- **Boost.ASIO**: I/O assíncrono
- **Boost.Beast**: HTTP parsing
- **std::function**: Callbacks em C++

## 📖 Referências

- Boost.ASIO: http://www.boost.org/doc/libs/release/doc/html/boost_asio/
- Boost.Beast: http://www.boost.org/doc/libs/release/libs/beast/doc/html/
- HTTP/1.1 RFC: https://tools.ietf.org/html/rfc7231

