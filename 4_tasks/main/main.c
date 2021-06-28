#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <driver/gpio.h>

#include <esp_log.h>
#include <esp_system.h>

#define TRUE 1
#define FALSE 0
#define DEBUG true

#define LED_BUILTIN 2
#define GPIO_OUTPUT_PIN_SEL ((1ULL<<LED_BUILTIN))

#define BUTTON 4
#define GPIO_INPUT_PIN_SEL ((1ULL<<BUTTON))

void task_blink(void *pvParameters);
void task_button(void *pvParameters);

const char* led_state[2] = {"on", "off"};
xQueueHandle gpio_evt_queue = NULL;

void app_main(void) {
    gpio_config_t io_conf_output = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .pull_down_en = 0,
        .pull_up_en = 1
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_output))

    gpio_set_level(LED_BUILTIN, 1);

    gpio_config_t io_conf_input = {
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_input));

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (gpio_evt_queue == NULL) {
        if (DEBUG) {
            ESP_LOGI("main", "gpio_evt_queue error...");
        }
        while(TRUE);
    }

    if (xTaskCreate(task_button, "task_button", 2048, NULL, 2, NULL) != pdTRUE) {
        if (DEBUG) {
            ESP_LOGI("main", "task_button error...");
        }
        while(TRUE);
    }

    if (xTaskCreate(task_blink, "task_blink", 2048, NULL, 2, NULL) != pdTRUE) {
        if (DEBUG) {
            ESP_LOGI("main", "task_blink error...");
        }
        while(TRUE);
    }
}

void task_blink(void *pvParameters) {
    uint32_t io_num;

    while(TRUE) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if (DEBUG) {
                ESP_LOGI("task_blink", "led builtin = : %d", io_num % 2);
            }
            gpio_set_level(LED_BUILTIN, io_num % 2);
        }
    }
}

void task_button(void *pvParameters) {
    uint32_t counter = 0;

    while(TRUE) {
        if(!gpio_get_level(BUTTON)) {
            if(DEBUG) {
                ESP_LOGI("task_button", "Button pressed, sending = :%d", (counter % 2));
            }
            if (xQueueSend(gpio_evt_queue, (void*) &counter, (TickType_t) 10) == pdPASS) {
                if(DEBUG) {
                    ESP_LOGI("task_button", "counter = :%d enqueued", (counter % 2));
                }
                counter++;
            }       
        }
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
