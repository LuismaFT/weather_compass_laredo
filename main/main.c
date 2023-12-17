

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#include "gpio.h"
#include "adc.h"
#include "utils.h"
#include "http.h"

#define TAG "SSD1306"

#define GAFAS_DE_SOL    GPIO14  // D12
#define PANTALON_CORTO  GPIO27  // D14
#define JERSEY          GPIO26  // D19
#define PARAGUAS        GPIO25  // D28
#define VENTANA         GPIO33  // TX2
#define SILENCIO        GPIO32  // RX2

uint8_t corto[] = {
		
};

uint8_t jersey[] = {
		
};

typedef struct value_and_voltage vav;

void init_gpio(void);

// punto de entrada de la aplicación
void app_main(void)
{
    struct value_and_voltage vav;

    int co2_value = 0;
    int co2_voltage = 0;
    int noise_value = 0;
    int noise_voltage= 0;

    int lluvia = 0;
    int luz = 0;
    int temp = 0;
    int co2 = 0;
    int ruido = 0;

    adc_init();
    init_http();
    init_gpio();

#ifdef OLED
	SSD1306_t dev;
	ESP_LOGI(TAG, "INTERFACE is i2c");
	ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
	dev._flip = true;
	ESP_LOGW(TAG, "Flip upside down");
	ESP_LOGI(TAG, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
	ssd1306_contrast(&dev, 0xff);
#endif

    while (1) {

        adc_read_value(ADC1_CHAN6, &vav);
        co2_value = vav.value;
        co2_voltage = vav.voltage;
        adc_read_value(ADC1_CHAN7, &vav);
        noise_value = vav.value;
        noise_voltage = vav.voltage;
        //co2 = analogToPPM(co2_value);
        //co2 = co2_value;

        ruido = noise_value;
        struct url_data data = {"chAU4l9GZJVQTTh2LIci/telemetry", "co2_value", "", co2_value, ""};
        http_post(&data);
        struct url_data data2 = {"chAU4l9GZJVQTTh2LIci/telemetry", "noise_value", "", noise_value, ""};
        http_post(&data2);
        printf("ruido = %d\n", ruido);
        printf("co2 = %d\n", co2_value);

        struct url_data agua = {"chAU4l9GZJVQTTh2LIci/attributes", "", "clientKeys=agua", 0};
        http_get(&agua);
        vTaskDelay(pdMS_TO_TICKS(500));
        lluvia = value_from_json(agua.resp);
        printf("agua = %d\n", lluvia);

        struct url_data lumenes = {"chAU4l9GZJVQTTh2LIci/attributes", "", "clientKeys=lumenes", 0};
        http_get(&lumenes);
        vTaskDelay(pdMS_TO_TICKS(500));
        luz = value_from_json(lumenes.resp);
        printf("lumenes = %d\n", luz);

        struct url_data temperatura = {"chAU4l9GZJVQTTh2LIci/attributes", "", "clientKeys=temperature", 0};
        http_get(&temperatura);
        vTaskDelay(pdMS_TO_TICKS(500));
        temp = value_from_json(temperatura.resp);
        printf("temperatura = %d\n", temp);

        // Luminosidad
        if (luz > 50) {
            gpio_set_level(GAFAS_DE_SOL, HIGH);
        } else {
            gpio_set_level(GAFAS_DE_SOL, LOW);
		}
		
        // Temperatura
        if (temp > 30) {
            gpio_set_level(JERSEY, LOW);
            gpio_set_level(PANTALON_CORTO, HIGH);
            
            #ifdef OLED
            ssd1306_clear_screen(&dev, false);
            ssd1306_bitmaps(&dev, 0, 0, corto, 32, 13, false);
			#endif
        } else if (temp < 22) {
            gpio_set_level(PANTALON_CORTO, LOW);
            gpio_set_level(JERSEY, HIGH);
            
            #ifdef OLED
            ssd1306_clear_screen(&dev, false);
            ssd1306_bitmaps(&dev, 0, 0, jersey, 32, 13, false);
            #endif
        } else {
        	#ifdef OLED
        	ssd1306_clear_screen(&dev, false);
        	#endif
            gpio_set_level(PANTALON_CORTO, LOW);
            gpio_set_level(JERSEY, LOW);
        }

        // Lluvia
        if (lluvia > 0) {
            gpio_set_level(PARAGUAS, HIGH);
        } else {
            gpio_set_level(PARAGUAS, LOW);
        }

        // Co2
        if (co2 > 1000) {
            gpio_set_level(VENTANA, HIGH);
        } else {
            gpio_set_level(VENTANA, LOW);
        }

        // Ruido
        if (ruido > 100) {
            gpio_set_level(SILENCIO, HIGH);
        } else {
            gpio_set_level(SILENCIO, LOW);
        }
		
    }

    adc_terminate();
}

void init_gpio(void) {
    gpio_set_mode(PANTALON_CORTO, OUTPUT);
    gpio_set_mode(JERSEY, OUTPUT);
    gpio_set_mode(PARAGUAS, OUTPUT);
    gpio_set_mode(GAFAS_DE_SOL, OUTPUT);
    gpio_set_mode(VENTANA, OUTPUT);
    gpio_set_mode(SILENCIO, OUTPUT);

    gpio_set_level(PANTALON_CORTO, HIGH);
    gpio_set_level(JERSEY, HIGH);
    gpio_set_level(PARAGUAS, HIGH);
    gpio_set_level(GAFAS_DE_SOL, HIGH);
    gpio_set_level(VENTANA, HIGH);
    gpio_set_level(SILENCIO, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(PANTALON_CORTO, LOW);
    gpio_set_level(JERSEY, LOW);
    gpio_set_level(PARAGUAS, LOW);
    gpio_set_level(GAFAS_DE_SOL, LOW);
    gpio_set_level(VENTANA, LOW);
    gpio_set_level(SILENCIO, LOW);
}

