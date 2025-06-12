/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_timer.h"
#include "sys/time.h"  // Adicionado para struct timeval e gettimeofday

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_SPEED

// Definições do LED
#define LED GPIO_NUM_2

// Definições do NRF24L01
#define NRF_CE GPIO_NUM_4
#define NRF_CSN GPIO_NUM_5
#define NRF_SCK GPIO_NUM_18
#define NRF_MOSI GPIO_NUM_23
#define NRF_MISO GPIO_NUM_19

// Constantes do NRF24L01
#define NRF24_CMD_R_REGISTER 0x00
#define NRF24_CMD_W_REGISTER 0x20
#define NRF24_CMD_R_RX_PAYLOAD 0x61
#define NRF24_CMD_W_TX_PAYLOAD 0xA0
#define NRF24_CMD_FLUSH_TX 0xE1
#define NRF24_CMD_FLUSH_RX 0xE2
#define NRF24_CMD_REUSE_TX_PL 0xE3
#define NRF24_CMD_R_RX_PL_WID 0x60
#define NRF24_CMD_W_ACK_PAYLOAD 0xA8
#define NRF24_CMD_W_TX_PAYLOAD_NOACK 0xB0
#define NRF24_CMD_NOP 0xFF

// Registradores do NRF24L01
#define NRF24_REG_CONFIG 0x00
#define NRF24_REG_EN_AA 0x01
#define NRF24_REG_EN_RXADDR 0x02
#define NRF24_REG_SETUP_AW 0x03
#define NRF24_REG_SETUP_RETR 0x04
#define NRF24_REG_RF_CH 0x05
#define NRF24_REG_RF_SETUP 0x06
#define NRF24_REG_STATUS 0x07
#define NRF24_REG_OBSERVE_TX 0x08
#define NRF24_REG_RPD 0x09
#define NRF24_REG_RX_ADDR_P0 0x0A
#define NRF24_REG_RX_ADDR_P1 0x0B
#define NRF24_REG_TX_ADDR 0x10
#define NRF24_REG_RX_PW_P0 0x11
#define NRF24_REG_RX_PW_P1 0x12
#define NRF24_REG_FIFO_STATUS 0x17
#define NRF24_REG_DYNPD 0x1C
#define NRF24_REG_FEATURE 0x1D

// Configuração do SPI
spi_device_handle_t spi;

static struct timeval time_new, time_old;
static long data_num = 0;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;


// Função para inicializar o SPI
void nrf24_init_spi() {
    esp_err_t ret;
    
    spi_bus_config_t buscfg = {
        .miso_io_num = NRF_MISO,
        .mosi_io_num = NRF_MOSI,
        .sclk_io_num = NRF_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0
    };
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
        .mode = 0,                          // SPI mode 0
        .spics_io_num = NRF_CSN,            // CS pin
        .queue_size = 7,                    // We want to be able to queue 7 transactions at a time
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0
    };
    
    // Initialize the SPI bus
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    
    // Attach the NRF24L01 to the SPI bus
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}

// Função para escrever um byte em um registrador do NRF24L01
void nrf24_write_register(uint8_t reg, uint8_t value) {
    esp_err_t ret;
    spi_transaction_t t;
    uint8_t tx_data[2] = {NRF24_CMD_W_REGISTER | reg, value};
    
    memset(&t, 0, sizeof(t));
    t.length = 16; // 2 bytes * 8 bits
    t.tx_buffer = tx_data;
    t.rx_buffer = NULL;
    
    gpio_set_level(NRF_CSN, 0);
    ret = spi_device_polling_transmit(spi, &t);
    gpio_set_level(NRF_CSN, 1);
    ESP_ERROR_CHECK(ret);
}

// Função para ler um byte de um registrador do NRF24L01
uint8_t nrf24_read_register(uint8_t reg) {
    esp_err_t ret;
    spi_transaction_t t;
    uint8_t tx_data[2] = {NRF24_CMD_R_REGISTER | reg, NRF24_CMD_NOP};
    uint8_t rx_data[2] = {0};
    
    memset(&t, 0, sizeof(t));
    t.length = 16; // 2 bytes * 8 bits
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;
    
    gpio_set_level(NRF_CSN, 0);
    ret = spi_device_polling_transmit(spi, &t);
    gpio_set_level(NRF_CSN, 1);
    ESP_ERROR_CHECK(ret);
    
    return rx_data[1];
}

// Função para escrever vários bytes em um registrador do NRF24L01
void nrf24_write_register_multi(uint8_t reg, const uint8_t* values, uint8_t length) {
    esp_err_t ret;
    spi_transaction_t t;
    uint8_t tx_data[length + 1];
    
    tx_data[0] = NRF24_CMD_W_REGISTER | reg;
    memcpy(&tx_data[1], values, length);
    
    memset(&t, 0, sizeof(t));
    t.length = (length + 1) * 8; // (length + 1) bytes * 8 bits
    t.tx_buffer = tx_data;
    t.rx_buffer = NULL;
    
    gpio_set_level(NRF_CSN, 0);
    ret = spi_device_polling_transmit(spi, &t);
    gpio_set_level(NRF_CSN, 1);
    ESP_ERROR_CHECK(ret);
}

// Função para inicializar o NRF24L01
void nrf24_init() {
    // Configurar os pinos CE e CSN
    gpio_reset_pin(NRF_CE);
    gpio_set_direction(NRF_CE, GPIO_MODE_OUTPUT);
    gpio_set_level(NRF_CE, 0);
    
    gpio_reset_pin(NRF_CSN);
    gpio_set_direction(NRF_CSN, GPIO_MODE_OUTPUT);
    gpio_set_level(NRF_CSN, 1);
    
    // Inicializar o SPI
    nrf24_init_spi();
    
    // Configurar o NRF24L01 como transmissor
    // 1. Desativar o chip
    gpio_set_level(NRF_CE, 0);
    
    // 2. Configurar o comprimento do endereço (5 bytes)
    nrf24_write_register(NRF24_REG_SETUP_AW, 0x03);
    
    // 3. Configurar retransmissão (500us de atraso, 15 tentativas)
    nrf24_write_register(NRF24_REG_SETUP_RETR, 0x5F);
    
    // 4. Configurar canal RF (canal 76, 2.476 GHz)
    nrf24_write_register(NRF24_REG_RF_CH, 76);
    
    // 5. Configurar velocidade e potência (2 Mbps, 0dBm)
    nrf24_write_register(NRF24_REG_RF_SETUP, 0x0E);
    
    // 6. Configurar endereço TX
    uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nrf24_write_register_multi(NRF24_REG_TX_ADDR, tx_addr, 5);
    
    // 7. Configurar endereço RX (para o pipe 0, necessário mesmo para TX)
    uint8_t rx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nrf24_write_register_multi(NRF24_REG_RX_ADDR_P0, rx_addr, 5);
    
    // 8. Configurar tamanho do payload (32 bytes)
    nrf24_write_register(NRF24_REG_RX_PW_P0, 32);
    
    // 9. Habilitar auto-acknowledge para o pipe 0
    nrf24_write_register(NRF24_REG_EN_AA, 0x01);
    
    // 10. Habilitar pipe 0
    nrf24_write_register(NRF24_REG_EN_RXADDR, 0x01);
    
    // 11. Configurar como transmissor, ativar CRC (2 bytes), power up
    nrf24_write_register(NRF24_REG_CONFIG, 0x0E);
    
    // 12. Esperar 5ms para estabilização
    vTaskDelay(pdMS_TO_TICKS(5));
}

// Função para transmitir dados via NRF24L01
bool nrf24_transmit(const uint8_t* data, uint8_t length) {
    // 1. Certificar-se de que estamos no modo standby
    gpio_set_level(NRF_CE, 0);
    
    // 2. Limpar o FIFO de TX
    esp_err_t ret;
    spi_transaction_t t;
    uint8_t tx_data = NRF24_CMD_FLUSH_TX;
    
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &tx_data;
    t.rx_buffer = NULL;
    
    gpio_set_level(NRF_CSN, 0);
    ret = spi_device_polling_transmit(spi, &t);
    gpio_set_level(NRF_CSN, 1);
    ESP_ERROR_CHECK(ret);
    
    // 3. Escrever o payload
    uint8_t tx_buffer[length + 1];
    tx_buffer[0] = NRF24_CMD_W_TX_PAYLOAD;
    memcpy(&tx_buffer[1], data, length);
    
    memset(&t, 0, sizeof(t));
    t.length = (length + 1) * 8;
    t.tx_buffer = tx_buffer;
    t.rx_buffer = NULL;
    
    gpio_set_level(NRF_CSN, 0);
    ret = spi_device_polling_transmit(spi, &t);
    gpio_set_level(NRF_CSN, 1);
    ESP_ERROR_CHECK(ret);
    
    // 4. Iniciar transmissão
    gpio_set_level(NRF_CE, 1);
    vTaskDelay(pdMS_TO_TICKS(1)); // Pulsar CE por pelo menos 10us
    gpio_set_level(NRF_CE, 0);
    
    // 5. Esperar a transmissão completar ou falhar
    uint32_t start_time = esp_timer_get_time();
    uint8_t status;
    
    do {
        status = nrf24_read_register(NRF24_REG_STATUS);
        if ((status & 0x20) || (esp_timer_get_time() - start_time > 500000)) { // 500ms timeout
            break;
        }
        vTaskDelay(1);
    } while (1);
    
    // 6. Limpar flags de interrupção
    nrf24_write_register(NRF24_REG_STATUS, status | 0x70);
    
    // 7. Verificar se a transmissão foi bem-sucedida
    return (status & 0x20) == 0;
}

static char *bda2str(uint8_t * bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

static void print_speed(void)
{
    float time_old_s = time_old.tv_sec + time_old.tv_usec / 1000000.0;
    float time_new_s = time_new.tv_sec + time_new.tv_usec / 1000000.0;
    float time_interval = time_new_s - time_old_s;
    float speed = data_num * 8 / time_interval / 1000.0;
    ESP_LOGI(SPP_TAG, "speed(%fs ~ %fs): %f kbit/s" , time_old_s, time_new_s, speed);
    data_num = 0;
    time_old.tv_sec = time_new.tv_sec;
    time_old.tv_usec = time_new.tv_usec;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    char bda_str[18] = {0};

    switch (event) {
    case ESP_SPP_INIT_EVT:
        if (param->init.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
            esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_INIT_EVT status:%d", param->init.status);
        }
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT status:%d handle:%"PRIu32" close_by_remote:%d", param->close.status,
                 param->close.handle, param->close.async);
        break;
    case ESP_SPP_START_EVT:
        if (param->start.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT handle:%"PRIu32" sec_id:%d scn:%d", param->start.handle, param->start.sec_id,
                     param->start.scn);
            esp_bt_gap_set_device_name(EXAMPLE_DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_START_EVT status:%d", param->start.status);
        }
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len:%d handle:%"PRIu32,
                 param->data_ind.len, param->data_ind.handle);
        if (param->data_ind.len < 128) {
            esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);
        }
#else
        gettimeofday(&time_new, NULL);
        data_num += param->data_ind.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
        // Processar os dados recebidos via Bluetooth
        if (param->data_ind.len > 0) {
            // Controlar o LED
            char cmd = param->data_ind.data[0];
            if (cmd == 'a') {
                gpio_set_level(LED, 0);
                ESP_LOGI(SPP_TAG, "LED desligado pelo comando 'a'");
            } else if (cmd == 'A') {
                gpio_set_level(LED, 1);
                ESP_LOGI(SPP_TAG, "LED Ligado pelo comando 'A'");
            }
            
            // Transmitir os dados via NRF24L01
            bool success = nrf24_transmit(param->data_ind.data, param->data_ind.len);
            if (success) {
                ESP_LOGI(SPP_TAG, "Dados transmitidos com sucesso via NRF24L01");
            } else {
                ESP_LOGE(SPP_TAG, "Falha na transmissão via NRF24L01");
            }
        }
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT status:%d handle:%"PRIu32", rem_bda:[%s]", param->srv_open.status,
                 param->srv_open.handle, bda2str(param->srv_open.rem_bda, bda_str, sizeof(bda_str)));
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    char bda_str[18] = {0};

    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s bda:[%s]", param->auth_cmpl.device_name,
                     bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)));
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_EXAMPLE_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %"PRIu32, param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%"PRIu32, param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d bda:[%s]", param->mode_chg.mode,
                 bda2str(param->mode_chg.bda, bda_str, sizeof(bda_str)));
        break;

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(SPP_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(SPP_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    // Configurar nome do dispositivo
    esp_bt_gap_set_device_name(EXAMPLE_DEVICE_NAME);

    // Registrar callbacks
    esp_bt_gap_register_callback(esp_bt_gap_cb);
    esp_spp_register_callback(esp_spp_cb);
    
    // Configuração do SPP
    esp_spp_cfg_t spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = false,
        .tx_buffer_size = 0, // Usar valor padrão
    };
    esp_spp_enhanced_init(&spp_cfg);

    // Configurar o GPIO do LED
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    // Inicializar o NRF24L01
    nrf24_init();

    ESP_LOGI(SPP_TAG, "Bluetooth SPP acceptor example started");
}