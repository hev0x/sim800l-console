#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define SIM800L_TX 27
#define SIM800L_RX 26
#define SIM800L_PWRKEY 4
#define SIM800L_RST 5
#define SIM800L_POWER 23

#define LINE_MAX	100
#define UART_NUM UART_NUM_1
#define BUF_SIZE 1024
#define TAG "SIM800L"

static void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 4800,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, SIM800L_TX, SIM800L_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}


static void send_at_command(const char *command) {
    uint8_t data[BUF_SIZE];
    int length;

    ESP_LOGI(TAG, "Sending command: %s", command);
    uart_write_bytes(UART_NUM, command, strlen(command));
    uart_write_bytes(UART_NUM, "\r\n", 2);

    vTaskDelay(pdMS_TO_TICKS(5000));

    length = uart_read_bytes(UART_NUM, data, BUF_SIZE, pdMS_TO_TICKS(1000));
    if (length > 0) {
        data[length] = '\0';
        ESP_LOGI(TAG, "Response: %s", (char *)data);
    } else {
        ESP_LOGW(TAG, "No response received.");
    }
}

void print_prompt() {
	printf("\nSIM800L> ");
	fflush(stdout);
}

static void gsm_console_task(void *pvParameters) {
    char line_ctrl[LINE_MAX];
    int line_pos = 0;

    /* SIM800L Modem startup */
    gpio_set_direction(SIM800L_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(SIM800L_POWER, 1); 
    vTaskDelay(pdMS_TO_TICKS(10000)); 
    ESP_LOGI(TAG, "Initializing communication with SIM800L modem...");
    init_uart();

    print_prompt();

    while (1) {
        int c = getchar();
        if (c < 0) {
            vTaskDelay(pdMS_TO_TICKS(10)); 
            continue;
        }

        if (c == '\r') continue; 

        if (c == '\n') { 
            line_ctrl[line_pos] = '\0'; 

            if (line_pos > 0) {
                printf("\n"); 
                send_at_command(line_ctrl); 
            } else {
                printf("\nNo command entered!\n");
            }

            line_pos = 0;
            print_prompt();
        } else if (c == 127 || c == 8) {
            if (line_pos > 0) {
                printf("\b \b"); 
                fflush(stdout); 
                line_pos--; 
            }
        } else { 
            if (line_pos < LINE_MAX - 1) {
                putchar(c);
                fflush(stdout);
                line_ctrl[line_pos++] = c; 
            } else {
                printf("\nCommand buffer full! Resetting...\n");
                line_pos = 0; 
                print_prompt();
            }
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing TTGO T-Call ESP32 with SIM800L (Console)");
    xTaskCreate(gsm_console_task, "gsm_console_task", 4096, NULL, 5, NULL);
}
