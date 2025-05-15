// Definição dos pinos dos LEDs
int ldrPin = A0;       // Pino onde o LDR está conectado
int ledPin = 6;        // LED no pino digital 9
int limiar = 500;      // Valor limite de luz (ajustável)
int valorLuz = 0;
//int led2 = 7;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);  // Para monitorar o valor lido
}

void loop() {
  valorLuz = analogRead(ldrPin);  // Lê valor de 0 a 1023
  Serial.print("Luz: ");
  Serial.println(valorLuz);

  if (valorLuz < limiar) {
    digitalWrite(ledPin, HIGH);  // Ambiente escuro → acende LED
  } else {
    digitalWrite(ledPin, LOW);   // Ambiente claro → apaga LED
  }

  delay(1000); // Espera 200 ms
}
