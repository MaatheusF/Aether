#pragma once

#include "../../common/HttpRequest.hpp"
#include "../../common/HttpResponse.hpp"
#include <functional>
#include <map>
#include <memory>

namespace Aether::Api
{
    /**
     * @brief Tipo de callback para handlers de rota
     *
     * Um handler de rota recebe uma requisição HTTP e retorna uma resposta.
     *
     * @example
     * @code
     *   RouteHandler handler = [&](const HttpRequest& req) {
     *       HttpResponse response;
     *       response.status = 200;
     *       response.body = "Hello";
     *       return response;
     *   };
     * @endcode
     */
    using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

    /**
     * @brief Registro de rotas HTTP
     *
     * Permite adicionar handlers para métodos HTTP/paths.
     *
     * Uso recomendado:
     * - Crie um único RouteRegistry na aplicação
     * - Configure todas as rotas no inicialização
     * - Passe para Router para processar requisições
     *
     * @example
     * @code
     *   // Criar registry e registrar rotas
     *   auto registry = std::make_shared<RouteRegistry>();
     *
     *   registry->get("/api/status", [&](const HttpRequest& req) {
     *       return statusController.get(req);
     *   });
     *
     *   registry->post("/api/users", [&](const HttpRequest& req) {
     *       return userController.create(req);
     *   });
     *
     *   registry->put("/api/users/:id", [&](const HttpRequest& req) {
     *       return userController.update(req);
     *   });
     *
     *   registry->delete_("/api/users/:id", [&](const HttpRequest& req) {
     *       return userController.delete(req);
     *   });
     *
     *   // Usar no router
     *   auto handler = registry->find(HttpMethod::GET, "/api/status");
     *   if (handler) {
     *       response = (*handler)(request);
     *   }
     * @endcode
     *
     * @note Para suporte a path parameters (ex: /users/:id),
     *       estender para fazer pattern matching
     *
     * @see Router - consome rotas do registry
     */
    class RouteRegistry
    {
    public:
        /**
         * @brief Registra uma rota GET
         *
         * @param path Caminho da rota (ex: "/api/users" ou "/api/users/:id")
         * @param handler Função que processa a requisição e retorna resposta
         *
         * @example
         * @code
         *   registry.get("/api/status", [](const HttpRequest& req) {
         *       HttpResponse res;
         *       res.status = 200;
         *       res.body = "OK";
         *       return res;
         *   });
         * @endcode
         */
        void get(const std::string& path, RouteHandler handler);

        /**
         * @brief Registra uma rota POST
         *
         * @param path Caminho da rota
         * @param handler Função que processa a requisição
         */
        void post(const std::string& path, RouteHandler handler);

        /**
         * @brief Registra uma rota PUT
         *
         * @param path Caminho da rota
         * @param handler Função que processa a requisição
         */
        void put(const std::string& path, RouteHandler handler);

        /**
         * @brief Registra uma rota PATCH
         *
         * @param path Caminho da rota
         * @param handler Função que processa a requisição
         */
        void patch(const std::string& path, RouteHandler handler);

        /**
         * @brief Registra uma rota DELETE
         *
         * @param path Caminho da rota
         * @param handler Função que processa a requisição
         */
        void delete_(const std::string& path, RouteHandler handler);

        /**
         * @brief Procura uma rota registrada
         *
         * Busca por handler registrado para o método HTTP e path específicos.
         *
         * @param method Método HTTP (GET, POST, etc)
         * @param path Caminho da rota a procurar
         *
         * @return Ponteiro para RouteHandler se encontrado, nullptr caso contrário
         *
         * @example
         * @code
         *   auto handler = registry->find(HttpMethod::GET, "/api/users");
         *   if (handler) {
         *       auto response = (*handler)(request);
         *   } else {
         *       // Rota não encontrada
         *   }
         * @endcode
         */
        RouteHandler* find(HttpMethod method, const std::string& path);

    private:
        std::map<std::string, RouteHandler> m_getRoutes;       /**< Mapa de rotas GET */
        std::map<std::string, RouteHandler> m_postRoutes;      /**< Mapa de rotas POST */
        std::map<std::string, RouteHandler> m_putRoutes;       /**< Mapa de rotas PUT */
        std::map<std::string, RouteHandler> m_patchRoutes;     /**< Mapa de rotas PATCH */
        std::map<std::string, RouteHandler> m_deleteRoutes;    /**< Mapa de rotas DELETE */
    };
}

