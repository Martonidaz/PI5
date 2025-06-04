from flask import Flask, request, jsonify, render_template
import serial
import time
from datetime import datetime
import mysql.connector
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

# Configurações
DB_CONFIG = {
    'host': '100.77.203.115',
    'user': 'root',
    'password': '1234',
    'database': 'sensores'
}

SENHAS = {
    "1234": "Joao V Ferreira",
    "2222": "Mariana Freitas",
    "1111": "Breno Andrade",
    "0000": "Daniel Marton"
}

# Configuração da porta serial (ajuste conforme necessário)
#Altere a porta de acordo com a sua entrada USB
try:
    porta_serial = serial.Serial('/dev/ttyACM0', 9600, timeout=2) 
    time.sleep(2)  # Aguarda o Arduino iniciar
except:
    print("Erro ao conectar à porta serial")
    porta_serial = None

def registrar_acesso(nome, status):
    try:
        conexao = mysql.connector.connect(**DB_CONFIG)
        cursor = conexao.cursor()
        data_hora = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        sql = "INSERT INTO portaria (nome, data_hora, status) VALUES (%s, %s, %s)"
        valores = (nome, data_hora, status)
        cursor.execute(sql, valores)
        conexao.commit()
        conexao.close()
        return True
    except Exception as e:
        print(f"Erro ao registrar acesso: {e}")
        return False

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/verificar_senha', methods=['POST'])
def verificar_senha():
    data = request.get_json()
    senha = data.get('senha', '')

    if senha in SENHAS:
        nome = SENHAS[senha]
        registrar_acesso(nome, "Liberado")
        if porta_serial:
            porta_serial.write(b"LIBERADO\n")
        return jsonify({
            'status': 'success',
            'message': f'Acesso liberado para {nome}',
            'nome': nome,
            'acesso': 'Liberado'
        })
    else:
        registrar_acesso("Desconhecido", "Negado")
        if porta_serial:
            porta_serial.write(b"NEGADO\n")
        return jsonify({
            'status': 'error',
            'message': 'Acesso negado',
            'acesso': 'Negado'
        })

@app.route('/ultimos_acessos', methods=['GET'])
def ultimos_acessos():
    try:
        conexao = mysql.connector.connect(**DB_CONFIG)
        cursor = conexao.cursor(dictionary=True)
        sql = "SELECT nome, data_hora, status FROM portaria ORDER BY data_hora DESC LIMIT 10"
        cursor.execute(sql)
        resultados = cursor.fetchall()
        conexao.close()
        return jsonify({
            'status': 'success',
            'acessos': resultados
        })
    except Exception as e:
        return jsonify({
            'status': 'error',
            'message': str(e)
        })

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5001)