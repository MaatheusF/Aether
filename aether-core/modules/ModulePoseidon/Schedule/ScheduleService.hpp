#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <string>

class ConnectionPool;

/**
 * @class PoseidonSchedule
 * @brief Gerencia as tarefas agendadas do Poseidon.
 *
 * Mantém uma thread interna que verifica periodicamente os agendamentos
 * cadastrados no banco de dados e executa os jobs correspondentes,
 * simulando o comportamento de um crontab.
 */
class PoseidonSchedule
{
public:
    /**
     * @brief Construtor padrão.
     *
     * Inicializa o pool de conexões com o banco de dados que será
     * reutilizado durante o ciclo de vida do serviço.
     */
    PoseidonSchedule();

    /**
     * @brief Destruidor.
     *
     * Garante o encerramento seguro da thread de agendamento
     * ao destruir a instância.
     */
    ~PoseidonSchedule();

    /**
     * @brief Inicia o serviço de agendamento.
     *
     * Define o estado interno como ativo e cria a thread responsável
     * por executar o loop periódico de verificação dos agendamentos.
     * Não tem efeito caso o serviço já esteja em execução.
     */
    void start();

    /**
     * @brief Encerra o serviço de agendamento.
     *
     * Sinaliza a thread para que finalize o loop imediatamente,
     * sem aguardar o próximo ciclo de sleep, e aguarda sua
     * finalização de forma segura via join().
     */
    void stop();

private:
    /**
     * @brief Loop principal do serviço de agendamento.
     *
     * Executa continuamente enquanto o serviço estiver ativo.
     * A cada 60 segundos busca os agendamentos ativos no banco de dados
     * e executa os jobs cujo próximo horário de execução já foi atingido.
     */
    void loop();

    /**
     * @brief Atualiza o banco de dados após a execução de um job.
     *
     * Registra o horário da última execução na tabela de agendamentos
     * e insere um registro de histórico com status SUCCESS.
     *
     * @param jobId Identificador do job executado.
     */
    static void jobUpdateDb(const std::string& jobId);

    /**
     * @brief Registra uma falha de execucao de um job no historico.
     *
     * Insere um registro na tabela de historico com status FAILED
     * e a mensagem de erro correspondente, sem alterar last_run_datetime.
     *
     * @param jobId        Identificador do job que falhou.
     * @param errorMessage Descricao do motivo da falha.
     */
    static void jobFailDb(const std::string& jobId, const std::string& errorMessage);

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
    static bool jobChangeStateRelay(const std::string& jobId, const std::string& payloadStr, const std::string& deviceName);

    /**
     * @brief Converte uma string de data/hora para um time_point UTC.
     *
     * Espera o formato "YYYY-MM-DD HH:MM:SS". Lança std::runtime_error
     * caso a conversão falhe.
     *
     * @param datetime String de data/hora no formato esperado.
     * @return Ponto no tempo equivalente em UTC.
     * @throws std::runtime_error se o formato for inválido.
     */
    std::chrono::system_clock::time_point stringToTimePoint(const std::string& datetime);

    std::atomic<bool>       running_{false};
    std::thread             threadScheduleService_;
    std::mutex              cvMutex_;
    std::condition_variable cv_;
    std::unique_ptr<ConnectionPool>  pool_;
};
