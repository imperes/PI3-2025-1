#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "mirf.h"

#define SPP_TAG "SPP_NRF_BRIDGE"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP32_BT_NRF"
#define NRF_PAYLOAD_SIZE 32

// Fila para passar comandos Bluetooth
static QueueHandle_t nrf_queue = NULL;

// NRF24 handler
static NRF24_t nrf_dev;

// Handle do cliente Bluetooth conectado
static uint32_t spp_client_handle = 0;

// NRF Task
void nrf_task(void *pvParameters)
{
    uint8_t current_message[NRF_PAYLOAD_SIZE] = {0};
    bool recebeu_ok = false;

    while (1) {
        // Primeiro espera novo comando Bluetooth
        ESP_LOGI(SPP_TAG, "Aguardando dados do Bluetooth...");
        if (xQueueReceive(nrf_queue, current_message, portMAX_DELAY)) {
            ESP_LOGI(SPP_TAG, "Iniciando transmissao ate receber 'ok': %s", current_message);
            recebeu_ok = false;

            while (!recebeu_ok) {
                // 1. Transmitir
                Nrf24_send(&nrf_dev, current_message);
                if (Nrf24_isSend(&nrf_dev, 1000)) {
                    ESP_LOGI(SPP_TAG, "NRF transmitido");
                } else {
                    ESP_LOGW(SPP_TAG, "Falha ao transmitir");
                }

                // 2. Modo RX para aguardar resposta
                Nrf24_powerUpRx(&nrf_dev);
                vTaskDelay(pdMS_TO_TICKS(50)); // Aguarda 500ms

                // 3. Verifica se recebeu algo
                if (Nrf24_dataReady(&nrf_dev)) {
                    uint8_t rx_buf[NRF_PAYLOAD_SIZE] = {0};
                    Nrf24_getData(&nrf_dev, rx_buf);
                    ESP_LOGI(SPP_TAG, "Recebido via NRF: %s", rx_buf);

                    if (strncmp((char *)rx_buf, "ok", 2) == 0) {
                        ESP_LOGI(SPP_TAG, "o motorista estah a caminho");
                        recebeu_ok = true;

                        // Envia 'ok' ao celular via Bluetooth
                        if (spp_client_handle != 0) {
                            const char *mensagem_ok = "ok";
                            esp_err_t ret = esp_spp_write(spp_client_handle, strlen(mensagem_ok), (uint8_t *)mensagem_ok);
                            if (ret == ESP_OK) {
                                ESP_LOGI(SPP_TAG, "Enviado 'ok' ao celular via Bluetooth");
                            } else {
                                ESP_LOGW(SPP_TAG, "Falha ao enviar via Bluetooth (%d)", ret);
                            }
                        } else {
                            ESP_LOGW(SPP_TAG, "Nenhum cliente Bluetooth conectado");
                        }

                        break; // Sai do loop de envio
                    }
                }

                // 4. Antes de aguardar 5s, verifica se chegou novo comando
                if (uxQueueMessagesWaiting(nrf_queue) > 0) {
                    // Novo comando recebido, pega ele imediatamente
                    ESP_LOGI(SPP_TAG, "Novo comando chegou! Interrompendo envio atual.");
                    memset(current_message, 0, NRF_PAYLOAD_SIZE);
                    xQueueReceive(nrf_queue, current_message, 0);
                    ESP_LOGI(SPP_TAG, "Novo comando: %s", current_message);
                    // Reinicia ciclo
                    continue;
                }

                // 5. Aguardar 5s antes de retransmitir
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
        }
    }
}

// Bluetooth SPP callback
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, SPP_SERVER_NAME);
        ESP_LOGI(SPP_TAG, "SPP server started");
        break;

    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "BT client connected, handle=%"PRIu32, param->srv_open.handle);
        spp_client_handle = param->srv_open.handle;
        break;

    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(SPP_TAG, "Recebido via Bluetooth (%d bytes)", param->data_ind.len);

        // Controle do LED
        if (param->data_ind.len > 0) {
            char cmd = param->data_ind.data[0];
            if (cmd == 'A') {
                gpio_set_level(GPIO_NUM_2, 1);
                ESP_LOGI(SPP_TAG, "LED ON");
            } else if (cmd == 'a') {
                gpio_set_level(GPIO_NUM_2, 0);
                ESP_LOGI(SPP_TAG, "LED OFF");
            }
        }

        // Copiar para fila
        uint8_t temp_buf[NRF_PAYLOAD_SIZE] = {0};
        size_t len = param->data_ind.len > NRF_PAYLOAD_SIZE ? NRF_PAYLOAD_SIZE : param->data_ind.len;
        memcpy(temp_buf, param->data_ind.data, len);

        if (xQueueSend(nrf_queue, temp_buf, 0) != pdPASS) {
            ESP_LOGW(SPP_TAG, "Queue cheia, descartando dado");
        }
        break;

    default:
        break;
    }
}

// Bluetooth GAP callback
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    // Pode deixar vazio ou adicionar logs se desejar
}

// app_main
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_bt_gap_register_callback(esp_bt_gap_cb));
    ESP_ERROR_CHECK(esp_spp_register_callback(esp_spp_cb));

    esp_spp_cfg_t spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = false,
        .tx_buffer_size = 0,
    };
    ESP_ERROR_CHECK(esp_spp_enhanced_init(&spp_cfg));

    esp_bt_gap_set_device_name(EXAMPLE_DEVICE_NAME);
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    ESP_LOGI(SPP_TAG, "Bluetooth SPP inicializado");

    // Configura GPIO do LED
    gpio_reset_pin(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0);

    // Inicializa NRF24
    Nrf24_init(&nrf_dev);
    Nrf24_config(&nrf_dev, 10, NRF_PAYLOAD_SIZE);
    if (Nrf24_setTADDR(&nrf_dev, (uint8_t *)"FGHIJ") != ESP_OK) {
        ESP_LOGE(SPP_TAG, "NRF24 nao instalado corretamente");
        while (1) { vTaskDelay(1); }
    }
    Nrf24_printDetails(&nrf_dev);

    // Cria fila
    nrf_queue = xQueueCreate(5, NRF_PAYLOAD_SIZE);

    // Cria tarefa NRF
    xTaskCreate(nrf_task, "NRF_TASK", 4096, NULL, 5, NULL);

    ESP_LOGI(SPP_TAG, "Sistema pronto. Conecte via BT e envie comandos.");
}
