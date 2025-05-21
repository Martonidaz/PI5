const int ledPin = 6;      // Pino do LED
const int ldrPin = A0;     // Pino do LDR (entrada analógica)
int ldrValor = 0;          // Variável para armazenar a leitura do LDR

void setup() {
  pinMode(ledPin, OUTPUT);   // Define o pino do LED como saída
  Serial.begin(9600);        // Inicia a comunicação serial (para monitoramento)
}

void loop() {
  ldrValor = analogRead(ldrPin);  // Lê o valor do LDR
  Serial.println(ldrValor);       // Mostra o valor no monitor serial

  // Se a luminosidade for baixa (valor alto do LDR), acende o LED
  if (ldrValor < 500) {
    digitalWrite(ledPin, HIGH);   // Acende o LED
  } else {
    digitalWrite(ledPin, LOW);    // Apaga o LED
  }

  delay(100);  // Pequeno atraso para estabilidade
}
