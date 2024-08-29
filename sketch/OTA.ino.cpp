#include <Arduino.h>
#line 1 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "Cuyi el macho";
const char* password = "Cullisemental";
const char* firmware_url = "archivo de la github .bin";

unsigned long previousMillis = 0;
const long interval = 60000;

#line 12 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void setup();
#line 24 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void loop();
#line 37 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void reconnectWiFi();
#line 54 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void download_firmware();
#line 12 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void setup() {
  Serial.begin(921600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a WiFi");
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida. Intentando reconectar...");
    reconnectWiFi();
  }else{
    Serial.println("Conexion WIFI estable ;)"); 
  }
  download_firmware();
  Serial.println("Version Alpha 0.1v");
  delay(5000);
}

void reconnectWiFi() {
  WiFi.disconnect();
  WiFi.reconnect();
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) { // 10 segundos de espera
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Re-conectado a WiFi!");
  } else {
    Serial.println("Falló la reconexión a WiFi.");
  }
}

void download_firmware(){
    unsigned long currentMillis = millis();
  
  // Verifica si ha pasado el intervalo para buscar una actualización
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Buscando actualizaciones...");
      HTTPClient http;
      http.begin(firmware_url);

      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        bool canBegin = Update.begin(contentLength, U_FLASH);

        if (canBegin) {
                      Serial.println("Comenzando la actualización OTA...");
                      size_t written = Update.writeStream(http.getStream());

                      if (written == contentLength) {
                        //Serial.println("Escrito: " + String(written) + " bytes exitosamente.");
                        Serial.println("El firmware descargado esta escrito completo en la memoria flash");
                      } else {
                        //Serial.println("Escrito solo: " + String(written) + "/" + String(contentLength));
                        Serial.println("El firmware no se ha descargado correctamente en la memoria flash");
                      }

                      if (Update.end()) {
                        Serial.println("Actualización completada.");
                        if (Update.isFinished()) {
                          Serial.println("Reiniciando...");
                          ESP.restart();  
                        } else {
                          Serial.println("La actualización no se ha completado.");
                        }
                      } else {
                        Serial.println("Error durante la actualización #: " + String(Update.getError()));
                      }
                      } else {
                        Serial.println("No se puede iniciar la actualización OTA.");
                      }
      } else {
        Serial.println("No se puede conectar al servidor de actualización. Código HTTP: " + String(httpCode));
      }
      http.end();
    } else {
      Serial.println("Conexión WiFi perdida.");
    }
  } 
}
