#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

const char* ssid = "Iphone Mari";
const char* password = "123456789";
const char* serverName = "http://172.20.10.3:5000/sensor"; // IP do Flask
const char* ntpServer = "a.st1.ntp.br";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

const int pinoSensor = D2;

void setup() {
  Serial.begin(115200);
  pinMode(pinoSensor, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando ao Wi-Fi...");
    delay(1000);
  }
  Serial.println("Wi-Fi conectado");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Aguardando sincronização NTP...");
    delay(1000);
  }
  Serial.println("Horário sincronizado");
}

void loop() {
  int movimento = digitalRead(pinoSensor);
  String descricao = (movimento == 1) ? "Movimento Detectado" : "Sem Movimento";

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char dataHora[25];
  strftime(dataHora, sizeof(dataHora), "%Y-%m-%d %H:%M:%S", &timeinfo);

  String json = "{\"movimento\":" + String(movimento) +
                ",\"descricao\":\"" + descricao +
                "\",\"data_hora\":\"" + String(dataHora) + "\"}";

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(json);
    Serial.println(json);
    Serial.println("Resposta HTTP: " + String(httpResponseCode));
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado");
  }

  delay(10000); // 10 segundos
}
