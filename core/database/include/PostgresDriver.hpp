#pragma once
#include <string>
#include <libpq-fe.h>

/**
 * @class PostgresDriver
 * @brief Classe responsável por gerenciar a conexão e execução de comandos
 *        no banco de dados PostgreSQL
 */
class PostgresDriver
{
public:
    /**
     * @brief Construtor do driver.
     * @param connString String de conexão do PostgreSQL no formato:
     * "host=... port=... dbname=... user=... password=..."
     */
    explicit PostgresDriver(const std::string& connString);

    /**
     * @brief Destrutor do driver.
     */
    ~PostgresDriver();

    /**
     * @brief Estabelece conexão com o banco de dados.
     * @return true se conectou com sucesso.
     * @return false se houve falha ao conectar.
     * Em caso de falha, detalhes do erro podem ser recuperados usando
     * PQerrorMessage().
     */
    bool connect();

    /**
     * @brief Encerra a conexão ativa com o PostgreSQL.
     */
    void disconnect();

    /**
     * @brief Verifica se a conexão está ativa e funcional.
     * @return true se conectado e sem erros.
     * @return false caso contrário.
     */
    bool isConnected() const;

    /**
     * @brief Executa uma consulta SQL no banco de dados.
     * @param sql Comando SQL a ser executado.
     * @return Ponteiro para PGresult contendo o resultado da query,
     *         ou nullptr em caso de erro.
     *
     * Importante:
     *  - O chamador **deve liberar** o resultado usando freeResult().
     */
    PGresult* query(const std::string& sql) const;

    /**
     * @brief Libera a memória associada a um PGresult.
     * @param res Ponteiro retornado por query().
     */
    static void freeResult(PGresult* res);

    /**
     * @brief Getter para retornar um elemento Pgconn* para usar nas conexões
     */
    PGconn* get() const
    {
        return m_conn;
    }

    /**
     * @brief Realiza uma consulta parametrizada usando PQexecParams.
     * @param sql Comando SQL com placeholders ($1, $2, etc.).
     * @param nParams Número de parâmetros a serem substituídos.
     * @param paramValues Array de strings contendo os valores dos parâmetros.
     * @return Ponteiro para PGresult contendo o resultado da query,
     */
    PGresult* queryParams(const std::string& sql, int nParams, const char* const* paramValues) const
    {
        return PQexecParams(
            m_conn,
            sql.c_str(),
            nParams,
            nullptr,
            paramValues,
            nullptr,
            nullptr,
            0
        );
    }

private:
    /**
     * @brief Conexão nativa com o PostgreSQL (libpq).
     */
    PGconn* m_conn;
    /**
     * @brief String contendo os parâmetros de conexão.
     */
    std::string m_connString;
};