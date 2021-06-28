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

#define BUTTON 4
#define GPIO_INPUT_PIN_SEL ((1ULL<<BUTTON))

void gpio_isr_handler(void *arg);

const char* led_state[2] = {"on", "off"};

void gpio_isr_handler(void *arg) {
    static int counter = 0;
    gpio_set_level(LED_BUILTIN, counter++ % 2);
}

int app_main(void) {
    gpio_config_t io_conf_output = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .pull_down_en = 0,
        .pull_up_en = 1 
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf_output));   

    gpio_set_level(LED_BUILTIN, 1);

    gpio_config_t io_conf_input = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_input));

    gpio_install_isr_service(0);

    gpio_isr_handler_add(BUTTON, gpio_isr_handler, NULL);

    while(TRUE) {
        ESP_LOGI("main", "Program running...");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}