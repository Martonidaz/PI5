#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN     5     // Pino RST do RFID
#define SS_PIN      53    // Pino SDA (SS) do RFID
#define SERVO_PIN   8     // Pino do servo motor

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Instância do MFRC522
Servo meuServo;                     // Instância do Servo

// Substitua pelo UID do seu cartão (em hexadecimal)
String uidAutorizado = "73 F2 B3 0E"; // Exemplo: "A1 B2 C3 D4"

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Configura o servo motor
  meuServo.attach(SERVO_PIN);
  meuServo.write(45); // Posição inicial (45 graus)
  
  Serial.println("Sistema RFID com Servo Motor");
  Serial.println("Aproxime o cartao RFID do leitor...");
  Serial.println();
}

void loop() {
  // Verifica se há cartão presente
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Tenta ler o cartão
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Mostra UID na serial
  Serial.print("UID da tag: ");
  String uidLido = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    
    uidLido.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uidLido.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  uidLido.toUpperCase();
  
  // Verifica se é o cartão autorizado
  if (uidLido.substring(1) == uidAutorizado) {
    Serial.println("Cartao autorizado! Ativando servo...");
    
    // Levanta o servo (150 graus)
    meuServo.write(150);
    delay(5000); // Mantém por 1 segundo
    
    // Retorna à posição inicial (45 graus)
    meuServo.write(45);
  } else {
    Serial.println("Cartao NAO autorizado!");
  }
  
  delay(1000); // Delay entre leituras
}
