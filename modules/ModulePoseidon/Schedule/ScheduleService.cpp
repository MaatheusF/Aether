#include "ScheduleService.hpp"
#include "../../../core/database/include/ConnectionPool.hpp"
#include "../config/DatabaseConfig.hpp"
#include "../../../core/network/SessionManager.hpp"
#include "../../../protocols/aether/include/CommandType.hpp"
#include "../../../protocols/aether/common/ModuleId.hpp"
#include "../../../protocols/aether/include/PacketBuilder.hpp"
#include "../../../include/external/croncpp.h"
#include <sstream>
#include <iomanip>
#include <iostream>

/**
 * @brief Construtor padrão.
 *
 * Inicializa o pool de conexões com o banco de dados configurado,
 * mantendo-o ativo durante o ciclo de vida da instância para
 * evitar recriação custosa a cada ciclo do loop.
 */
PoseidonSchedule::PoseidonSchedule()
    : pool_(std::make_unique<ConnectionPool>(Poseidon::DatabaseConfig::connectionString(), 5))
{
}

/**
 * @brief Destruidor.
 *
 * Garante o encerramento seguro da thread de agendamento
 * ao destruir a instância.
 */
PoseidonSchedule::~PoseidonSchedule()
{
    stop();
}

/**
 * @brief Inicia o serviço de agendamento.
 *
 * Define o estado interno como ativo e cria a thread responsável
 * por executar o loop periódico de verificação dos agendamentos.
 * Não tem efeito caso o serviço já esteja em execução.
 */
void PoseidonSchedule::start()
{
    if (running_)
        return;

    running_ = true;
    threadScheduleService_ = std::thread(&PoseidonSchedule::loop, this);
}

/**
 * @brief Encerra o serviço de agendamento.
 *
 * Sinaliza a thread para que finalize o loop imediatamente,
 * sem aguardar o próximo ciclo de sleep, e aguarda sua
 * finalização de forma segura via join().
 */
void PoseidonSchedule::stop()
{
    {
        std::lock_guard<std::mutex> lock(cvMutex_);
        running_ = false;
    }
    cv_.notify_all();

    if (threadScheduleService_.joinable())
        threadScheduleService_.join();
}

/**
 * @brief Loop principal do serviço de agendamento.
 *
 * Executa continuamente enquanto o serviço estiver ativo.
 * A cada 60 segundos busca os agendamentos ativos no banco de dados
 * e executa os jobs cujo próximo horário de execução já foi atingido.
 * O sleep é interrompível via condition_variable, permitindo shutdown imediato.
 */
void PoseidonSchedule::loop()
{
    while (running_)
    {
        auto conn = pool_->acquire();

        if (!conn) {
            std::cout << "[Poseidon] Falha ao conectar-se ao banco de dados para verificar os agendamentos!\n";
        } else {
            auto now = std::chrono::system_clock::now();

            PGresult* resultDb = conn->query(R"(
                SELECT
                    jcrs.id,
                    jcrs.job_type,
                    jcrs.payload,
                    jcrs.cron_expression,
                    jcrs.cron_schedule_datetime,
                    jcrs.last_run_datetime,
                    jcrs.create_date,
                    devc.device_name,
                    rely.external_id
                FROM poseidon.jcrs_job_cron_schedule jcrs
                    JOIN poseidon.devc_device devc ON (devc.id = device_id)
                    JOIN poseidon.rely_relay rely ON (rely.id = relay_id)
                WHERE jcrs.active = TRUE
            )");

            if (resultDb) {
                int numRows = PQntuples(resultDb);

                for (int i = 0; i < numRows; i++)
                {
                    std::string jobId           = PQgetvalue(resultDb, i, PQfnumber(resultDb, "id"));
                    std::string cronExpression  = PQgetvalue(resultDb, i, PQfnumber(resultDb, "cron_expression"));
                    std::string lastRunDatetime = PQgetvalue(resultDb, i, PQfnumber(resultDb, "last_run_datetime"));
                    std::string createDate      = PQgetvalue(resultDb, i, PQfnumber(resultDb, "create_date"));
                    std::string jobType         = PQgetvalue(resultDb, i, PQfnumber(resultDb, "job_type"));
                    std::string jsonPayload     = PQgetvalue(resultDb, i, PQfnumber(resultDb, "payload"));
                    std::string deviceName      = PQgetvalue(resultDb, i, PQfnumber(resultDb, "device_name"));

                    auto cron = cron::make_cron(cronExpression);
                    auto baseTime = lastRunDatetime.empty()
                        ? stringToTimePoint(createDate)
                        : stringToTimePoint(lastRunDatetime);

                    auto nextRun = cron::cron_next(cron, baseTime);
                    if (nextRun <= now)
                    {
                        if (jobType == "RELAY_CHANGE_STATE")
                        {
                            if (!jobChangeStateRelay(jobId, jsonPayload, deviceName))
                            {
                                std::cout << "[Poseidon] - [Schedule] Falha ao executar o agendamento cód. " << jobId << "\n";
                                continue;
                            }
                        } else {
                            std::cout << "[Poseidon] - [Schedule] Agendamento cód. " << jobId
                                      << " não executado — tipo não implementado: " << jobType << "\n";
                        }
                        jobUpdateDb(jobId);
                    }
                }
                conn->freeResult(resultDb);
            }
        }

        std::unique_lock<std::mutex> lock(cvMutex_);
        cv_.wait_for(lock, std::chrono::seconds(60), [this] { return !running_.load(); });
    }
}

/**
 * @brief Atualiza o banco de dados após a execução de um job.
 *
 * Registra o horário da última execução na tabela de agendamentos
 * e insere um registro de histórico com status SUCCESS.
 *
 * @param jobId Identificador do job executado.
 */
void PoseidonSchedule::jobUpdateDb(const std::string& jobId)
{
    ConnectionPool pool(Poseidon::DatabaseConfig::connectionString(), 1);
    auto conn = pool.acquire();

    if (!conn) {
        std::cout << "[Poseidon] - [Schedule] Falha ao conectar ao banco para atualizar o job cód. " << jobId << "\n";
        return;
    }

    const char* paramValues[1];
    paramValues[0] = jobId.c_str();

    auto res = conn->queryParams(R"(
        UPDATE poseidon.jcrs_job_cron_schedule
        SET last_run_datetime = NOW()
        WHERE id = $1;
    )", 1, paramValues);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
        std::cout << "[Poseidon] ERRO ao atualizar last_run_datetime: " << PQerrorMessage(conn->get()) << "\n";

    PostgresDriver::freeResult(res);

    res = conn->queryParams(R"(
        INSERT INTO poseidon.jcrh_job_cron_history
        (schedule_id, run_at, status, error_message, create_date)
        VALUES ($1, NOW(), 'SUCCESS', NULL, NOW());
    )", 1, paramValues);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
        std::cout << "[Poseidon] ERRO ao inserir histórico do job: " << PQerrorMessage(conn->get()) << "\n";

    PostgresDriver::freeResult(res);
}

/**
 * @brief Envia um comando de alteração de estado para um relay.
 *
 * Localiza a sessão ativa do device pelo nome, constrói o pacote
 * de protocolo Aether com o payload informado e o envia ao device.
 *
 * @param jobId      Identificador do job (usado em logs).
 * @param payloadStr Payload JSON serializado como string.
 * @param deviceName Nome do device de destino.
 * @return true  se o pacote foi enviado com sucesso.
 * @return false se o device não estiver conectado ou identificado.
 */
bool PoseidonSchedule::jobChangeStateRelay(const std::string& jobId, const std::string& payloadStr, const std::string& deviceName)
{
    auto channel = SessionManager::instance().getChannelByDeviceExternalId(deviceName);
    if (!channel) {
        std::cout << "[Poseidon] - [Schedule] device '" << deviceName << "' não conectado ou não identificado\n";
        return false;
    }

    std::vector<uint8_t> payload(payloadStr.begin(), payloadStr.end());

    auto packet = ProtocolAether::PacketBuilder::build(
        CommandType::DATA_PUSH,
        static_cast<uint16_t>(ModuleId::MODULE_POSEIDON),
        payload
    );

    channel->sendResponse(packet);
    std::cout << "[Poseidon] - [Schedule] pacote enviado para device=" << deviceName << " (job=" << jobId << ")\n";

    return true;
}

/**
 * @brief Converte uma string de data/hora para um time_point UTC.
 *
 * Espera o formato "YYYY-MM-DD HH:MM:SS". Interpreta a string como UTC
 * utilizando timegm (POSIX) para evitar deslocamento pelo fuso horário local.
 * Lança std::runtime_error caso o parse falhe.
 *
 * @param datetime String de data/hora no formato "YYYY-MM-DD HH:MM:SS".
 * @return Ponto no tempo equivalente em UTC.
 * @throws std::runtime_error se o formato for inválido.
 */
std::chrono::system_clock::time_point PoseidonSchedule::stringToTimePoint(const std::string& datetime)
{
    std::tm tm = {};
    std::istringstream ss(datetime);

    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail())
        throw std::runtime_error("Falha ao converter datetime: " + datetime);

#if defined(_WIN32)
    auto time_c = _mkgmtime(&tm);
#else
    auto time_c = timegm(&tm);
#endif

    return std::chrono::system_clock::from_time_t(time_c);
}


/**
 * @brief Registra uma falha de execucao de um job no historico.
 *
 * Insere um registro na tabela de historico com status FAILED
 * e a mensagem de erro correspondente, sem alterar last_run_datetime.
 *
 * @param jobId        Identificador do job que falhou.
 * @param errorMessage Descricao do motivo da falha.
 */
void PoseidonSchedule::jobFailDb(const std::string& jobId, const std::string& errorMessage)
{
    ConnectionPool pool(Poseidon::DatabaseConfig::connectionString(), 1);
    auto conn = pool.acquire();

    if (!conn) {
        std::cout << "[Poseidon] - [Schedule] Falha ao conectar ao banco para registrar falha do job cod. " << jobId << "\n";
        return;
    }

    const char* paramValues[2];
    paramValues[0] = jobId.c_str();
    paramValues[1] = errorMessage.c_str();

    auto res = conn->queryParams(R"(
        INSERT INTO poseidon.jcrh_job_cron_history
        (schedule_id, run_at, status, error_message, create_date)
        VALUES ($1, NOW(), 'FAILED', $2, NOW());
    )", 2, paramValues);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
        std::cout << "[Poseidon] ERRO ao inserir historico de falha do job: " << PQerrorMessage(conn->get()) << "\n";

    PostgresDriver::freeResult(res);
}