#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_spp_api.h"

#define LED_GPIO        22
#define DEVICE_NAME     "VisuTech"

static const char *TAG = "BT_APP";

void led_control(char cmd) {
    if (cmd == 'A') {
        gpio_set_level(LED_GPIO, 1);
        ESP_LOGI(TAG, "LED ON");
    } else if (cmd == 'a') {
        gpio_set_level(LED_GPIO, 0);
        ESP_LOGI(TAG, "LED OFF");
    }
}

void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_DATA_IND_EVT:
            for (int i = 0; i < param->data_ind.len; i++) {
                led_control(param->data_ind.data[i]);
            }
            break;
        default:
            break;
    }
}

void app_main(void) {
    esp_err_t ret;

    // Inicializa NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }

    // Inicializa GPIO do LED
    gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 0); // LED inicialmente desligado

    // Inicializa Bluetooth
    esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    esp_bluedroid_init();
    esp_bluedroid_enable();

    // Inicializa Bluetooth SPP
    esp_spp_register_callback(spp_callback);
    esp_spp_init(ESP_SPP_MODE_CB);
    esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, DEVICE_NAME);

    ESP_LOGI(TAG, "Bluetooth SPP initialized. Waiting for connections...");
}
