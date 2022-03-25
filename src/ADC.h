#include <driver/adc.h>             // Inclui a biblioteca "Analog Digital Converter" nativa do ESP32
#include <esp_adc_cal.h>            // Inclui a biblioteca para auto calibração do ADC

#define DEFAULT_VREF    1072
#define NO_OF_SAMPLES   10

static const adc_channel_t      canal = ADC_CHANNEL_0;          // GPIO 36
static const adc_bits_width_t   resolucao = ADC_WIDTH_BIT_12;

static const adc_atten_t        atten = ADC_ATTEN_DB_11;
static const adc_unit_t         unit = ADC_UNIT_1;

static esp_adc_cal_characteristics_t *adc_chars;

void configADC() {
    adc1_config_width(resolucao);
    adc1_config_channel_atten(ADC1_CHANNEL_0, atten);

    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, resolucao, DEFAULT_VREF, adc_chars);   
}

float readCurrent() {
    float corrent = 0;
    uint32_t leitura_adc = 0;
    for(int i = 0; i < NO_OF_SAMPLES; i++) {
        leitura_adc += adc1_get_raw(ADC1_CHANNEL_0);
    }
    leitura_adc /= NO_OF_SAMPLES;
    int tensão = esp_adc_cal_raw_to_voltage(leitura_adc, adc_chars);
    
}