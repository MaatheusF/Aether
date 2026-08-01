#include "AccessLogger.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace Aether::Api
{
    namespace
    {
        std::ofstream sLogFile;
        std::mutex sMutex;
        std::string sDetailsDir;
        std::unordered_set<HttpMethod> sLoggedMethods;
        std::unordered_set<HttpMethod> sDetailedMethods;

        // Desambigua nomes de arquivo quando duas requisições caem no mesmo
        // milissegundo -- possível agora que a API é multi-thread.
        std::atomic<std::uint64_t> sSequence{0};

        std::string formatHeaders(const std::unordered_map<std::string, std::string>& headers)
        {
            std::ostringstream oss;

            for (const auto& [name, value] : headers)
                oss << name << ": " << value << "\n";

            return oss.str();
        }

        /**
         * Headers HTTP são case-insensitive, mas o unordered_map guarda a
         * chave crua como chegou -- então "Content-Type" e "content-type"
         * são chaves diferentes ali. Busca ignorando caixa.
         */
        std::string findHeaderCaseInsensitive(
            const std::unordered_map<std::string, std::string>& headers,
            const std::string& key)
        {
            for (const auto& [name, value] : headers)
            {
                if (name.size() != key.size())
                    continue;

                if (std::equal(name.begin(), name.end(), key.begin(), [](char a, char b)
                    {
                        return std::tolower(static_cast<unsigned char>(a)) ==
                               std::tolower(static_cast<unsigned char>(b));
                    }))
                {
                    return value;
                }
            }

            return "";
        }

        bool isImageContentType(const std::unordered_map<std::string, std::string>& headers)
        {
            const std::string contentType = findHeaderCaseInsensitive(headers, "Content-Type");
            return contentType.rfind("image/", 0) == 0;
        }

        /**
         * Corpo da resposta pra gravar no arquivo de detalhe. Snapshots de
         * câmera (image/*) chegam a 1 por segundo por câmera -- gravar o
         * JPEG inteiro em cada arquivo de detalhe geraria GBs de log em
         * pouco tempo sem nenhuma utilidade prática (ninguém abre um .log
         * pra ver a imagem). Nesses casos só anota o tamanho, sem o corpo.
         */
        std::string responseBodyForDetailFile(const HttpResponse& response)
        {
            if (isImageContentType(response.headers))
            {
                return "<corpo omitido -- imagem, " + std::to_string(response.body.size()) + " bytes>";
            }

            return response.body;
        }
    }

    void AccessLogger::Initialize(
        const std::string& accessLogPath,
        const std::string& requestDetailsDir,
        std::unordered_set<HttpMethod> loggedMethods,
        std::unordered_set<HttpMethod> detailedMethods)
    {
        std::lock_guard<std::mutex> lock(sMutex);

        sDetailsDir = requestDetailsDir;
        sLoggedMethods = std::move(loggedMethods);
        sDetailedMethods = std::move(detailedMethods);

        std::error_code dirError;
        std::filesystem::create_directories(sDetailsDir, dirError);
        if (dirError)
        {
            std::cerr << "[AccessLogger] Falha ao criar diretorio de detalhes: "
                      << sDetailsDir << " (" << dirError.message() << ")" << std::endl;
        }

        sLogFile.open(accessLogPath, std::ios::out | std::ios::app);
        if (!sLogFile.is_open())
        {
            std::cerr << "[AccessLogger] Falha ao abrir arquivo de log de acesso: "
                      << accessLogPath << std::endl;
        }
    }

    std::string AccessLogger::currentTimestamp()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

        std::tm localTm{};
        localtime_r(&timeNow, &localTm);

        std::ostringstream oss;
        oss << std::put_time(&localTm, "%d/%b/%Y:%H:%M:%S %z");
        return oss.str();
    }

    std::string AccessLogger::methodToString(HttpMethod method)
    {
        switch (method)
        {
            case HttpMethod::GET:     return "GET";
            case HttpMethod::POST:    return "POST";
            case HttpMethod::PUT:     return "PUT";
            case HttpMethod::PATCH:   return "PATCH";
            case HttpMethod::DELETE_: return "DELETE";
            default:                  return "UNKNOWN";
        }
    }

    std::string AccessLogger::buildDetailFilename()
    {
        const auto now = std::chrono::system_clock::now();
        const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        const std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

        std::tm localTm{};
        localtime_r(&timeNow, &localTm);

        const auto sequence = sSequence.fetch_add(1, std::memory_order_relaxed);

        std::ostringstream oss;
        oss << std::put_time(&localTm, "%Y%m%d-%H%M%S")
            << "-" << std::setfill('0') << std::setw(3) << nowMs.count()
            << "-" << std::setfill('0') << std::setw(6) << sequence
            << ".log";

        return oss.str();
    }

    void AccessLogger::writeDetailFile(
        const std::string& filename,
        const std::string& clientIp,
        const HttpRequest& request,
        const HttpResponse& response,
        std::chrono::milliseconds duration)
    {
        const std::filesystem::path fullPath = std::filesystem::path(sDetailsDir) / filename;

        // Modo binário: corpo pode ser um JPEG (snapshot de câmera) -- não
        // queremos nenhuma tradução de fim de linha mexendo nos bytes.
        std::ofstream detailFile(fullPath, std::ios::out | std::ios::binary);

        if (!detailFile.is_open())
        {
            std::cerr << "[AccessLogger] Falha ao criar arquivo de detalhe: "
                      << fullPath << std::endl;
            return;
        }

        detailFile
            << "IP: " << clientIp << "\n"
            << "Data: " << currentTimestamp() << "\n"
            << "Duracao: " << duration.count() << "ms\n"
            << "\n"
            << "=== REQUEST ===\n"
            << methodToString(request.method) << " " << request.path << " HTTP/1.1\n"
            << formatHeaders(request.headers)
            << "\n"
            << request.body
            << "\n\n"
            << "=== RESPONSE ===\n"
            << "Status: " << response.status << "\n"
            << formatHeaders(response.headers)
            << "\n"
            << responseBodyForDetailFile(response);
    }

    void AccessLogger::Log(
        const std::string& clientIp,
        const HttpRequest& request,
        const HttpResponse& response,
        std::chrono::milliseconds duration)
    {
        if (!sLoggedMethods.count(request.method))
            return;

        std::string detailFilename = "-";

        if (sDetailedMethods.count(request.method))
        {
            detailFilename = buildDetailFilename();
            writeDetailFile(detailFilename, clientIp, request, response, duration);
        }

        std::lock_guard<std::mutex> lock(sMutex);

        if (!sLogFile.is_open())
            return;

        sLogFile
            << clientIp << " - - "
            << "[" << currentTimestamp() << "] "
            << "\"" << methodToString(request.method) << " " << request.path << " HTTP/1.1\" "
            << response.status << " "
            << response.body.size() << " "
            << duration.count() << "ms "
            << "detail=" << detailFilename
            << std::endl;
    }
}
