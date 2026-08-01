#pragma once

#include "../../common/HttpMethod.hpp"
#include "../../common/HttpRequest.hpp"
#include "../../common/HttpResponse.hpp"

#include <chrono>
#include <string>
#include <unordered_set>

namespace Aether::Api
{
    /**
     * @brief Log de acesso da API HTTP, no estilo access.log do Apache.
     *
     * Cada requisição gera:
     * 1. Uma linha resumo no arquivo de access log (IP, timestamp, método,
     *    rota, status, tamanho da resposta, duração) -- fácil de ler/grep,
     *    igual ao access.log do Apache.
     * 2. Um arquivo próprio no "repositório" de detalhes (um arquivo por
     *    requisição), com o payload da requisição e o corpo da resposta
     *    completos e crus. Exceção: respostas de imagem (Content-Type
     *    image/*, ex: snapshot de câmera) não têm o corpo gravado -- só um
     *    placeholder com o tamanho -- já que essas requisições se repetem
     *    a cada poucos segundos por câmera e gravar o JPEG inteiro em cada
     *    arquivo geraria gigabytes de log sem utilidade prática. A linha
     *    resumo referencia o arquivo de detalhe pelo nome, pra
     *    correlacionar as duas pontas.
     *
     * Escreve em arquivos próprios, separados do log operacional do
     * AetherCoreLogger (assim como o Apache separa access.log de
     * error.log). Protegido por mutex internamente: a API roda
     * multi-thread (ver HttpServer), então requisições concorrentes podem
     * chamar Log() ao mesmo tempo.
     *
     * Métodos fora de ApiConfig::loggedMethods são ignorados por completo
     * (nem linha resumo, nem arquivo de detalhe). Métodos em loggedMethods
     * mas fora de ApiConfig::detailedMethods geram a linha resumo
     * normalmente, só sem arquivo de detalhe (campo `detail=-`) -- ver
     * Initialize().
     */
    class AccessLogger
    {
    public:
        /**
         * @brief Abre o access log e garante que o diretório de detalhes existe.
         *
         * @param accessLogPath Caminho do arquivo de access log (linha resumo).
         * @param requestDetailsDir Diretório onde cada requisição grava seu
         *        próprio arquivo com payload/response completos.
         * @param loggedMethods Métodos HTTP que devem ser logados (linha
         *        resumo); qualquer método fora desse conjunto é
         *        silenciosamente ignorado por completo.
         * @param detailedMethods Dentre os métodos logados, quais também
         *        geram arquivo de detalhe. Método fora desse conjunto (mas
         *        dentro de loggedMethods) só gera a linha resumo.
         */
        static void Initialize(
            const std::string& accessLogPath,
            const std::string& requestDetailsDir,
            std::unordered_set<HttpMethod> loggedMethods,
            std::unordered_set<HttpMethod> detailedMethods);

        /**
         * @brief Registra uma requisição processada (linha resumo + arquivo de detalhe).
         *
         * @param clientIp Endereço IP do cliente (do socket TCP).
         * @param request Requisição já processada.
         * @param response Resposta que foi enviada ao cliente.
         * @param duration Tempo gasto processando a requisição (Router::dispatch).
         */
        static void Log(
            const std::string& clientIp,
            const HttpRequest& request,
            const HttpResponse& response,
            std::chrono::milliseconds duration);

    private:
        /** @brief Nome único e ordenável cronologicamente pro arquivo de detalhe desta requisição. */
        static std::string buildDetailFilename();

        /** @brief Grava o arquivo individual com o transcript completo da requisição/resposta. */
        static void writeDetailFile(
            const std::string& filename,
            const std::string& clientIp,
            const HttpRequest& request,
            const HttpResponse& response,
            std::chrono::milliseconds duration);

        static std::string methodToString(HttpMethod method);

        static std::string currentTimestamp();
    };
}
