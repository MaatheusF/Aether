#pragma once

#include "../../common/HttpResponse.hpp"
#include "../../common/HttpRequest.hpp"
#include "../../controllers/StatusController.hpp"

#include <string>

namespace Aether::Api
{
    /**
     * @brief Router HTTP - Roteador de requisições
     *
     * Responsável por rotear requisições HTTP para os controllers
     * apropriados baseado no método HTTP (GET, POST, PUT, DELETE)
     * e no caminho.
     *
     * O roteamento atualmente é feito internamente nos métodos
     * especializado que verificam paths específicos. Para sistemas
     * mais complexos, considere usar RouteRegistry.
     *
     * @see HttpSession - quem chama dispatch()
     * @see StatusController - exemplo de controller
     *
     * @example
     * @code
     *   Router router;
     *   HttpRequest request{HttpMethod::GET, "/api/status", ...};
     *   HttpResponse response = router.dispatch(request);
     * @endcode
     */
    class Router
    {
        public:
            /**
             * @brief Roteador principal de requisições
             *
             * Analisa o método HTTP e delega para o despachante apropriado.
             *
             * @param request Requisição HTTP a processar
             * @return Resposta HTTP apropriada
             */
            HttpResponse dispatch(const HttpRequest& request);

        private:
            /**
             * @brief Processa requisições GET
             * @param request Requisição GET recebida
             * @return Resposta HTTP
             */
            HttpResponse dispatchGet(const HttpRequest& request);

            /**
             * @brief Processa requisições POST
             * @param request Requisição POST recebida
             * @return Resposta HTTP
             */
            HttpResponse dispatchPost(const HttpRequest& request);

            /**
             * @brief Processa requisições PUT
             * @param request Requisição PUT recebida
             * @return Resposta HTTP
             */
            HttpResponse dispatchPut(const HttpRequest& request);

            /**
             * @brief Processa requisições DELETE
             * @param request Requisição DELETE recebida
             * @return Resposta HTTP
             */
            HttpResponse dispatchDelete(const HttpRequest& request);

            /**
             * @brief Resposta padrão para rotas não encontradas
             * @return Resposta 404 Not Found em JSON
             */
            HttpResponse notFound() const;

        private:
            StatusController m_statusController;  /**< Controller de status */
    };
}