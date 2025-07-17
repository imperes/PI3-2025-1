#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "mirf.h"                   // Biblioteca para comunicação com o módulo NRF24L01
#include "ssd1306.h"               // Biblioteca para controle do display OLED
#include "matrix_keyboard_4x4.h"   // Biblioteca para o teclado matricial 4x4

#define LED GPIO_NUM_2             // LED indicador (geralmente embutido)
#define BUZZER GPIO_NUM_16         // GPIO do buzzer (RX2 no ESP32)

static const char *TAG = "MOTORISTA";  // Tag para mensagens de log

// Estados principais da aplicação
typedef enum {
    ESTADO_DIGITANDO,      // Usuário digitando a linha de ônibus
    ESTADO_ESPERANDO_RF    // Esperando sinal RF do passageiro
} estado_t;

// Função principal executada como uma task do FreeRTOS
void receiver(void *pvParameters)
{
    ESP_LOGI(pcTaskGetName(NULL), "Start");

    // Inicializa o display OLED
    init_ssd1306();
    ssd1306_print_str(0, 0, "Digite linha:", false);
    ssd1306_display();

    // Inicializa teclado 4x4 com pinos de linha e coluna
    int row_pins[4] = {13, 12, 14, 27};
    int col_pins[4] = {26, 25, 33, 32};
    matrix_kbd_4x4_handle_t kbd = NULL;
    if (matrix_kbd_4x4_init(row_pins, col_pins, &kbd) != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao inicializar teclado");
        while (1) vTaskDelay(1);
    }

    char linha_onibus[17] = {0};     // Armazena o número da linha digitada
    size_t idx = 0;                  // Índice atual da string digitada
    estado_t estado = ESTADO_DIGITANDO;

    bool tela_atualizada = false;
    int64_t ultimo_ok_enviado = 0;  // Armazena o tempo da última resposta 'ok' enviada
    bool ok_habilitado = true;      // Flag para controlar o envio de resposta

    // Inicializa NRF24L01 para comunicação RF
    NRF24_t dev;
    Nrf24_init(&dev);
    uint8_t payload = 32;
    uint8_t channel = 10;
    Nrf24_config(&dev, channel, payload);

    // Configura pinos do LED e BUZZER
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    gpio_reset_pin(BUZZER);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 0);

    // Define endereço de recepção do NRF24
    esp_err_t ret = Nrf24_setRADDR(&dev, (uint8_t *)"FGHIJ");
    if (ret != ESP_OK) {
        ESP_LOGE(pcTaskGetName(NULL), "nrf24l01 not installed");
        while (1) vTaskDelay(1);
    }

    Nrf24_powerUpRx(&dev);
    Nrf24_printDetails(&dev);
    ESP_LOGI(pcTaskGetName(NULL), "Listening...");

    uint8_t buf[32];  // Buffer de recepção RF

    while (1) {
        uint32_t key_code;
        // Leitura do teclado matricial
        if (matrix_kbd_4x4_scan(kbd, &key_code) == ESP_OK && key_code != MATRIX_KBD_4X4_KEY_NONE) {
            char c = matrix_kbd_4x4_key_to_char(key_code);
            ESP_LOGI(TAG, "Tecla pressionada: %c", c);

            // '*' limpa e reinicia a digitação
            if (c == '*') {
                estado = ESTADO_DIGITANDO;
                idx = 0;
                memset(linha_onibus, 0, sizeof(linha_onibus));
                ok_habilitado = true;
                tela_atualizada = false;
                ssd1306_clear_display();
                ssd1306_print_str(0, 0, "Digite linha:", false);
                ssd1306_display();
                continue;
            }

            if (estado == ESTADO_DIGITANDO) {
                if (c == '#') {
                    // Finaliza digitação e muda para modo de espera RF
                    linha_onibus[idx] = '\0';
                    estado = ESTADO_ESPERANDO_RF;
                    ok_habilitado = true;
                    tela_atualizada = false;
                    ESP_LOGI(TAG, "Linha cadastrada: %s", linha_onibus);
                } else if (c == 'A') {
                    // Apaga o último dígito
                    if (idx > 0) {
                        idx--;
                        linha_onibus[idx] = '\0';
                    }
                    ssd1306_clear_display();
                    ssd1306_print_str(0, 0, "Digite linha:", false);
                    ssd1306_print_str(0, 16, linha_onibus, false);
                    ssd1306_display();
                } else if (c >= '0' && c <= '9') {
                    // Adiciona dígito à string da linha
                    if (idx < sizeof(linha_onibus) - 1) {
                        linha_onibus[idx++] = c;
                        linha_onibus[idx] = '\0';
                        ssd1306_clear_display();
                        ssd1306_print_str(0, 0, "Digite linha:", false);
                        ssd1306_print_str(0, 16, linha_onibus, false);
                        ssd1306_display();
                    }
                }
            }
            else if (estado == ESTADO_ESPERANDO_RF) {
                // 'C' permite novo embarque (reset visual)
                if (c == 'C') {
                    ok_habilitado = true;
                    tela_atualizada = false;
                    ssd1306_clear_display();
                    ssd1306_print_str(36, 20, "USUARIO", false);
                    ssd1306_print_str(28, 36, "EMBARCOU", false);
                    ssd1306_display();
                    ESP_LOGI(TAG, "Confirmacao do motorista: pronto para novo usuario");
                    vTaskDelay(pdMS_TO_TICKS(3000));
                }
            }
        }

        // Estado de espera por dados RF
        if (estado == ESTADO_ESPERANDO_RF) {
            if (!tela_atualizada) {
                ssd1306_clear_display();
                ssd1306_print_str(0, 0, "Linha:", false);
                ssd1306_print_str(48, 0, linha_onibus, false);
                ssd1306_display();
                tela_atualizada = true;
            }

            // Checa se há dados recebidos do NRF24
            while (Nrf24_dataReady(&dev)) {
                memset(buf, 0, sizeof(buf));
                Nrf24_getData(&dev, buf);
                ESP_LOGI(TAG, "Recebido: %s", buf);

                // Controle do LED com 'A'/'a'
                if (buf[0] == 'A') {
                    gpio_set_level(LED, 1);
                    ESP_LOGI(TAG, "LED ligado");
                } else if (buf[0] == 'a') {
                    gpio_set_level(LED, 0);
                    ESP_LOGI(TAG, "LED desligado");
                }

                // Verifica se a linha recebida é a correta
                if (ok_habilitado && strncmp((char *)buf, linha_onibus, strlen(linha_onibus)) == 0) {
                    int64_t agora = esp_timer_get_time();
                    if ((agora - ultimo_ok_enviado) > 10000000) { // 10s
                        ultimo_ok_enviado = agora;
                        ESP_LOGI(TAG, "Linha confirmada, enviando ok");

                        // Inicializa PWM para o buzzer
                        ledc_timer_config_t ledc_timer = {
                            .duty_resolution = LEDC_TIMER_8_BIT,
                            .freq_hz = 1000,
                            .speed_mode = LEDC_HIGH_SPEED_MODE,
                            .timer_num = LEDC_TIMER_0
                        };
                        ledc_timer_config(&ledc_timer);

                        ledc_channel_config_t ledc_channel = {
                            .channel    = LEDC_CHANNEL_0,
                            .duty       = 255,
                            .gpio_num   = BUZZER,
                            .speed_mode = LEDC_HIGH_SPEED_MODE,
                            .hpoint     = 0,
                            .timer_sel  = LEDC_TIMER_0
                        };
                        ledc_channel_config(&ledc_channel);

                        // Ativa buzzer com som intermitente
                        for (int i = 0; i < 10; i++) {
                            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 128);
                            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                            vTaskDelay(pdMS_TO_TICKS(200));
                            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
                            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
                            vTaskDelay(pdMS_TO_TICKS(100));
                        }

                        // Mostra mensagem de parada obrigatória
                        ssd1306_print_str(24, 16, "  AVISO", false);
                        ssd1306_print_str(24, 32, "  PARADA", false);
                        ssd1306_print_str(8, 48, "  OBRIGATORIA", false);
                        ssd1306_display();

                        // Prepara e envia resposta 'ok' via RF
                        Nrf24_setTADDR(&dev, (uint8_t *)"FGHIJ");
                        uint8_t resposta[32] = {0};
                        strcpy((char *)resposta, "ok");
                        vTaskDelay(pdMS_TO_TICKS(1000));

                        int tentativas = 0;
                        bool enviado = false;
                        while (tentativas < 3 && !enviado) {
                            Nrf24_send(&dev, resposta);
                            if (Nrf24_isSend(&dev, 1000)) {
                                ESP_LOGI(TAG, "Resposta 'ok' enviada (tentativa %d)", tentativas + 1);
                                enviado = true;
                            } else {
                                ESP_LOGW(TAG, "Falha ao enviar resposta (tentativa %d)", tentativas + 1);
                                vTaskDelay(pdMS_TO_TICKS(200));
                            }
                            tentativas++;
                        }

                        Nrf24_powerUpRx(&dev);  // Retorna para modo de recepção
                        ok_habilitado = false;  // Aguarda nova confirmação manual
                    }
                }
            }
        }

        // Aguarda pequena pausa entre interações
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Função principal do programa
void app_main(void)
{
    // Cria a task que executa a função receiver
    xTaskCreate(&receiver, "RECEIVER", 8192, NULL, 5, NULL);
}
