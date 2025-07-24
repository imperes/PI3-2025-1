/*
 * SPDX-FileCopyrightText: 2024 Seu Nome
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle
 */
typedef struct matrix_kbd_4x4_ctx_t *matrix_kbd_4x4_handle_t;

/**
 * @brief Key codes
 */
typedef enum {
    MATRIX_KBD_4X4_KEY_1    = 0x0000,
    MATRIX_KBD_4X4_KEY_2    = 0x0001,
    MATRIX_KBD_4X4_KEY_3    = 0x0002,
    MATRIX_KBD_4X4_KEY_A    = 0x0003,
    MATRIX_KBD_4X4_KEY_4    = 0x0100,
    MATRIX_KBD_4X4_KEY_5    = 0x0101,
    MATRIX_KBD_4X4_KEY_6    = 0x0102,
    MATRIX_KBD_4X4_KEY_B    = 0x0103,
    MATRIX_KBD_4X4_KEY_7    = 0x0200,
    MATRIX_KBD_4X4_KEY_8    = 0x0201,
    MATRIX_KBD_4X4_KEY_9    = 0x0202,
    MATRIX_KBD_4X4_KEY_C    = 0x0203,
    MATRIX_KBD_4X4_KEY_STAR = 0x0300,
    MATRIX_KBD_4X4_KEY_0    = 0x0301,
    MATRIX_KBD_4X4_KEY_HASH = 0x0302,
    MATRIX_KBD_4X4_KEY_D    = 0x0303,
    MATRIX_KBD_4X4_KEY_NONE = 0xFFFF
} matrix_kbd_4x4_key_t;

/**
 * @brief Initialize 4x4 keyboard
 *
 * @param row_pins 4 GPIOs of rows
 * @param col_pins 4 GPIOs of columns
 * @param out_handle Handle output
 * @return esp_err_t
 */
esp_err_t matrix_kbd_4x4_init(const int row_pins[4], const int col_pins[4], matrix_kbd_4x4_handle_t *out_handle);

/**
 * @brief Scan keyboard
 *
 * @param handle Handle
 * @param key_code Pointer to store code
 * @return esp_err_t
 */
esp_err_t matrix_kbd_4x4_scan(matrix_kbd_4x4_handle_t handle, uint32_t *key_code);

/**
 * @brief Convert code to char
 *
 * @param code
 * @return char or '\0'
 */
char matrix_kbd_4x4_key_to_char(uint32_t code);

#ifdef __cplusplus
}
#endif
