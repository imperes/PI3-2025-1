/*
 * SPDX-FileCopyrightText: 2024 Seu Nome
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "matrix_keyboard_4x4.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdlib.h>

struct matrix_kbd_4x4_ctx_t {
    int row_pins[4];
    int col_pins[4];
};

static const char *TAG = "kbd4x4";

esp_err_t matrix_kbd_4x4_init(const int row_pins[4], const int col_pins[4], matrix_kbd_4x4_handle_t *out_handle)
{
    if (!row_pins || !col_pins || !out_handle) return ESP_ERR_INVALID_ARG;

    matrix_kbd_4x4_handle_t ctx = calloc(1, sizeof(struct matrix_kbd_4x4_ctx_t));
    if (!ctx) return ESP_ERR_NO_MEM;

    // Configurar linhas como saída
    for (int i = 0; i < 4; i++) {
        ctx->row_pins[i] = row_pins[i];
        gpio_reset_pin(row_pins[i]);

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << row_pins[i]),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&io_conf);
        gpio_set_level(row_pins[i], 1);
    }

    // Configurar colunas como entrada com pull-up
    for (int i = 0; i < 4; i++) {
        ctx->col_pins[i] = col_pins[i];
        gpio_reset_pin(col_pins[i]);

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << col_pins[i]),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&io_conf);

        int lvl = gpio_get_level(col_pins[i]);
        ESP_LOGI(TAG, "Coluna %d GPIO %d Level inicial: %d", i, col_pins[i], lvl);
    }

    *out_handle = ctx;
    ESP_LOGI(TAG, "Teclado 4x4 inicializado.");
    return ESP_OK;
}

esp_err_t matrix_kbd_4x4_scan(matrix_kbd_4x4_handle_t ctx, uint32_t *key_code)
{
    if (!ctx || !key_code) return ESP_ERR_INVALID_ARG;

    *key_code = MATRIX_KBD_4X4_KEY_NONE;

    // Procurar tecla pressionada
    for (int row = 0; row < 4; row++) {
        gpio_set_level(ctx->row_pins[row], 0);

        for (int col = 0; col < 4; col++) {
            int lvl = gpio_get_level(ctx->col_pins[col]);
            if (lvl == 0) {
                // Tecla detectada
                *key_code = (row << 8) | col;

                // Aguarda tecla soltar
                bool solto = false;
                while (!solto) {
                    bool liberado = true;
                    for (int i = 0; i < 4; i++) {
                        if (gpio_get_level(ctx->col_pins[i]) == 0) {
                            liberado = false;
                            break;
                        }
                    }
                    if (liberado) {
                        solto = true;
                    } else {
                        vTaskDelay(pdMS_TO_TICKS(10));
                    }
                }

                // Pequeno atraso extra
                vTaskDelay(pdMS_TO_TICKS(50));

                gpio_set_level(ctx->row_pins[row], 1);
                return ESP_OK;
            }
        }

        gpio_set_level(ctx->row_pins[row], 1);
    }

    return ESP_OK;
}

char matrix_kbd_4x4_key_to_char(uint32_t code)
{
    switch (code) {
        case MATRIX_KBD_4X4_KEY_1:    return '1';
        case MATRIX_KBD_4X4_KEY_2:    return '2';
        case MATRIX_KBD_4X4_KEY_3:    return '3';
        case MATRIX_KBD_4X4_KEY_A:    return 'A';
        case MATRIX_KBD_4X4_KEY_4:    return '4';
        case MATRIX_KBD_4X4_KEY_5:    return '5';
        case MATRIX_KBD_4X4_KEY_6:    return '6';
        case MATRIX_KBD_4X4_KEY_B:    return 'B';
        case MATRIX_KBD_4X4_KEY_7:    return '7';
        case MATRIX_KBD_4X4_KEY_8:    return '8';
        case MATRIX_KBD_4X4_KEY_9:    return '9';
        case MATRIX_KBD_4X4_KEY_C:    return 'C';
        case MATRIX_KBD_4X4_KEY_STAR: return '*';
        case MATRIX_KBD_4X4_KEY_0:    return '0';
        case MATRIX_KBD_4X4_KEY_HASH: return '#';
        case MATRIX_KBD_4X4_KEY_D:    return 'D';
        default:                      return '\0';
    }
}
