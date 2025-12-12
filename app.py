from flask import Flask, render_template, request, redirect
from database import conecta, encerra_conexao

app = Flask(__name__)

# ----------- ROTAS -----------

# HOME
@app.route("/")
def home():
    return render_template("index.html")


# LISTAR VAGAS
@app.route("/listar")
def listar():
    conn = conecta()
    cursor = conn.cursor()
    cursor.execute("SELECT id, coordenada_x, coordenada_y, tipo_da_vaga, matricula_do_estacionador, reservavel FROM vagas_puc ORDER BY id ASC")
    vagas = cursor.fetchall()
    encerra_conexao(conn)
    return render_template("vagas/listar.html", vagas=vagas)

# LISTAR PESSOAS
@app.route("/listar_pessoas")
def listar_pessoas():
    conn = conecta()
    cursor = conn.cursor()
    cursor.execute("SELECT matricula_do_estacionador, nome_completo, cargo FROM pessoas ORDER BY matricula_do_estacionador ASC")
    pessoas = cursor.fetchall()
    encerra_conexao(conn)
    return render_template("pessoas/listar_pessoas.html", pessoas=pessoas)

# LISTAR PESSOAS
@app.route("/listar_reservas")
def listar_reservas():
    conn = conecta()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM reservas ORDER BY data_reserva, horario_chegada DESC")

    # TABLE reservas
    # id - PRIMARY KEY,
    # matricula,
    # vaga_id ,
    # horario_chegada - TIME,
    # horario_saida - TIME,
    # data_reserva - CURRENT_DATE,

    reservas = cursor.fetchall()
    encerra_conexao(conn)
    return render_template("reserva/listar_reservas.html", reservas=reservas)



# INSERIR VAGA
@app.route("/inserir", methods=["GET", "POST"])
def inserir():
    if request.method == "POST":
        x = request.form["coordenada_x"]
        y = request.form["coordenada_y"]
        tipo = request.form["tipo_da_vaga"]
        mat = ""

        conn = conecta()
        cursor = conn.cursor()
        cursor.execute(
            "INSERT INTO vagas_puc (coordenada_x, coordenada_y, tipo_da_vaga, matricula_do_estacionador) VALUES (%s, %s, %s, %s)",
            (x, y, tipo, None)
        )
        conn.commit()
        encerra_conexao(conn)
        return redirect("/listar")

    return render_template("vagas/inserir.html")

#INSERIR PESSOAS
@app.route("/inserir_pessoa", methods=["GET", "POST"])
def inserir_pessoa():
    if request.method == "POST":
        mat = request.form["matricula_do_estacionador"]
        nome = request.form["nome_completo"]
        cargo = request.form["cargo"]

        conn = conecta()
        cursor = conn.cursor()
        cursor.execute(
            "INSERT INTO pessoas (matricula_do_estacionador, nome_completo, cargo) VALUES (%s, %s, %s)",
            (mat, nome, cargo)
        )
        conn.commit()
        encerra_conexao(conn)
        return redirect("/listar_pessoas")

    return render_template("pessoas/inserir_pessoa.html")


# ATUALIZAR MATRÍCULA
@app.route("/atualizar/<int:id>", methods=["GET", "POST"])
def atualizar(id):
    conn = conecta()
    cursor = conn.cursor()

    cursor.execute("SELECT id, matricula_do_estacionador FROM vagas_puc WHERE id = %s", (id,))
    vaga = cursor.fetchone()

    if request.method == "POST":
        nova_mat = request.form["matricula_do_estacionador"] or None

        cursor.execute("SELECT 1 FROM pessoas WHERE matricula_do_estacionador = %s", (nova_mat,))
        
        existe = cursor.fetchone()

        if not existe:
            cursor.close()
            conn.close()
            return render_template("vagas/atualizar.html", vaga=vaga, erro="Essa matricula não está cadastrada!", )
           
        cursor.execute("SELECT 1 FROM vagas_puc WHERE matricula_do_estacionador = %s", (nova_mat,))
        
        ja_estacionado = cursor.fetchone()

        if ja_estacionado:
            cursor.close()
            conn.close()
            return render_template("vagas/atualizar.html", vaga=vaga, erro="Esta pessoa já esta estacionada!", )

        cursor.execute("UPDATE vagas_puc SET matricula_do_estacionador = %s WHERE id = %s", (nova_mat, id))        
        conn.commit()
        encerra_conexao(conn)
        return redirect("/listar")

    # pegar valores atuais
    encerra_conexao(conn)
    return render_template("vagas/atualizar.html", vaga=vaga)

#VAGAR
@app.route("/vagar/<int:id>")
def vagar(id):
    conn = conecta()
    cursor = conn.cursor()
    cursor.execute("UPDATE vagas_puc SET matricula_do_estacionador = NULL WHERE id = %s", (id, ))
    conn.commit()
    encerra_conexao(conn)
    return redirect("/listar")


# DELETAR
@app.route("/deletar/<int:id>")
def deletar(id):
    conn = conecta()
    cursor = conn.cursor()
    cursor.execute("DELETE FROM vagas_puc WHERE id = %s", (id,))
    conn.commit()
    encerra_conexao(conn)
    return redirect("/listar")

# LOGIN
@app.route("/login", methods=["GET", "POST"])
def pagina_inicial_login():
    if request.method == "POST":
        matricula = request.form["matricula_do_login"]
        senha = request.form["senha"]

        conn = conecta()
        cursor = conn.cursor()

        cursor.execute("SELECT 1 FROM pessoas WHERE matricula_do_estacionador = %s", (matricula,))
        
        existe = cursor.fetchone()

        if not existe:
            cursor.close()
            conn.close()
            return render_template("reserva/login.html", erro="Essa matricula não está cadastrada!", )
        
        cursor.execute("""
            SELECT 1
            FROM (
                SELECT matricula_do_estacionador
                FROM vagas_puc
                WHERE matricula_do_estacionador = %s

                UNION

                -- Pessoa tem reserva ativa ou futura
                SELECT matricula
                FROM reservas
                WHERE matricula = %s
                AND
                data_reserva >= CURRENT_DATE
            ) AS resultado
            LIMIT 1;
        """, (matricula, matricula))

        ja_tem_reserva = cursor.fetchone()

        if ja_tem_reserva:
            
            cursor.execute("""
                SELECT horario_chegada, horario_saida, data_reserva
                FROM reservas
                WHERE matricula = %s
                AND 
                data_reserva >= CURRENT_DATE
                LIMIT 1;
            """, (matricula,))

            reserva = cursor.fetchone()

            if reserva is not None:

                horario_chegada = reserva[0]
                horario_saida   = reserva[1]
                data_reserva    = reserva[2]

                responde = render_template("reserva/nao_pode_reservar.html", chegada=horario_chegada, saida=horario_saida, data=data_reserva)

                cursor.close()
                conn.close()

                return responde
                
            else:
                return render_template("reserva/esta_estacionado.html")

        encerra_conexao(conn)
        return redirect(f"/pagina_de_reserva?matricula={matricula}")

    return render_template("reserva/login.html")

# RESERVA
@app.route("/pagina_de_reserva", methods=["GET", "POST"])
def reservar():

    matricula = request.args.get("matricula")

    conn = conecta()
    cursor = conn.cursor()
    cursor.execute("SELECT id, coordenada_x, coordenada_y, COUNT(*) OVER() AS total FROM vagas_puc WHERE tipo_da_vaga = 'grande' AND reservavel IS TRUE AND matricula_do_estacionador IS NULL ORDER BY id ASC;")
    vagas_grande = cursor.fetchall()

    cursor.execute("SELECT id, coordenada_x, coordenada_y, COUNT(*) OVER() AS total FROM vagas_puc WHERE tipo_da_vaga = 'pequena' AND reservavel IS TRUE AND matricula_do_estacionador IS NULL ORDER BY id ASC;")
    vagas_pequena = cursor.fetchall()
    
    cursor.execute("SELECT id, coordenada_x, coordenada_y, COUNT(*) OVER() AS total FROM vagas_puc WHERE tipo_da_vaga = 'elétrica' AND  reservavel IS TRUE AND matricula_do_estacionador IS NULL ORDER BY id ASC;")
    vagas_eletrica = cursor.fetchall()
    
    if request.method == "POST":
        matricula = request.form["matricula"]
        tipo = request.form["tipo_da_vaga"]
        horario_chegada = request.form["horario_chegada"]
        horario_saida = request.form["horario_saida"]

    # TABLE reservas
    # id - PRIMARY KEY,
    # matricula,
    # vaga_id ,
    # horario_chegada - TIME,
    # horario_saida - TIME,
    # data_reserva - CURRENT_DATE,

        conn = conecta()
        cursor = conn.cursor()

        tipo_db = tipo.split("vaga_",1)[1]

        cursor.execute("""
        SELECT v.id
        FROM vagas_puc v
        WHERE v.tipo_da_vaga = %s
          AND v.matricula_do_estacionador IS NULL
          AND reservavel IS TRUE
          AND NOT EXISTS (
                SELECT 1
                FROM reservas r
                WHERE r.vaga_id = v.id
                AND r.horario_chegada < %s
                AND r.horario_saida > %s
          )
        ORDER BY v.id
        LIMIT 1;
        """, (tipo_db, horario_saida, horario_chegada))

        vaga_disponivel = cursor.fetchone()

        if vaga_disponivel is None:
            encerra_conexao(conn)
            return render_template("reserva/pagina_de_reserva.html", vagas_grande=vagas_grande, vagas_pequena=vagas_pequena, vagas_eletrica=vagas_eletrica, erro="Não existem vagas deste tipo disponíveis neste horário!", )

        vaga_id = vaga_disponivel[0]

        # Inserir a reserva
        cursor.execute("""
            INSERT INTO reservas (matricula, vaga_id, horario_chegada, horario_saida)
            VALUES (%s, %s, %s, %s)
        """, (matricula, vaga_id, horario_chegada, horario_saida)
        )

        conn.commit()
        encerra_conexao(conn)
        return redirect("/listar_reservas")

    encerra_conexao(conn)
    return render_template("reserva/pagina_de_reserva.html", vagas_grande=vagas_grande, vagas_pequena=vagas_pequena, vagas_eletrica=vagas_eletrica, matricula=matricula)


# INICIAR
if __name__ == "__main__":
    app.run(debug=True)
