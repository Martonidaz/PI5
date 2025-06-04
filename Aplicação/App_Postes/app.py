from flask import Flask, jsonify, render_template_string, request
from datetime import datetime
import serial
import mysql.connector
from mysql.connector import Error
import time
import threading
import logging
import json

# Configuração de logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = Flask(__name__)

# Configurações
DB_CONFIG = {
    'host': '100.77.203.115',
    'user': 'root',
    'password': '1234',
    'database': 'sensores'
}

SERIAL_PORT = 'COM9' #Altere de acordo com sua entrada USB
BAUD_RATE = 9600

# Variável global para os dados
latest_data = {
    'leds': [False] * 6,
    'ldrs': [0] * 3,
    'timestamp': None,
    'location': None
}

# Carrega o HTML da interface
with open('index.html', 'r') as file:
    INTERFACE_HTML = file.read()

class ArduinoMonitor:
    def __init__(self):
        self.arduino = None
        self.running = False
        self.connect()

    def connect(self):
        """Estabelece conexão com o Arduino"""
        max_retries = 5
        for attempt in range(max_retries):
            try:
                self.arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
                time.sleep(2)
                logger.info(f"Conectado ao Arduino em {SERIAL_PORT}")
                return True
            except (serial.SerialException, OSError) as e:
                logger.error(f"Tentativa {attempt + 1}/{max_retries}: {e}")
                if attempt == max_retries - 1:
                    logger.error("Falha ao conectar ao Arduino")
                    return False
                time.sleep(2)

    def start_monitoring(self):
        """Inicia o monitoramento contínuo"""
        if not self.arduino and not self.connect():
            return

        self.running = True
        logger.info("Iniciando monitoramento do Arduino...")
        
        while self.running:
            try:
                line = self.arduino.readline().decode('utf-8').strip()
                if line:
                    self.process_data(line)
            except Exception as e:
                logger.error(f"Erro na leitura serial: {e}")
                time.sleep(1)
                self.reconnect()

    def process_data(self, line):
        """Processa os dados recebidos do Arduino"""
        try:
            line = line.strip()
            
            if not line or not line.startswith('{') or not line.endswith('}'):
                logger.warning(f"Dados incompletos recebidos: {line}")
                return

            try:
                data = json.loads(line)
            except json.JSONDecodeError as e:
                logger.warning(f"JSON inválido: {e} - Dados: {line}")
                return

            # Valida a estrutura dos dados
            required_keys = {'leds', 'ldrs'}
            if not all(key in data for key in required_keys):
                logger.warning(f"Estrutura inválida: {data}")
                return

            # Processa os LEDs (garante 6 valores)
            leds = data['leds']
            if len(leds) != 6:
                logger.warning(f"Quantidade inválida de LEDs: {len(leds)}")
                return

            # Processa os LDRs (garante 3 valores)
            ldrs = data['ldrs']
            if len(ldrs) != 3:
                logger.warning(f"Quantidade inválida de LDRs: {len(ldrs)}")
                return

            # Atualiza os dados globais
            global latest_data
            latest_data = {
                'leds': [bool(led) for led in leds],
                'ldrs': ldrs,
                'timestamp': datetime.now(),
                'location': "Local Padrão"
            }
            
            # Salva no banco de dados
            self.save_to_database(latest_data)
            logger.info("Dados processados e salvos com sucesso")

        except Exception as e:
            logger.error(f"Erro ao processar dados: {str(e)}")

    def save_to_database(self, data):
        """Salva os dados no MySQL"""
        connection = None
        try:
            connection = mysql.connector.connect(**DB_CONFIG)
            cursor = connection.cursor()
            
            query = """
                INSERT INTO postes 
                (data_hora, led1, led2, led3, led4, led5, led6, ldr1, ldr2, ldr3, localizacao)
                VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
            """
            values = (
                data['timestamp'],
                *data['leds'],
                *data['ldrs'],
                data['location']
            )
            
            cursor.execute(query, values)
            connection.commit()
            logger.info("Dados salvos no banco de dados")
            
        except Error as e:
            logger.error(f"Erro no banco de dados: {e}")
        finally:
            if cursor:
                cursor.close()
            if connection:
                connection.close()

    def reconnect(self):
        """Tenta reconectar ao Arduino"""
        logger.info("Tentando reconectar ao Arduino...")
        if self.arduino:
            self.arduino.close()
        self.connect()

    def stop(self):
        """Para o monitoramento"""
        self.running = False
        if self.arduino:
            self.arduino.close()
        logger.info("Monitoramento encerrado")

def init_database():
    """Inicializa o banco de dados"""
    try:
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor()
        
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS postes (
                id INT AUTO_INCREMENT PRIMARY KEY,
                data_hora DATETIME NOT NULL,
                led1 BOOLEAN NOT NULL,
                led2 BOOLEAN NOT NULL,
                led3 BOOLEAN NOT NULL,
                led4 BOOLEAN NOT NULL,
                led5 BOOLEAN NOT NULL,
                led6 BOOLEAN NOT NULL,
                ldr1 INT NOT NULL,
                ldr2 INT NOT NULL,
                ldr3 INT NOT NULL,
                localizacao VARCHAR(255)
            )
        """)
        connection.commit()
        logger.info("Banco de dados verificado/criado")
        
    except Error as e:
        logger.error(f"Erro no banco de dados: {e}")
    finally:
        if cursor:
            cursor.close()
        if connection:
            connection.close()

@app.route('/')
def index():
    """Rota principal que serve a interface HTML"""
    return render_template_string(INTERFACE_HTML)

@app.route('/api/data', methods=['GET'])
def get_data():
    return jsonify(latest_data)

@app.route('/api/postes', methods=['GET'])
def get_postes():
    """Retorna dados dos postes para a tabela"""
    connection = None
    try:
        connection = mysql.connector.connect(**DB_CONFIG)
        cursor = connection.cursor(dictionary=True)
        
        cursor.execute("""
            SELECT * FROM postes 
            ORDER BY data_hora DESC 
            LIMIT 5
        """)
        postes = cursor.fetchall()
        
        # Formata os dados para a tabela
        formatted_data = []
        for i, poste in enumerate(postes, 1):
            formatted_data.append({
                'id': f"P{str(i).zfill(3)}",
                'data_hora': poste['data_hora'].strftime('%d/%m/%Y %H:%M:%S'),
                'status': 'Aceso' if int(poste['led1']) == 1 else 'Apagado',
                'ldr': f"{int((1023 - poste['ldr1']) / 10.23)}%",
                'localizacao': poste['localizacao']
            })
        
        return jsonify(formatted_data)
        
    except Error as e:
        logger.error(f"Erro ao buscar postes: {e}")
        return jsonify([])
    finally:
        if cursor:
            cursor.close()
        if connection:
            connection.close()

@app.route('/api/leds', methods=['GET'])
def get_leds():
    return jsonify({
        'leds': latest_data['leds'],
        'timestamp': latest_data['timestamp'].isoformat() if latest_data['timestamp'] else None
    })

@app.route('/api/ldrs', methods=['GET'])
def get_ldrs():
    return jsonify({
        'ldrs': latest_data['ldrs'],
        'timestamp': latest_data['timestamp'].isoformat() if latest_data['timestamp'] else None
    })

@app.route('/health', methods=['GET'])
def health_check():
    global arduino_monitor
    return jsonify({
        'status': 'healthy',
        'arduino_connected': arduino_monitor.arduino is not None if 'arduino_monitor' in globals() else False,
        'last_update': latest_data['timestamp'].isoformat() if latest_data['timestamp'] else None
    })

if __name__ == '__main__':
    # Inicializa banco de dados
    init_database()
    
    # Cria e inicia o monitor do Arduino
    arduino_monitor = ArduinoMonitor()
    monitor_thread = threading.Thread(target=arduino_monitor.start_monitoring, daemon=True)
    monitor_thread.start()
    
    # Configuração do Flask para ignorar logs de 404
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)
    
    # Roda o aplicativo Flask
    try:
        logger.info("Servidor Flask iniciado")
        app.run(host='0.0.0.0', port=5000, debug=False)
    except KeyboardInterrupt:
        logger.info("Desligando servidor...")
        arduino_monitor.stop()