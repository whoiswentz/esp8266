#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"

#define TRUE 1
#define FALSE 0
#define DEBUG true
#define LED_BUILTIN 2
#define GPIO_OUTPUT_PIN_SEL ((1ULL<<LED_BUILTIN))

const char* led_state[2] = {"on", "off"};

void app_main(void) {
	int counter = 0;

	gpio_config_t io_conf;

	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 1;

	gpio_config(&io_conf);

	gpio_set_level(LED_BUILTIN, 1);

	while(TRUE) {
		counter += 1;
		if (DEBUG) {
			ESP_LOGI("main", "led builtin = : %s", led_state[counter % 2]);
		}
		gpio_set_level(LED_BUILTIN, counter % 2);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}