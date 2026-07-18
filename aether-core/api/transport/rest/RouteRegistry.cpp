#include "RouteRegistry.hpp"

namespace Aether::Api
{
    /**
     * Registra handler para rota GET
     */
    void RouteRegistry::get(const std::string& path, RouteHandler handler)
    {
        m_getRoutes[path] = handler;
    }

    /**
     * Registra handler para rota POST
     */
    void RouteRegistry::post(const std::string& path, RouteHandler handler)
    {
        m_postRoutes[path] = handler;
    }

    /**
     * Registra handler para rota PUT
     */
    void RouteRegistry::put(const std::string& path, RouteHandler handler)
    {
        m_putRoutes[path] = handler;
    }

    /**
     * Registra handler para rota PATCH
     */
    void RouteRegistry::patch(const std::string& path, RouteHandler handler)
    {
        m_patchRoutes[path] = handler;
    }

    /**
     * Registra handler para rota DELETE
     */
    void RouteRegistry::delete_(const std::string& path, RouteHandler handler)
    {
        m_deleteRoutes[path] = handler;
    }

    /**
     * Procura uma rota registrada por método e path
     *
     * Retorna ponteiro para o handler se encontrado, nullptr caso contrário.
     * O ponteiro é válido enquanto o RouteRegistry não for modificado.
     *
     * Algoritmo:
     * 1. Seleciona mapa apropriado baseado no método HTTP
     * 2. Busca path no mapa (O(log n))
     * 3. Retorna ponteiro para handler ou nullptr
     */
    RouteHandler* RouteRegistry::find(HttpMethod method, const std::string& path)
    {
        std::map<std::string, RouteHandler>* routes = nullptr;

        // Seleciona o mapa de rotas baseado no método
        switch (method)
        {
            case HttpMethod::GET:
                routes = &m_getRoutes;
                break;
            case HttpMethod::POST:
                routes = &m_postRoutes;
                break;
            case HttpMethod::PUT:
                routes = &m_putRoutes;
                break;
            case HttpMethod::PATCH:
                routes = &m_patchRoutes;
                break;
            case HttpMethod::DELETE_:
                routes = &m_deleteRoutes;
                break;
            default:
                return nullptr;
        }

        // Busca no mapa
        auto it = routes->find(path);
        if (it != routes->end())
        {
            // Encontrou - retorna ponteiro para o handler
            return &it->second;
        }

        // Não encontrou
        return nullptr;
    }
}

