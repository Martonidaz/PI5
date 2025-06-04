const int ledPins[] = {2, 3, 4, 5, 6, 7};
const int ldrPins[] = {A0, A1, A2};
const int NUM_LEDS = 6;
const int NUM_LDRS = 3;
const int LDR_THRESHOLD = 500;

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  Serial.begin(9600);
  while (!Serial) {
    ; // Espera a porta serial conectar (necessário para algumas placas)
  }
}

void loop() {
  // Lê valores dos LDRs
  int ldrValues[NUM_LDRS];
  for (int i = 0; i < NUM_LDRS; i++) {
    ldrValues[i] = analogRead(ldrPins[i]);
  }
  
  // Determina estados dos LEDs (cada par de LEDs controlado por um LDR)
  int ledStates[NUM_LEDS];
  ledStates[0] = (ldrValues[0] < LDR_THRESHOLD) ? HIGH : LOW;
  ledStates[1] = ledStates[0]; // Mesmo LDR controla LED1 e LED2
  ledStates[2] = (ldrValues[1] < LDR_THRESHOLD) ? HIGH : LOW;
  ledStates[3] = ledStates[2]; // Mesmo LDR controla LED3 e LED4
  ledStates[4] = (ldrValues[2] < LDR_THRESHOLD) ? HIGH : LOW;
  ledStates[5] = ledStates[4]; // Mesmo LDR controla LED5 e LED6
  
  // Aplica estados aos LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], ledStates[i]);
  }

  // Envia dados formatados via serial (formato JSON-like para facilitar parsing)
  Serial.print("{"); // Início do JSON
  Serial.print("\"leds\":[");
  for (int i = 0; i < NUM_LEDS; i++) {
    Serial.print(ledStates[i]);
    if (i < NUM_LEDS - 1) Serial.print(",");
  }
  Serial.print("],\"ldrs\":[");
  for (int i = 0; i < NUM_LDRS; i++) {
    Serial.print(ldrValues[i]);
    if (i < NUM_LDRS - 1) Serial.print(",");
  }
  Serial.print("]}");
  Serial.println(); // Finaliza a linha
  
  delay(1000); // Intervalo entre leituras
}
