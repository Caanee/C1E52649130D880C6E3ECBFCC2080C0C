#include <Arduino.h>
#line 1 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "Cuyi el macho";
const char* password = "Cullisemental";
const char* firmware_url = "https://raw.githubusercontent.com/Caanee/C1E52649130D880C6E3ECBFCC2080C0C/main/OTA.ino.bin";
const char* version_url = "https://raw.githubusercontent.com/Caanee/C1E52649130D880C6E3ECBFCC2080C0C/main/version.txt";

const char* current_version = "0.0.1"; 


unsigned long previousMillis = 0;
const long interval = 10000;

#line 16 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void setup();
#line 28 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void loop();
#line 50 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void reconnectWiFi();
#line 68 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void check_for_update();
#line 94 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
void download_firmware();
#line 16 "C:\\Users\\Sebastian\\OneDrive - UNIVERSIDAD ANDRES BELLO\\Proyecto\\electronica\\ESP32\\OTA\\OTA.ino"
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
unsigned long currentMillis = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida. Intentando reconectar...");
    reconnectWiFi();
  }else{
    Serial.println("Conexion WIFI estable ;)"); 
  }
  
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println("Revisando si funciona ..");
    check_for_update();
  }


  Serial.println("Version Alpha 0.0.1");
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
    Serial.println("Fallo la reconexion a WiFi.");
  }
}


void check_for_update() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(version_url);
    
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String new_version = http.getString();
      new_version.trim();  // Eliminar espacios o saltos de línea adicionales

      if (new_version != current_version) {
        Serial.println("Nueva version disponible: " + new_version);
        download_firmware();
      } else {
        Serial.println("El firmware esto actualizado. No se requiere actualizacion.");
      }
    } else {
      Serial.println("No se pudo verificar la version. Codigo HTTP: " + String(httpCode));
    }
    
    http.end();
  } else {
    Serial.println("No se pudo verificar la version debido a la pérdida de conexion WiFi.");
  }
}

void download_firmware() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Descargando nueva version de firmware...");
    HTTPClient http;
    http.begin(firmware_url);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      int contentLength = http.getSize();
      bool canBegin = Update.begin(contentLength, U_FLASH);

      if (canBegin) {
        Serial.println("Comenzando la actualizacion OTA...");
        size_t written = Update.writeStream(http.getStream());

        if (written == contentLength) {
          Serial.println("El firmware descargado esto escrito completamente en la memoria flash");
        } else {
          Serial.println("El firmware no se ha descargado correctamente en la memoria flash");
        }

        if (Update.end()) {
          Serial.println("Actualizacion completada.");
          if (Update.isFinished()) {
            Serial.println("Reiniciando...");
            ESP.restart();  
          } else {
            Serial.println("La actualizacion no se ha completado.");
          }
        } else {
          Serial.println("Error durante la actualizacion #: " + String(Update.getError()));
        }
      } else {
        Serial.println("No se puede iniciar la actualizacion OTA.");
      }
    } else {
      Serial.println("No se puede conectar al servidor de actualizacion. Codigo HTTP: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("Conexion WiFi perdida.");
  }
}
