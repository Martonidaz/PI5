const int led1 = 2;      //Atribui o pino 2 ao LED1
const int led2 = 3;      //Atribui o pino 3 ao LED2
const int led3 = 6;      //Atribui o pino 6 ao LED5
const int led4 = 7;      //Atribui o pino 7 ao LED6        
const int ldr1 = A0;     //Atribui o pino analógico A0 LDR1
const int ldr2 = A2;     //Atribui o pino analógico A2 LDR2

void setup() {
  //Define os 4 LEDs como saída
  pinMode(led1, OUTPUT); 
  pinMode(led2, OUTPUT); 
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Lê valor dos LDRs
  int valorLdr1 = analogRead(ldr1); 
  int valorLdr2 = analogRead(ldr2);

  //Mostra os valores do LDR no monitor serial
  Serial.print("LDR1: ");
  Serial.println(valorLdr1); 

  Serial.print("LDR2: ");
  Serial.println(valorLdr2);

  //Se o valor do LDR for menor que 500, acende os LEDs
  //Se não, os LEDs apagam 
  
  if (valorLdr1 < 500) {
    // Pouca luz: acende os LEDs
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  } else {
    // Muita luz: apaga os LEDs
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  
  if (valorLdr2 < 500) { 
    // Pouca luz: acende os LEDs
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  } else {
    // Muita luz: apaga os LEDs
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  }

  delay(500); // Aguarda meio segundo
}
