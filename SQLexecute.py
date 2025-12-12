import psycopg2
import os
from dotenv import load_dotenv
from psycopg2 import Error

load_dotenv()

password = os.getenv("PASSWORD")

def executar_sql(arquivo):
    with open(arquivo, "r", encoding = "utf-8") as f:
        sql = f.read()
    
    conn = psycopg2.connect(
            user="iot",
            password=password,
            host="postgresql.janks.dev.br",
            port="5432",
            database="projeto_b"
        )
    
    cur = conn.cursor()
    cur.execute(sql)
    conn.commit()

    cur.close()
    conn.close()
    print("Arquivo SQL executado com sucesso!")

# Chame assim:
executar_sql("SQL DEMO.sql")