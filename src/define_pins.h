#include <Arduino.h>
// Pinos Sensor DHT11
#define pin_dataDHT11   4

// Pinos SD Card
#define pin_csSDCARD    5
#define pin_sckSDCARD   18
#define pin_misoSDCARD  19
#define pin_mosiSDCARD  23

// Pinos RTC e MPU6050
#define pin_intMPU6050  15
#define pin_sqwDS3231   16
#define pin_scl         22
#define pin_sda         21

// Pino botão de configuração
#define pin_button      1

// Pinos sinalização
#define pin_ledVerde    32
#define pin_ledVermelho 33
#define pin_ledAzul     25

// Pino sensor doppler
#define pin_doppler     12

// Pinos sensores indutivos
#define pin_ind1        27
#define pin_ind2        14

// Pino SSR
#define pin_ssr         26

// Pino sensor de corrente
#define pin_current     36

// Pinos sensores de presença
#define pin_presenca1   34
#define pin_presenca2   35
#define pin_presenca3   39

void pinsSetup() {
  pinMode(pin_button, INPUT_PULLUP);
  pinMode(pin_ind1, INPUT);
  pinMode(pin_ind2, INPUT);
  pinMode(pin_doppler, INPUT);
  pinMode(pin_current, INPUT);
  pinMode(pin_presenca1, INPUT);
  pinMode(pin_presenca2, INPUT);
  pinMode(pin_presenca3, INPUT);
  pinMode(pin_ledVerde, OUTPUT);
  pinMode(pin_ledVermelho, OUTPUT);
  pinMode(pin_ledAzul, OUTPUT);
  pinMode(pin_ssr, OUTPUT);

  digitalWrite(pin_ledVerde, LOW);
  digitalWrite(pin_ledVermelho, LOW);
  digitalWrite(pin_ledAzul, LOW);
  digitalWrite(pin_ssr, LOW);
}
