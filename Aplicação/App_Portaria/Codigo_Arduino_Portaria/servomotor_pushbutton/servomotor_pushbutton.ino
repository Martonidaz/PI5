#include <Servo.h>

Servo cancela;
const int botao = 7;  // Pino do pushbutton
//const int pinoServo = 8; // Pino do Servo
bool autorizado = false;

void setup() {
  Serial.begin(9600);
  pinMode(botao, INPUT_PULLUP); // Botão com resistor de pull-up
  cancela.attach(8);  // Pino do servo
  cancela.write(0);   // Cancela fechada
}

void loop() {
  // Lê comandos do computador
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    if (comando == "LIBERADO") {
      autorizado = true;
      Serial.println("Aguardando botão...");
    } else {
      autorizado = false;
      Serial.println("Acesso negado");
    }
  }

  // Verifica botão
  if (autorizado && digitalRead(botao) == LOW) {
    cancela.write(90); // Abrir cancela
    delay(4000);       // Espera 4 segundos
    cancela.write(0);  // Fecha cancela
    autorizado = false;
    Serial.println("Cancela acionada");
  }
}
