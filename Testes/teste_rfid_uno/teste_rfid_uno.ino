//Bibliotecas
#include <SPI.h>
#include <MFRC522.h>

//Pinos
#define SS_PIN 10
#define RST_PIN 9

//RFID: SDA = 10, SCK = 13, MOSI = 11, MISO = 12, RST = 9
//Servomotor: 5

String IDtag = ""; //Variável que armazenará o ID da Tag
bool Permitido = false; //Variável que verifica a permissão 

//Vetor responsável por armazenar os ID's das Tag's cadastradas
String TagsCadastradas[] = {"73f2b3e"};

MFRC522 LeitorRFID(SS_PIN, RST_PIN);    // Cria uma nova instância para o leitor e passa os pinos como parâmetro

void setup() {
        Serial.begin(9600);             // Inicializa a comunicação Serial
        SPI.begin();                    // Inicializa comunicacao SPI 
        LeitorRFID.PCD_Init();          // Inicializa o leitor RFID
}

void loop() {  
  Leitura();  //Chama a função responsável por fazer a leitura das Tag's
}
void Leitura(){
        IDtag = ""; //Inicialmente IDtag deve estar vazia.
        
        // Verifica se existe uma Tag presente
        if ( !LeitorRFID.PICC_IsNewCardPresent() || !LeitorRFID.PICC_ReadCardSerial() ) {
            delay(50);
            return;
        }
        
        // Pega o ID da Tag através da função LeitorRFID.uid e Armazena o ID na variável IDtag        
        for (byte i = 0; i < LeitorRFID.uid.size; i++) {        
            IDtag.concat(String(LeitorRFID.uid.uidByte[i], HEX));
        }        
        
        //Compara o valor do ID lido com os IDs armazenados no vetor TagsCadastradas[]
        for (int i = 0; i < (sizeof(TagsCadastradas)/sizeof(String)); i++) {
          if(  IDtag.equalsIgnoreCase(TagsCadastradas[i])  ){
              Permitido = true; //Variável Permitido assume valor verdadeiro caso o ID Lido esteja cadastrado
          }
        }       
        if(Permitido == true){
          Serial.println("Acesso permitido a tag: " + IDtag);
          delay(5000);
        }        
        else{
          Serial.println("Acesso Negado a tag: " + IDtag);
          delay(100); //aguarda 2 segundos para efetuar uma nova leitura
        }
}
