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

    cursor.execute("DELETE FROM reservas;")

    connection.commit()

    #--------------------------------------------------------------------------------------------------------------------#

    encerra_conexao(connection)


if __name__ == "__main__":
    main()