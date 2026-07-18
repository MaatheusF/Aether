# GET /api/status

## Descrição

Retorna o estado atual do Aether Core/API. Método utilizado para verificar a saude do sistema e monitorar os seus parametros de funcionamento.

## Request

GET /api/status

## Response

HTTP 200

```json
{
    "status": "UP",
    "timestamp": "2026-07-18T19:23:32Z"
}
```

## Campos

| Campo | Tipo | Descrição | Observações                                                                   |
|-------|------|-----------|-------------------------------------------------------------------------------|
| status | string | Estado do Core | Retorna "UP" quando a API estiver funcional e "Down" quando estiver com falha |
| timestamp | string | Data/hora UTC em ISO 8601 | Retorna a data/hora do retorno da requisição em Zulu                          |