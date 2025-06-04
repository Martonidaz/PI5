#include <Servo.h>

Servo meuServo;

const int pinoBotao = 7;
const int pinoServo = 8;

int estadoBotaoAnterior = LOW;
int posicaoAtual = 0;
bool servoAtivo = false;

void setup() {
  pinMode(pinoBotao, INPUT);
  meuServo.attach(pinoServo);
  meuServo.write(posicaoAtual);
  Serial.begin(9600);
}

void loop() {
  int estadoBotaoAtual = digitalRead(pinoBotao);

  // Detecta borda de subida (pressionado)
  if (estadoBotaoAtual == HIGH && estadoBotaoAnterior == LOW) {
    servoAtivo = !servoAtivo; // Alterna estado

    if (servoAtivo) {
      posicaoAtual = 90;
    } else {
      posicaoAtual = 0;
    }

    meuServo.write(posicaoAtual);
    delay(200); // debounce simples
  }

  estadoBotaoAnterior = estadoBotaoAtual;
}
