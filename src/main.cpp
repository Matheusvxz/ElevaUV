/* 
 *
 *
 * 
 * 
*/

#include <OTA.h>                    // Biblioteca para incluir a funcionalidade Over-the-Air updates

#include <FS.h>                     // Biblioteca para gerenciar arquivos do cartão SD
#include <SD.h>                     // Biblioteca para comunicar com o cartão SD
#include <SPI.h>                    // Biblioteca para a comunicação SPI
#include <ArduinoJson.h>            // Biblioteca para ler arquivos no formato Json

#include <RTClib.h>                 // Biblioteca para comunicar com o módulo RTC DS3231 via I2C
#include <FreqCountESP.h>           // Biblioteca para medir a frequência do sensor Doppler HB100

#include <Adafruit_MPU6050.h>       // Biblioteca para comunicar com o Acelerômetro MPU6050
#include <Adafruit_Sensor.h>        // Biblioteca dependecia para o Acelerômetro MPU6050


/*-----------------------------------------------------------------------------------------------*/
/*------------------------------------------- Objetos -------------------------------------------*/

StaticJsonDocument<100> doc;                // Instância um onjeto do tipo StaticJsonDocument
RTC_DS3231 rtc;                             // Instância um objeto do tipo RTC_DS3231
DateTime agora;                             // Instância um objeto do tipo DateTime
Adafruit_MPU6050 mpu;                       // Instância um objeto do tipo MPU 6050

/*-----------------------------------------------------------------------------------------------*/
/*------------------------------------------ Variáveis ------------------------------------------*/

const char *fileConfig = "/config.txt";

bool OTAEnable = false;
int ledState = LOW;
int dopplerThreshold;
const int timerDoppler = 1000;

unsigned long previousMillis = 0;
const long interval = 2000;

/*-----------------------------------------------------------------------------------------------*/
/*------------------------------------ Definição das funções ------------------------------------*/

bool loadConfig(const char *filename, WifiNetwork &wifi, int &doppler);

/*-----------------------------------------------------------------------------------------------*/
/*-------------------------------------------- Setup --------------------------------------------*/

void setup() {

  pinsSetup();                      // Inicia os pinos do ESP32
  
  Serial.begin(115200);             // Inicia o protocolo serial com baud rate de 115200

  if(!SD.begin(pin_csSDCARD)) {     // Verifica se tem algum cartão SD conectado
    Serial.println("A montagem do cartão falhou!");
    delay(5000);
    ESP.restart();
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE) {       // Tenta conectar com o cartão SD
    Serial.println("Nenhum cartão conectado!");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Inicializando o cartão SD...");

  // Carrega as configurações do arquivo config.txt
  if(!loadConfig(fileConfig, wifi, dopplerThreshold)) {
    delay(5000);
    ESP.restart();
  }
  
  delay(10);
  
  if (! rtc.begin()) {            // Tenta comunicar com o RTC3231
    Serial.println("RTC não foi encontrado");
    delay(5000);
    ESP.restart();
  }
  
  delay(2000);
  
  // Se o botão for pressionado na inicialização o programa entra no modo de
  // atualização. A página de atualização fica disponível em http://elevaUV.local
  if(digitalRead(pin_button) == LOW) {
    Serial.println("Modo de update!");
    setupWIFI();
    OTAEnable = true;
    Serial.println(getNTP());
    rtc.adjust(DateTime(getNTP()));
    setupOTA();
  }

  FreqCountESP.begin(pin_doppler, timerDoppler);  // Inicia a contagem de frequência do doppler
  
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);   // Configura o intervalo de medição de aceleração
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);     // Configura o filtro passa-baixa do acelerômetro

}

/*-----------------------------------------------------------------------------------------------*/
/*-------------------------------------------- Loop ---------------------------------------------*/

void loop() {
  if(OTAEnable) {
    handleOTA();
  } else {

    unsigned long currentMillis = millis();
  
    if (currentMillis - previousMillis >= interval) {
  
      previousMillis = currentMillis;
  
      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      
      Serial.println("LED PISCA");
      digitalWrite(pin_ledVermelho, ledState);
  
      agora = rtc.now();
      
      char dateFormat[] = "YYYY-MM-DD hh:mm:ss";
      Serial.println(agora.toString(dateFormat));
    }
  }
}

/*-----------------------------------------------------------------------------------------------*/
/*------------------------------------------- Funções -------------------------------------------*/

bool loadConfig(const char *filename, WifiNetwork &wifi, int &doppler) {
  File arquivo = SD.open(filename);
  char strDoppler[10];
  if(!arquivo) {
    Serial.println("O arquivo config não foi encontrado");
    return false;
  } else {
    Serial.println("O arquivo config foi encontrado");
  }
  DeserializationError error = deserializeJson(doc, arquivo);
  if (error) {
    Serial.println(F("Falha para ler o arquivo JSON"));
    Serial.println(error.f_str());
  }
  strlcpy(wifi.ssid, doc["wifi"]["ssid"], sizeof(wifi.ssid));
  strlcpy(wifi.pass, doc["wifi"]["pass"], sizeof(wifi.pass));
  strlcpy(strDoppler, doc["config"]["dopplerThreshold"], sizeof(strDoppler));
  doppler = atoi(strDoppler);
  return true;
}
