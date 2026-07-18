#include "../include/PostgresDriver.hpp"

#include <iostream>

/**
 * @brief Construtor do PostgresDriver.
 * Armazena a string de conexão e inicializa o ponteiro
 * para conexão como nullptr. Abre a conexão com connect()
 *
 * @param connString String contendo os parâmetros de conexão
 * "host=... port=... dbname=... user=... password=..."
 */
PostgresDriver::PostgresDriver(const std::string& connString) : m_conn(nullptr), m_connString(connString)
{
    connect();
}

/**
 * @brief Destrutor da classe.
 */
PostgresDriver::~PostgresDriver() {
    disconnect();
}

/**
 * @brief Inicia conexão com o banco PostgreSQL.
 * @return true se a conexão foi estabelecida com sucesso.
 * @return false se ocorreu algum erro.
 */
bool PostgresDriver::connect()
{
    //Abre conexão com o banco de dados
    m_conn = PQconnectdb(m_connString.c_str());

    //Verifica se a conexão foi bem sucedida
    if (PQstatus(m_conn) != CONNECTION_OK)
    {
        std::cerr << " [Core Database] Falha ao se conectar ao banco de dados! " << PQerrorMessage(m_conn) << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Encerra a conexão ativa com o banco.
 */
void PostgresDriver::disconnect()
{
    //Verifica se a conexão esta ativa antes de desconectar
    if (m_conn)
    {
        PQfinish(m_conn); //Finaliza a conexão
        m_conn = nullptr; //Limpa a variavel de conexão
    }
}

/**
 * @brief Verifica se há uma conexão ativa.
 * @return true se a conexão está ativa.
 * @return false se está desconectado ou em estado inválido.
 */
bool PostgresDriver::isConnected() const
{
    return m_conn && PQstatus(m_conn) == CONNECTION_OK;
}

/**
 * @brief Executa um comando SQL
 * @param sql String contendo o comando SQL a ser executado.
 * @return PGresult* contendo o resultado ou nullptr se houver erro.
 */
PGresult* PostgresDriver::query(const std::string& sql) const
{
    PGresult* res = PQexec(m_conn, sql.c_str()); //Executa o SQL no banco de dados

    //Verifica se ocorreu algum erro na transação com o banco de dados
    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << " [Core Database] Falha ao executar SQL: " << PQerrorMessage(m_conn) << std::endl;
        PQclear(res); //Limpa a memoria da variavel de retorno
        return nullptr;
    }
    return res;
}

/**
 * @brief Libera a memória associada a um PGresult.
 * @param res Ponteiro retornado por query().
 */
void PostgresDriver::freeResult(PGresult* res)
{
    PQclear(res);
}
