/* Cria a tabela came_camera no schema horus */
CREATE TABLE IF NOT EXISTS horus.came_camera (
	id SERIAL PRIMARY KEY,
	camera_name VARCHAR(25) NOT NULL,
	link_connection VARCHAR(255) NOT NULL,
	username VARCHAR(30) NULL,
	password VARCHAR(30) NULL,
	active BOOLEAN NOT NULL DEFAULT TRUE,
	create_date TIMESTAMP DEFAULT NOW()
);

COMMENT ON TABLE horus.came_camera IS 'Contem o cadastro das cameras de seguranca do modulo Horus';
COMMENT ON COLUMN horus.came_camera.id IS 'Codigo sequencial da tabela';
COMMENT ON COLUMN horus.came_camera.camera_name IS 'Nome de exibicao da camera';
COMMENT ON COLUMN horus.came_camera.link_connection IS 'URL de conexao com o stream/snapshot da camera';
COMMENT ON COLUMN horus.came_camera.username IS 'Usuario de autenticacao da camera, quando exigido pelo dispositivo';
COMMENT ON COLUMN horus.came_camera.password IS 'Senha de autenticacao da camera, em texto puro';
COMMENT ON COLUMN horus.came_camera.active IS 'Status da camera';
COMMENT ON COLUMN horus.came_camera.create_date IS 'Data/Hora em que o dado foi cadastrado';