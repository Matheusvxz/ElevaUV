#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>
#include <NTPClient.h>
#include "html_variables.h"
#include "define_pins.h"

String hostname = "ElevaUV-0001";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3*3600);
WebServer server(80);

typedef struct {
  char ssid[30];
  char pass[30];
} WifiNetwork;

WifiNetwork wifi;

void setupWIFI() {

  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str());
  WiFi.begin(wifi.ssid, wifi.pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Conexão falhou! Reiniciando...");
    delay(5000);
    ESP.restart();
  }
  
  Serial.println("Wifi conectado");
  
  Serial.print("Endereço IP: ");
  digitalWrite(pin_ledVerde, HIGH);
  digitalWrite(pin_ledVermelho, HIGH);
  digitalWrite(pin_ledAzul, HIGH);
  Serial.println(WiFi.localIP());
}

void setupOTA() {
  if (!MDNS.begin("elevaUV")) { //http://elevaUV.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void handleOTA() {
  server.handleClient();
}

uint32_t getNTP() {
  timeClient.update();
  return timeClient.getEpochTime();
}
