#include "CameraController.hpp"
#include "../../include/external/json.hpp"

namespace Aether::Api
{
    namespace
    {
        constexpr const char* kCamerasPrefix = "/api/cameras/";
        constexpr const char* kSnapshotSuffix = "snapshot";

        /**
         * Monta uma resposta de erro em JSON, no mesmo formato usado pelo
         * restante da API (ex: Router::notFound()).
         */
        HttpResponse buildErrorResponse(int status, const std::string& message)
        {
            nlohmann::json j;
            j["success"] = false;
            j["message"] = message;

            HttpResponse response;
            response.status = status;
            response.body = j.dump();
            response.headers["Content-Type"] = "application/json";
            response.headers["Access-Control-Allow-Origin"] = "*";

            return response;
        }
    }

    /**
     * Extrai o canal do path "/api/cameras/:channel/snapshot".
     * Tolera query string ao final (ex: "?t=123") para uso futuro
     * com cache-buster.
     */
    int CameraController::parseChannelFromPath(const std::string& path)
    {
        const std::string prefix = kCamerasPrefix;

        if (path.compare(0, prefix.size(), prefix) != 0)
            return -1;

        const std::string rest = path.substr(prefix.size());

        const auto slashPos = rest.find('/');
        if (slashPos == std::string::npos)
            return -1;

        const std::string channelStr = rest.substr(0, slashPos);

        std::string suffix = rest.substr(slashPos + 1);
        const auto queryPos = suffix.find('?');
        if (queryPos != std::string::npos)
            suffix = suffix.substr(0, queryPos);

        if (suffix != kSnapshotSuffix)
            return -1;

        try
        {
            return std::stoi(channelStr);
        }
        catch (const std::exception&)
        {
            return -1;
        }
    }

    /**
     * Processa requisição GET para /api/cameras/:channel/snapshot
     *
     * Delega ao CameraService a captura da imagem (Digest Auth) e
     * retorna o JPEG bruto, ou um erro em JSON quando a captura falha.
     */
    HttpResponse CameraController::getSnapshot(const HttpRequest& request)
    {
        const int channel = parseChannelFromPath(request.path);

        if (channel < 0)
        {
            return buildErrorResponse(400, "Canal invalido. Use /api/cameras/:channel/snapshot");
        }

        auto dto = m_service.getSnapshot(channel);

        if (!dto.success)
        {
            // 502 Bad Gateway: a API esta ok, quem falhou foi o upstream (a camera)
            return buildErrorResponse(502,
                dto.message.empty() ? "Falha ao obter snapshot da camera" : dto.message);
        }

        HttpResponse response;
        response.status = 200;
        response.body.assign(dto.data.begin(), dto.data.end());
        response.headers["Content-Type"] = dto.contentType;
        response.headers["Cache-Control"] = "no-store, no-cache, must-revalidate";
        response.headers["Access-Control-Allow-Origin"] = "*";

        return response;
    }
}
