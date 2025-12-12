from database import conecta, encerra_conexao

def main():

    connection = conecta()
    
    cursor = connection.cursor()

    def criar_tabela():
        with open("SQL DEMO.sql", "r") as file:
            content = file.read()
            cursor.execute(content)
            connection.commit()

    #CREATE
    def insert_vaga(coordenada_x, coordenada_y, tipo_da_vaga, matricula):
        cmd_insert = "INSERT INTO vagas_puc (coordenada_x, coordenada_y, tipo_da_vaga, matricula_do_estacionador) VALUES (%s, %s, %s, %s);"
        values = coordenada_x, coordenada_y, tipo_da_vaga, matricula
        cursor.execute(cmd_insert, values)
        connection.commit()
        print("Vaga inserida na tabela")

    def insert_pessoa(matricula, nome, cargo):
        cmd_insert = "INSERT INTO pessoas (matricula_do_estacionador, nome_completo, cargo) VALUES (%s, %s, %s);"
        values =  matricula, nome, cargo
        cursor.execute(cmd_insert, values)
        connection.commit()
        print("Pessoa inserida na tabela")

    

    #READ
    def seleciona():
        cmd_select = "SELECT coordenada_x, coordenada_y, tipo_da_vaga, matricula_do_estacionador from vagas_puc"
        cursor.execute(cmd_select)
        vagas = cursor.fetchall()
        for vaga in vagas:
            print(vaga)
        return vaga
    
    #UPDATE
    def atualiza(matricula, id):
        cmd_update = f"UPDATE vagas_puc SET matricula={matricula} WHERE id ='{id}'"
        cursor.execute(cmd_update)
        connection.commit()

    #DELETE
    def deleta(id):
        cmd_delete = f"DELETE FROM vagas_puc WHERE id='{id}'"
        cursor.execute(cmd_delete)
        connection.commit()
        print("Vaga deletada com sucesso")

    #--------------------------------------------------------------------------------------------------------------------#

    #resetar o banco:
    #   cursor.execute("TRUNCATE TABLE vagas_puc RESTART IDENTITY;")
    #   connection.commit()

    #banco base experimental:
    # for i in range(10):
    #     insert_vaga(str(i*10), 0, "pequena", None)
    # for i in range(10):
    #     insert_vaga(str((10+i)*10), 0, "grande", None)
    # for i in range(5):
    #     insert_vaga(str((20+i)*10), 0, "elétrica", None)

    #insert_pessoa("2410295", "Lucca Carone Marinho de Azevedo", "aluno")
    #insert_pessoa("1234567", "Jan 'JANKS' ks", "professor")

    #cursor.execute("ALTER TABLE vagas_puc ADD COLUMN reservavel BOOLEAN NOT NULL DEFAULT FALSE;")
    #connection.commit()

    # cursor.execute('''CREATE TABLE reservas (
    # id SERIAL PRIMARY KEY,
    # matricula VARCHAR(50) NOT NULL,
    # vaga_id INT NOT NULL,
    # horario_chegada TIME NOT NULL,
    # horario_saida TIME NOT NULL,
    # data_reserva DATE NOT NULL DEFAULT CURRENT_DATE,
    
    # CONSTRAINT fk_vaga
    #     FOREIGN KEY (vaga_id)
    #     REFERENCES vagas_puc(id)
    #     ON DELETE CASCADE
    # );
    # ''')
    
    # connection.commit()

    #cursor.execute("DELETE FROM reservas;")

    #connection.commit()

#     cursor.execute('''UPDATE vagas_puc
# SET
#   coordenada_x = CASE id
#     WHEN 1 THEN -22.97815
#     WHEN 2 THEN -22.97822
#     WHEN 3 THEN -22.97829
#     WHEN 4 THEN -22.97836
#     WHEN 5 THEN -22.97843

#     WHEN 6 THEN -22.97850
#     WHEN 7 THEN -22.97857
#     WHEN 8 THEN -22.97864
#     WHEN 9 THEN -22.97871
#     WHEN 10 THEN -22.97878

#     WHEN 11 THEN -22.97815
#     WHEN 12 THEN -22.97822
#     WHEN 13 THEN -22.97829
#     WHEN 14 THEN -22.97836
#     WHEN 15 THEN -22.97843

#     WHEN 16 THEN -22.97850
#     WHEN 17 THEN -22.97857
#     WHEN 18 THEN -22.97864
#     WHEN 19 THEN -22.97871
#     WHEN 20 THEN -22.97878

#     WHEN 21 THEN -22.97822
#     WHEN 22 THEN -22.97829
#     WHEN 23 THEN -22.97836
#     WHEN 24 THEN -22.97843
#     WHEN 25 THEN -22.97850
#     ELSE coordenada_x
#   END,

#   coordenada_y = CASE id
#     WHEN 1 THEN -43.23165
#     WHEN 2 THEN -43.23165
#     WHEN 3 THEN -43.23165
#     WHEN 4 THEN -43.23165
#     WHEN 5 THEN -43.23165

#     WHEN 6 THEN -43.23165
#     WHEN 7 THEN -43.23165
#     WHEN 8 THEN -43.23165
#     WHEN 9 THEN -43.23165
#     WHEN 10 THEN -43.23165

#     WHEN 11 THEN -43.23140
#     WHEN 12 THEN -43.23140
#     WHEN 13 THEN -43.23140
#     WHEN 14 THEN -43.23140
#     WHEN 15 THEN -43.23140

#     WHEN 16 THEN -43.23140
#     WHEN 17 THEN -43.23140
#     WHEN 18 THEN -43.23140
#     WHEN 19 THEN -43.23140
#     WHEN 20 THEN -43.23140

#     WHEN 21 THEN -43.23120
#     WHEN 22 THEN -43.23120
#     WHEN 23 THEN -43.23120
#     WHEN 24 THEN -43.23120
#     WHEN 25 THEN -43.23120
#     ELSE coordenada_y
#   END
# WHERE id BETWEEN 1 AND 25;

# ''')
    
    # connection.commit()

    #--------------------------------------------------------------------------------------------------------------------#

    encerra_conexao(connection)


if __name__ == "__main__":
    main()