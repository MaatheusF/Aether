/*Altera a tabela 'jcrs_job_cron_schedule' no schema 'poseidon' adicionando uma nova coluna chamada 'run_now'*/
ALTER TABLE poseidon.jcrs_job_cron_schedule ADD COLUMN run_now BOOLEAN;

COMMENT ON column poseidon.sens_sensor.external_id IS 'Define se a rotina deve ser executada imediatamente, ignorando o horário agendado.';