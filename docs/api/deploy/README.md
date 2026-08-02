# 📖 Documentação da API HTTP do Aether

Bem-vindo à documentação completa da API HTTP do Aether. Aqui você encontrará tudo que precisa para entender, usar e estender a API.

## 🚀 Comece por Aqui

Se você é novo na API, comece por:

1. **[Guia de Uso Rápido](API_USAGE.md)** - Inicialize e teste a API em 5 minutos
2. **[Arquitetura da API](API_ARCHITECTURE.md)** - Entenda como funciona internamente

## 📚 Documentos Disponíveis

### 1. **API_USAGE.md** - Guia Prático
- ✅ Inicialização rápida
- ✅ Estrutura de requisição/resposta
- ✅ Padrões de roteamento
- ✅ Exemplos com cURL
- ✅ Troubleshooting

**Quando usar:** Quando quer aprender a usar a API com exemplos práticos.

### 2. **API_ARCHITECTURE.md** - Design e Implementação
- ✅ Visão geral da arquitetura
- ✅ Fluxo de requisições
- ✅ Componentes principais (HttpServer, Router, etc)
- ✅ Como adicionar novas rotas
- ✅ Performance e otimizações
- ✅ Próximos passos

**Quando usar:** Quando quer entender como a API foi projetada e como estendê-la.

### 3. **API_CODE_REFERENCE.md** - Referência Técnica
- ✅ Documentação de todas as classes
- ✅ Signatures de funções
- ✅ Exemplos de código
- ✅ Padrão MVC na prática
- ✅ Snippets úteis
- ✅ Testes unitários

**Quando usar:** Como referência durante desenvolvimento.

## 🎯 Guia por Caso de Uso

### "Quero testar a API"
→ Veja [Quick Start em API_USAGE.md](API_USAGE.md#-inicialização-rápida)

### "Quero adicionar uma nova rota"
→ Veja [Como Adicionar Uma Nova Rota em API_ARCHITECTURE.md](API_ARCHITECTURE.md#-como-adicionar-uma-nova-rota)

### "Quero entender o fluxo de uma requisição"
→ Veja [Fluxo de Uma Requisição HTTP em API_ARCHITECTURE.md](API_ARCHITECTURE.md#-fluxo-de-uma-requisição-http)

### "Quero criar um novo Controller"
→ Veja [Implementando um Controller em API_USAGE.md](API_USAGE.md#-implementando-um-controller)

### "Preciso da referência de uma classe"
→ Veja [Core Classes em API_CODE_REFERENCE.md](API_CODE_REFERENCE.md#-core-classes)

### "Estou recebendo erro"
→ Veja [Troubleshooting em API_USAGE.md](API_USAGE.md#-troubleshooting)

## 📂 Estrutura de Arquivos de Código

```
aether-core/api/
├── common/                  # DTOs e estruturas compartilhadas
│   ├── HttpMethod.hpp      # Enum com GET, POST, etc
│   ├── HttpRequest.hpp     # Struct de requisição
│   └── HttpResponse.hpp    # Struct de resposta
├── config/
│   └── ApiConfig.hpp       # Configuração (host, porta)
├── controllers/             # Processam requisições HTTP (MVC)
│   ├── core/                # Sem módulo específico
│   │   └── StatusController.*
│   └── modules/<Nome>/      # Por módulo (ex: modules/Horus/)
│       └── CameraController.*
├── services/                # Lógica de negócio
│   ├── core/
│   │   └── StatusService.*
│   └── modules/<Nome>/
│       └── CameraService.*
├── dto/                     # Data Transfer Objects
│   ├── core/
│   │   └── StatusResponse.hpp
│   └── modules/<Nome>/
│       └── CameraSnapshotResponse.hpp
└── transport/rest/          # Implementação HTTP
    ├── HttpServer.*         # Servidor TCP (async, pool de threads)
    ├── HttpSession.*        # Gerencia conexão (async_read/async_write)
    ├── AccessLogger.*       # Access log estilo Apache + arquivo de detalhe por requisição
    ├── Router.*             # Despachante de rotas
    ├── RouteRegistry.*      # Registro de rotas (existe, mas não é usado pelo Router ainda)
    ├── RouterGet.cpp        # Handlers GET
    ├── RouterPost.cpp       # Handlers POST
    ├── RouterPut.cpp        # Handlers PUT
    └── RouterDelete.cpp     # Handlers DELETE
```

## 🔧 Configuração Rápida

### Alterar host e porta

Edite `aether-core/api/config/ApiConfig.hpp`:

```cpp
struct ApiConfig {
    std::string host = "0.0.0.0";  // Mudar aqui
    uint16_t port = 9001;           // Ou aqui
};
```

### Adicionar dependência Boost ao CMake

Se ainda não está em `aether-core/api/CMakeLists.txt`:

```cmake
target_link_libraries(aether_api
    PUBLIC
    aether_core
    Boost::system
    Boost::beast
)
```

## 📊 Diagrama de Componentes

```
┌─────────────────────────────────────────────┐
│           Cliente HTTP (cURL, browser)      │
└──────────────────┬──────────────────────────┘
                   │ TCP Port 9001
┌──────────────────▼──────────────────────────┐
│         HttpServer (accept())               │
│    Escuta conexões TCP e cria sessões      │
└──────────────────┬──────────────────────────┘
                   │
┌──────────────────▼──────────────────────────┐
│       HttpSession (per connection)          │
│  Lê HTTP com Beast, converte para interno   │
└──────────────────┬──────────────────────────┘
                   │
         ┌─────────▼──────────┐
         │ Router::dispatch()│
         │ (Method routing)   │
         └─────────┬──────────┘
                   │
    ┌──────────────┼──────────────┐
    │              │              │
┌───▼───┐      ┌───▼───┐      ┌──▼────┐
│ GET   │      │ POST  │      │ PUT   │
└───┬───┘      └───┬───┘      └──┬────┘
    │              │             │
┌───▼──────────────┴─────────────▼──┐
│ dispatchGet/Post/Put/Delete()     │
│ (Path matching)                   │
└───┬──────────────────────────────┘
    │
┌───▼──────────────────────────────┐
│     Controllers                  │
│ StatusController, UserController │
└───┬──────────────────────────────┘
    │
┌───▼──────────────────────────────┐
│     Services                     │
│ StatusService, UserService       │
└───┬──────────────────────────────┘
    │
┌───▼──────────────────────────────┐
│    HttpResponse                  │
│ (status, body, headers)          │
└───┬──────────────────────────────┘
    │
┌───▼──────────────────────────────┐
│ HttpSession::createResponse()    │
│ Converte para Beast              │
└───┬──────────────────────────────┘
    │
┌───▼──────────────────────────────┐
│      http::write() ao socket     │
└───┬──────────────────────────────┘
    │ HTTP Response
┌───▼──────────────────────────────┐
│     Cliente recebe resposta      │
└──────────────────────────────────┘
```

## 🛠️ Stack Tecnológico

- **C++ 17+**: Linguagem base
- **Boost.ASIO**: Rede assíncrona
- **Boost.Beast**: HTTP parsing
- **CMake**: Build system
- **HTTP/1.1**: Protocolo

## ✅ Checklist de Desenvolvimento

- [ ] Li o [Guia de Uso](API_USAGE.md)
- [ ] Li a [Arquitetura](API_ARCHITECTURE.md)
- [ ] Testei com cURL: `curl http://localhost:9001/api/test`
- [ ] Entendi o fluxo MVC
- [ ] Criei meu primeiro Controller
- [ ] Registrei uma rota no Router
- [ ] Testei minha rota com cURL

## 🐛 Problemas Comuns

| Problema | Solução |
|----------|---------|
| "Connection refused" | Verifique se servidor está rodando: `netstat -an \| grep 9001` |
| "404 Not Found" | Rota não está registrada, verifique caminho em Router |
| Resposta vazia | Verifique se `response.body` foi preenchido no controller |
| Build failing | Verifique se Boost está instalado: `find /usr -name "boost"` |

Mais detalhes em [Troubleshooting](API_USAGE.mdtroubleshooting).

## 📞 Suporte

- **Type-checking**: Use uma IDE com suporte C++ (VsCode, CLion)
- **Testing**: Use gtest com as chamadas de exemplo em [API_CODE_REFERENCE.md](API_CODE_REFERENCE.mdtesting)
- **Debugging**: Use gdb ou debugger da IDE

## 📈 Roadmap Futuro

- [ ] Suporte a path parameters dinâmicos (ex: /users/:id)
- [ ] Middleware (logging, auth, compression)
- [ ] Async I/O com coroutines C++20
- [ ] WebSocket support
- [ ] Rate limiting
- [ ] CORS automático

## 📜 Licença

Parte do projeto Aether. Veja LICENSE.md

---

**Última atualização:** 2026-07-18  
**Versão da Documentação:** 1.0

