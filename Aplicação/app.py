from flask import Flask, request, jsonify, render_template, Response
import mysql.connector
from datetime import datetime

app = Flask(__name__)

conn = mysql.connector.connect(
    host="100.77.203.115",
    user="root",
    password="1234",
    database="sensores"
)

def get_dados():
    cursor = conn.cursor(dictionary=True)
    
    # Query modificada para usar o fuso horário configurado
    cursor.execute("""
        SELECT id, movimento, descricao, 
               data_hora  -- Já está no fuso correto (-03:00)
        FROM presencas 
        ORDER BY data_hora DESC, id DESC
    """)
    
    dados = cursor.fetchall()
    
    # Processamento dos dados (agora simplificado)
    dados_filtrados = []
    for d in dados:
        if isinstance(d['data_hora'], str):
            try:
                d['data_hora'] = datetime.strptime(d['data_hora'], "%Y-%m-%d %H:%M:%S")
            except ValueError:
                d['data_hora'] = None
        
        if d['data_hora'] is not None:
            dados_filtrados.append(d)
    
    # Restante do código permanece igual...
    cursor.execute("SELECT descricao, COUNT(*) as total FROM presencas GROUP BY descricao")
    contagens = cursor.fetchall()
    cursor.close()
    
    total_detectado = sum(c['total'] for c in contagens if c['descricao'] == 'Movimento Detectado')
    total_sem = sum(c['total'] for c in contagens if c['descricao'] == 'Sem Movimento')
    
    return dados_filtrados, total_detectado, total_sem


@app.route("/")
def dashboard():
    dados, detectado, sem = get_dados()
    return render_template("dashboard.html", dados=dados, detectado=detectado, sem=sem)

@app.route("/exportar")
def exportar_csv():
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM presencas ORDER BY data_hora DESC")
    rows = cursor.fetchall()
    colunas = [i[0] for i in cursor.description]

    def gerar_csv():
        yield ','.join(colunas) + '\n'
        for row in rows:
            yield ','.join(str(campo) for campo in row) + '\n'

    return Response(gerar_csv(), mimetype="text/csv", headers={"Content-Disposition": "attachment; filename=presencas.csv"})

@app.route("/sensor", methods=["POST"])
def receber_sensor():
    try:
        dados = request.get_json()
        movimento = dados.get("movimento")
        descricao = dados.get("descricao")
        data_hora = dados.get("data_hora")  # enviado pelo ESP com horário do NTP

        cursor = conn.cursor()
        cursor.execute("INSERT INTO presencas (movimento, descricao, data_hora) VALUES (%s, %s, %s)", 
                       (movimento, descricao, data_hora))
        conn.commit()
        cursor.close()

        return jsonify({"status": "sucesso", "mensagem": "Dados recebidos e inseridos."}), 200

    except Exception as e:
        return jsonify({"status": "erro", "mensagem": str(e)}), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
