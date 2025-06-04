import cv2
import datetime
import mysql.connector
import os
import time
from mysql.connector import Error

# Configurações do banco de dados
DB_CONFIG = {
    'host': '100.77.203.115',
    'user': 'root',
    'password': '1234',
    'database': 'sensores'
}

# Configurações da captura
CAPTURE_DURATION = 10  # segundos de gravação
INTERVAL = 10  # 10 minutos em segundos
RESOLUTION = (640, 480)
FPS = 30

def find_webcam_device():
    """Tenta encontrar o dispositivo correto da webcam"""
    # Tentativas com dispositivos comuns
    for device in ['/dev/video3']:
        if os.path.exists(device):
            return device
    # Se não encontrar, verifica o dispositivo informado
    if os.path.exists('/dev/ttyACM0'):
        print("Aviso: /dev/ttyACM0 é tipicamente um dispositivo serial, não uma webcam")
        return '/dev/ttyACM0'
    return None

def create_videos_table(connection):
    """Cria a tabela de vídeos se não existir"""
    try:
        cursor = connection.cursor()
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS videos (
            id INT AUTO_INCREMENT PRIMARY KEY,
            timestamp DATETIME NOT NULL,
            file_path VARCHAR(255) NOT NULL,
            duration INT NOT NULL,
            resolution VARCHAR(20) NOT NULL,
            file_size INT NOT NULL,
            status VARCHAR(50) NOT NULL
        )
        """)
        connection.commit()
        print("Tabela de vídeos verificada/criada com sucesso.")
    except Error as e:
        print(f"Erro ao criar tabela: {e}")

def record_video(device, output_filename, duration):
    """Grava um vídeo da webcam"""
    cap = cv2.VideoCapture(device)
    
    if not cap.isOpened():
        print(f"Erro: Não foi possível abrir o dispositivo {device}")
        return 0, "dispositivo não disponível"
    
    # Configura resolução
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, RESOLUTION[0])
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, RESOLUTION[1])
    cap.set(cv2.CAP_PROP_FPS, FPS)
    
    # Verifica configurações efetivas
    actual_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    actual_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    actual_fps = cap.get(cv2.CAP_PROP_FPS)
    
    print(f"Configuração da câmera: {actual_width}x{actual_height} a {actual_fps:.2f} FPS")
    
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(output_filename, fourcc, FPS, (actual_width, actual_height))
    
    if not out.isOpened():
        print("Erro: Não foi possível criar o arquivo de vídeo")
        cap.release()
        return 0, "erro ao criar arquivo"
    
    start_time = time.time()
    frames_captured = 0
    
    while (time.time() - start_time) < duration:
        ret, frame = cap.read()
        if ret:
            out.write(frame)
            frames_captured += 1
        else:
            print("Erro ao capturar frame")
            break
    
    cap.release()
    out.release()
    
    if os.path.exists(output_filename):
        file_size = os.path.getsize(output_filename)
        status = f"sucesso - {frames_captured} frames"
        return file_size, status
    else:
        return 0, "arquivo não criado"

def save_video_info_to_db(connection, timestamp, file_path, duration, resolution, file_size, status):
    """Salva as informações do vídeo no banco de dados"""
    try:
        cursor = connection.cursor()
        query = """
        INSERT INTO videos (timestamp, file_path, duration, resolution, file_size, status)
        VALUES (%s, %s, %s, %s, %s, %s)
        """
        cursor.execute(query, (timestamp, file_path, duration, resolution, file_size, status))
        connection.commit()
        print(f"Informações do vídeo salvas no banco de dados. ID: {cursor.lastrowid}")
        return cursor.lastrowid
    except Error as e:
        print(f"Erro ao salvar no banco de dados: {e}")
        return None

def main():
    webcam_device = find_webcam_device()
    if not webcam_device:
        print("Erro: Nenhum dispositivo de câmera encontrado")
        return
    
    print(f"Usando dispositivo: {webcam_device}")
    
    try:
        connection = mysql.connector.connect(**DB_CONFIG)
        if connection.is_connected():
            print("Conectado ao banco de dados MySQL")
            create_videos_table(connection)
            
            while True:
                timestamp = datetime.datetime.now()
                filename = f"video_{timestamp.strftime('%Y%m%d_%H%M%S')}.avi"
                
                print(f"Iniciando gravação em {webcam_device}: {filename}")
                file_size, status = record_video(webcam_device, filename, CAPTURE_DURATION)
                
                resolution_str = f"{RESOLUTION[0]}x{RESOLUTION[1]}"
                save_video_info_to_db(
                    connection,
                    timestamp,
                    filename,
                    CAPTURE_DURATION,
                    resolution_str,
                    file_size,
                    status
                )
                
                print(f"Próxima gravação em {INTERVAL/60} minutos...")
                time.sleep(INTERVAL)
                
    except Error as e:
        print(f"Erro de conexão com o banco de dados: {e}")
    except KeyboardInterrupt:
        print("Programa encerrado pelo usuário")
    finally:
        if 'connection' in locals() and connection.is_connected():
            connection.close()
            print("Conexão com MySQL encerrada")

if __name__ == "__main__":
    main()