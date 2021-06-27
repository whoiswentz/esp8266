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

#define BUTTON 2
#define GPIO_INPUT_PIN_SEL ((1ULL<<BUTTON))

const char* led_state[2] = {"on", "off"};

void app_main(void) {
    int counter = 0;

    gpio_config_t io_conf_output;
    io_conf_output.intr_type = GPIO_INTR_DISABLE;
    io_conf_output.mode = GPIO_MODE_OUTPUT;
    io_conf_output.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf_output.pull_down_en = 0;
    io_conf_output.pull_up_en = 1;
    gpio_config(&io_conf_output);

    gpio_set_level(LED_BUILTIN, 1);

    gpio_config_t io_conf_input;
    io_conf_input.intr_type = GPIO_INTR_DISABLE;
    io_conf_input.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf_input.mode = GPIO_MODE_INPUT;
    io_conf_input.pull_up_en = 1;
    gpio_config(&io_conf_input);

    while(TRUE) {
        if(!gpio_get_level(BUTTON)) {
            if(DEBUG) {
                ESP_LOGI("main", "led builtin = : %s", led_state[counter % 2]);
            }   
            gpio_set_level(LED_BUILTIN, counter++ % 2);
        }
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
