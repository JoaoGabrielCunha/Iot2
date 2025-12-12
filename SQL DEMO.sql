-- CREATE TABLE IF NOT EXISTS pessoas(
-- 	matricula_do_estacionador VARCHAR(7) PRIMARY KEY,
-- 	nome_completo VARCHAR(50) not null,
-- 	cargo VARCHAR(15) not null
-- );

-- CREATE TABLE IF NOT EXISTS vagas_puc(
-- 	id SERIAL PRIMARY KEY,
-- 	coordenada_x DECIMAL(10, 2) not null,
-- 	coordenada_y DECIMAL(10, 2) not null,
-- 	tipo_da_vaga VARCHAR(10) not null,
-- 	matricula_do_estacionador VARCHAR(7),

--     CONSTRAINT fk_matricula
--         FOREIGN KEY (matricula_do_estacionador)
--         REFERENCES pessoas(matricula_do_estacionador)
--         ON DELETE SET NULL
--         ON UPDATE CASCADE
-- );

-- CREATE TABLE IF NOT EXISTS cargos(
-- 	cargo VARCHAR(15)
-- );
