const int ledPin1 = 2;     // LED 1 no pino digital 2
const int ledPin2 = 3;     // LED 2 no pino digital 3
const int ldrPin = A0;     // LDR no pino analógico A0

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int ldrValue = analogRead(ldrPin); // Lê valor do LDR

  Serial.print("LDR: ");
  Serial.println(ldrValue);

  if (ldrValue < 300) {
    // Pouca luz: acende os LEDs
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
  } else {
    // Muita luz: apaga os LEDs
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
  }

  delay(500); // Aguarda meio segundo
}
