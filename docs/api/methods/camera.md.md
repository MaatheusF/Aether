# Rota de snapshot de câmera — GET /api/horus/cameras/:channel/snapshot

## O que foi criado

Seguindo exatamente o padrão MVC do projeto (Controller → Service → DTO):

| Arquivo | Papel |
|---|---|
| `core/utils/Md5.hpp` / `.cpp` | MD5 próprio (sem dependência nova), usado no cálculo do Digest Auth |
| `api/config/CameraConfig.hpp` | Host, porta, usuário, senha e path do snapshot da câmera |
| `api/dto/modules/Horus/CameraSnapshotResponse.hpp` | DTO com os bytes da imagem, content-type e status |
| `api/services/modules/Horus/CameraService.hpp` / `.cpp` | Faz o handshake HTTP Digest Auth e busca o JPEG na câmera |
| `api/controllers/modules/Horus/CameraController.hpp` / `.cpp` | Extrai o canal da URL e devolve a imagem (ou erro em JSON) |
| `api/transport/rest/Router.hpp` (editado) | Adiciona `m_cameraController` |
| `api/transport/rest/RouterGet.cpp` (editado) | Registra a rota `GET /api/horus/cameras/:channel/snapshot` |
| `core/CMakeLists.txt` (editado) | Adiciona `Md5.cpp`/`Md5.hpp` na lib `aether_core` |

> `CameraController`/`CameraService`/`CameraSnapshotResponse` foram movidos pra
> dentro de `modules/Horus/` numa refatoração posterior, que separou `controllers/`,
> `services/` e `dto/` em `core/` (sem módulo) vs `modules/<Nome>/` (por módulo).
> Ver `API_ARCHITECTURE.md` para a convenção completa.

Os arquivos `.cpp` novos dentro de `api/` (`CameraService.cpp`, `CameraController.cpp`)
já são pegos automaticamente pelo `file(GLOB_RECURSE ...)` do `api/CMakeLists.txt` —
**não precisa editar esse arquivo**, só re-rodar o `cmake` (o glob é resolvido no
configure, não no build).

## Como funciona o Digest Auth

1ª requisição GET sem `Authorization` → câmera responde **401** com o header
`WWW-Authenticate: Digest realm="...", nonce="...", qop="auth", ...`

O `CameraService` extrai `realm`/`nonce`/`qop`/`opaque`, calcula:
```
HA1 = MD5(user:realm:password)
HA2 = MD5(GET:uri)
response = MD5(HA1:nonce:nc:cnonce:qop:HA2)
```
e reenvia a requisição com `Authorization: Digest ...`. A câmera responde
**200** com o JPEG, que é devolvido no `body` da `HttpResponse` (binário-seguro,
já que `HttpResponse::body` é `std::string` e o `HttpSession` não faz nenhum
parsing de texto em cima dele — só grava os bytes crus no socket).

## Configuração

Por padrão (`CameraConfig.hpp`):
```cpp
host     = "192.168.0.110";
port     = 80;
user     = "admin";
password = "favero10";
cgiPath  = "/cgi-bin/snapshot.cgi?channel=";
```
Isso replica os dados que você passou. Vale considerar mover `user`/`password`
para variável de ambiente ou arquivo de config fora do versionamento antes de
subir isso pra produção — deixei hardcoded só pra manter o mesmo padrão do
`ApiConfig` atual (host/porta também hardcoded lá).

Se um dia você tiver mais de uma câmera, dá pra evoluir `CameraConfig` para um
`std::vector<CameraConfig>` indexado por câmera, sem mexer no resto.

## Testando

```bash
# depois de rebuildar e subir o aetherd
curl http://localhost:9001/api/horus/cameras/1/snapshot --output snapshot.jpg
file snapshot.jpg   # deve dizer "JPEG image data"
```

Erro esperado se a câmera estiver fora do ar ou credenciais erradas:
```json
{"success":false,"message":"Falha na autenticacao Digest ou na captura do snapshot (status 401)"}
```
com HTTP status 502.

## Front-end

Veja `web/camera-viewer-example.html`. Uso de `fetch(..., { cache: 'no-store' })`
+ `URL.createObjectURL` em vez de só trocar `img.src` com um query param de
cache-buster — assim a imagem sempre vem fresca e não polui o path da rota
(o `Router` faz matching por path, então um `?t=123` ali quebraria o parsing
do canal se fosse feito por concatenação simples).

## Rebuild

```bash
cd cmake-build-debug
cmake ..          # necessário pra o GLOB_RECURSE pegar os novos .cpp
cmake --build . --target aetherd
```
