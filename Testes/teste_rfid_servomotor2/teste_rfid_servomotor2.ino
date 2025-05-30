#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN     5     // Pino RST do RFID
#define SS_PIN      53    // Pino SDA (SS) do RFID
#define SERVO_PIN   8     // Pino do servo motor

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Instância do MFRC522
Servo meuServo;                     // Instância do Servo

String uidAutorizado = "73 F2 B3 0E"; // UID do cartão autorizado

void setup() {
  pinMode(SS_PIN, OUTPUT); // Necessário no Arduino Mega
  Serial.begin(9600);
  while (!Serial); // Espera a porta serial estar pronta
  SPI.begin();
  mfrc522.PCD_Init();
  
  meuServo.attach(SERVO_PIN);
  meuServo.write(45); // Posição inicial
  
  Serial.println("Sistema RFID pronto - Aproxime o cartão");
}

void loop() {
  // Reinicia o leitor se não houver cartão
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
    return;
  }
  
  // Tenta ler o cartão
  if (!mfrc522.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }
  
  // Processa o UID do cartão
  Serial.print("UID detectado: ");
  String uidLido = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    uidLido.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uidLido.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  uidLido.toUpperCase();
  Serial.println();

  // Verifica autorização
  if (uidLido.substring(1) == uidAutorizado) {
    Serial.println("Acesso liberado!");
    meuServo.write(150);
    delay(5000);
    meuServo.write(45);
  } else {
    Serial.println("Acesso negado!");
    delay(1000); // Delay maior para cartões não autorizados
  }

  // Limpeza CRUCIAL para nova leitura
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
