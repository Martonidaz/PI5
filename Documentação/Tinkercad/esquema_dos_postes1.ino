// Configuração dos pinos
const int ledPins[] = {7, 6, 5, 4, 3, 2};  // Pinos dos LEDs (7 ao 2)
const int ldrPins[] = {A0, A1, A2, A3, A4, A5}; // Pinos dos LDRs
const int numSensores = 6;                 // Quantidade de pares LED/LDR
int ldrValores[numSensores];               // Array para armazenar leituras
const int threshold = 500;                 // Limiar de luminosidade (ajustável)

void setup() {
  Serial.begin(9600);
  
  // Configura todos os pinos dos LEDs como saída
  for (int i = 0; i < numSensores; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);  // Inicia com LEDs apagados
  }
  
  Serial.println("Sistema LDR-LED iniciado...");
}

void loop() {
  // Lê e processa cada par LDR/LED
  for (int i = 0; i < numSensores; i++) {
    ldrValores[i] = analogRead(ldrPins[i]);  // Lê o LDR
    
    // Controle do LED correspondente
    if (ldrValores[i] < threshold) {
      digitalWrite(ledPins[i], HIGH);  // Acende se escuro
    } else {
      digitalWrite(ledPins[i], LOW);   // Apaga se claro
    }
    
    // Exibe os dados no monitor serial
    Serial.print("LDR");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(ldrValores[i]);
    Serial.print("\t");
  }
  Serial.println();  // Nova linha
  
  delay(100);  // Intervalo entre leituras
}