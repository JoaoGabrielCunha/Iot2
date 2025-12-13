import psycopg2
import os
from dotenv import load_dotenv
from psycopg2 import Error

load_dotenv()

user = os.getenv("USER")
password = os.getenv("PASSWORD")
host = os.getenv("HOST")
port = os.getenv("PORT")
database = os.getenv("DATABASE")

def conecta():
    try:
        conn = psycopg2.connect(
            user=user,
            password=password,
            host=host,
            port=port,
            database=database
        )

        print("Conectado no postgres com sucesso!")
        return conn
    
    except Error as e:
        print(f"Ocorreu um erro ao tentar conectar no banco de dados: {e}")



def encerra_conexao(conn):
    if conn:
        conn.close()
        print("Conexao encerrada com o banco de dados!")
