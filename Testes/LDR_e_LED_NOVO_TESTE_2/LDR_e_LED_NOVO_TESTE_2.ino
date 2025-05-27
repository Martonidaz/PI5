const int ledPin1 = 2;      //Atribui o pino 2 ao LED1
const int ledPin2 = 3;      //Atribui o pino 3 ao LED2
const int ledPin3 = 4;      //Atribui o pino 4 ao LED3
const int ledPin4 = 5;      //Atribui o pino 5 ao LED4
const int ledPin5 = 6;      //Atribui o pino 6 ao LED5
const int ledPin6 = 7;      //Atribui o pino 7 ao LED6        
const int ldrPin1 = A0;     //Atribui o pino analógico A0 LDR1
const int ldrPin2 = A1;     //Atribui o pino analógico A1 LDR2
const int ldrPin3 = A2;     //Atribui o pino analógico A2 LDR3

void setup() {
  //Define os 6 LEDs como saída
  pinMode(ledPin1, OUTPUT); 
  pinMode(ledPin2, OUTPUT); 
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Lê valor dos LDRs
  int ldrValue1 = analogRead(ldrPin1); 
  int ldrValue2 = analogRead(ldrPin2);
  int ldrValue3 = analogRead(ldrPin3);

  //Mostra os valores do LDR no monitor serial
  Serial.print("LDR1: ");
  Serial.println(ldrValue1); 

  Serial.print("LDR2: ");
  Serial.println(ldrValue2);

  Serial.print("LDR3: ");
  Serial.println(ldrValue3);

  //Se o valor do LDR for menor que 500, acende os LEDs
  //Se não, os LEDs apagam 
  if (ldrValue3 < 500) { 
    // Pouca luz: acende os LEDs
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
  } else {
    // Muita luz: apaga os LEDs
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
  }

  if (ldrValue2 < 500) {
    // Pouca luz: acende os LEDs
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
  } else {
    // Muita luz: apaga os LEDs
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
  }

  if (ldrValue1 < 500) {
    // Pouca luz: acende os LEDs
    digitalWrite(ledPin5, HIGH);
    digitalWrite(ledPin6, HIGH);
  } else {
    // Muita luz: apaga os LEDs
    digitalWrite(ledPin5, LOW);
    digitalWrite(ledPin6, LOW);
  }

  delay(500); // Aguarda meio segundo
}
