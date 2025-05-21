#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

// Definição dos pinos
int ldrPin = A0;
int ledPin = 6;
int limiar = 500;
int valorLuz = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // Inicializa o RTC
  if (!rtc.begin()) {
    Serial.println("Não foi possível encontrar o módulo RTC!");
    while (1);
  }

  // Ajusta a hora e data uma vez, depois comente esta linha
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (rtc.lostPower()) {
    Serial.println("RTC perdeu a energia, ajustando hora...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  valorLuz = analogRead(ldrPin);
  DateTime agora = rtc.now(); // Pega a data e hora atuais

  Serial.print("Luz: ");
  Serial.print(valorLuz);
  Serial.print(" | Data: ");
  Serial.print(agora.day());
  Serial.print("/");
  Serial.print(agora.month());
  Serial.print("/");
  Serial.print(agora.year());
  Serial.print(" Hora: ");
  Serial.print(agora.hour());
  Serial.print(":");
  Serial.print(agora.minute());
  Serial.print(":");
  Serial.println(agora.second());

  if (valorLuz < limiar) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(1000); // Espera 1 segundo
}
