#include <stdio.h>
#include <driver/gpio.h>
#include <driver/pwm.h>
#include <esp_event.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_log.h"

#define GPIO_TTL 14
#define GPIO_IR_RECEIVER 12

static const char *TAG = "main";

void setupGpioOutput();

void setupGpioInput();

static void IRAM_ATTR infrared_handler(void *arg) {
    if (gpio_get_level(GPIO_IR_RECEIVER) == 1) {
        pwm_stop(0x00);
    } else {
        pwm_start();
    }
}

void run() {
    setupGpioOutput();
    setupGpioInput();
}

void setupGpioInput() {
    gpio_config_t inputCfg;
    inputCfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    inputCfg.pull_up_en = GPIO_PULLUP_DISABLE;
    inputCfg.intr_type = GPIO_INTR_ANYEDGE;
    inputCfg.mode = GPIO_MODE_INPUT;
    inputCfg.pin_bit_mask = 1 << GPIO_IR_RECEIVER;
    gpio_config(&inputCfg);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_IR_RECEIVER, infrared_handler, NULL);
}


uint32_t gpios[1] = {GPIO_TTL};
uint32_t duty[1] = {13};
float phase[1] = {-90};

void setupGpioOutput() {
    // start pwm cycle
    /*enable tsf0 interrupt for pwm*/
    REG_WRITE(PERIPHS_DPORT_BASEADDR, (REG_READ(PERIPHS_DPORT_BASEADDR) & ~0x1F) | 0x1);
    REG_WRITE(INT_ENA_WDEV, REG_READ(INT_ENA_WDEV) | WDEV_TSF0_REACH_INT);

    pwm_init(24, duty, 1, gpios);
    pwm_set_phases(phase);
}

void app_main() {
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
//    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
//    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
//    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
//    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
//    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
//    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
    run();

//    ESP_ERROR_CHECK(nvs_flash_init());
//    ESP_ERROR_CHECK(esp_netif_init());
//    ESP_ERROR_CHECK(esp_event_loop_create_default());
//
//    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
//     * Read "Establishing Wi-Fi or Ethernet Connection" section in
//     * examples/protocols/README.md for more information about this function.
//     */
//    ESP_ERROR_CHECK(example_connect());
}
