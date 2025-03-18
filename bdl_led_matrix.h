#ifndef _BITDOGLAB_LED_MATRIX_
    #include "hardware/pio.h"
    #include "hardware/clocks.h"
    #include "pico/stdlib.h"
    #include <stdlib.h>
    #define _BITDOGLAB_LED_MATRIX_
    #define LED_PIN 7
    #define BITDOGLAB_M_PI 3.1415926536
    static volatile bool bitdog_matrix_update_ready = false;
    // FAST BYTE REVERSION LUT (0.25 KB)
    static const uint8_t reverse_lookup[256] = {
        0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
        0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
        0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
        0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
        0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
        0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
        0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
        0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
        0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
        0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
        0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
        0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
        0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
        0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
        0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
        0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
      };
    #define bdl_reverse_byte(b) reverse_lookup[b]
    typedef struct {
        uint8_t G, R, B;
        float W;
    } pixel_t;
      
    typedef struct {
        int rows, cols;
        uint state_machine;
        PIO pio;
        pixel_t* state;
        pixel_t* next_state;
    } LedMatrix;
    typedef struct repeating_timer repeating_timer;
    void bdl_pioInit(PIO *pio, uint *sm, uint pin);
    void bdl_matrixInit(LedMatrix *m, int control_pin, int rows, int cols);
    int  bdl_getIndex(LedMatrix *m, int row, int col);
    void bdl_matrixSetPixel(LedMatrix *m, int row, int col, uint8_t r, uint8_t g, uint8_t b, float w);
    void bdl_matrixWrite(LedMatrix *m);
    void bdl_matrixClear(LedMatrix *m);
    bool bdl_matrixUpdateTimer(repeating_timer *t);
    void bdl_waitMatrixUpdate();
    void bdl_startMatrixUpdater(LedMatrix *m, double fps);
    void bdl_matrixDrawLine(LedMatrix *m,int row0, int col0,int row1, int col1,uint8_t r, uint8_t g, uint8_t b,float w);
    #ifdef BDLED_IMPLEMENTATION
        #ifndef BDLED_IMPLEMENTATION_ADDED
            #define BDLED_IMPLEMENTATION_ADDED
            #include <stdio.h>
            #include <string.h>
            #include <math.h>
            #pragma once
            #if !PICO_NO_HARDWARE
                #include "hardware/pio.h"
            #endif
            // ------- //
            // ws2818b //
            // ------- //
            #define ws2818b_wrap_target 0
            #define ws2818b_wrap 3
            #define ws2818b_pio_version 0
            static const uint16_t ws2818b_program_instructions[] = {
                        //     .wrap_target
                0x6221, //  0: out    x, 1            side 0 [2] 
                0x1123, //  1: jmp    !x, 3           side 1 [1] 
                0x1400, //  2: jmp    0               side 1 [4] 
                0xa442, //  3: nop                    side 0 [4] 
                        //     .wrap
            };
            #if !PICO_NO_HARDWARE
            static const struct pio_program ws2818b_program = {
                .instructions = ws2818b_program_instructions,
                .length = 4,
                .origin = -1,
                .pio_version = ws2818b_pio_version,
            #if PICO_PIO_VERSION > 0
                .used_gpio_ranges = 0x0
            #endif
            };
            static inline pio_sm_config ws2818b_program_get_default_config(uint offset) {
                pio_sm_config c = pio_get_default_sm_config();
                sm_config_set_wrap(&c, offset + ws2818b_wrap_target, offset + ws2818b_wrap);
                sm_config_set_sideset(&c, 1, false, false);
                return c;
            }
            void ws2818b_program_init(PIO pio, uint sm, uint offset, uint pin, float freq) {
              pio_gpio_init(pio, pin);
              pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
              // Program configuration.
              pio_sm_config c = ws2818b_program_get_default_config(offset);
              sm_config_set_sideset_pins(&c, pin); // Uses sideset pins.
              sm_config_set_out_shift(&c, true, true, 8); // 8 bit transfers, right-shift.
              sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX); // Use only TX FIFO.
              float prescaler = clock_get_hz(clk_sys) / (10.f * freq); // 10 cycles per transmission, freq is frequency of encoded bits.
              sm_config_set_clkdiv(&c, prescaler);
              pio_sm_init(pio, sm, offset, &c);
              pio_sm_set_enabled(pio, sm, true);
            }
            void bdl_pioInit(PIO *pio, uint *sm, uint pin) {
                uint offset = pio_add_program(pio0, &ws2818b_program);
                *pio = pio0;
                *sm = pio_claim_unused_sm(*pio, false);
                if (*sm < 0) {
                  *pio = pio1;
                  *sm = pio_claim_unused_sm(*pio, true);
                }
                ws2818b_program_init(*pio, *sm, offset, pin, 800000.0);
            }
            void bdl_matrixInit(LedMatrix *m, int control_pin, int rows, int cols) {
                bdl_pioInit(&m->pio, &m->state_machine, control_pin);
                m->rows = rows;
                m->cols = cols;
                m->state = (pixel_t*)calloc(rows * cols, sizeof(pixel_t));
                m->next_state = (pixel_t *)calloc(rows * cols, sizeof(pixel_t));
                if (m->state == NULL) {
                  printf("Failed to allocate memory for LED state\n");
                  while (1);
                }
                for (int r = 0; r < rows; r++) {
                  for (int c = 0; c < cols; c++) {
                    m->state[r * cols + c] = (pixel_t){0, 0, 0, 0.0};
                  }
                }
            }
            int bdl_getIndex(LedMatrix *m, int row, int col) {
                return row % 2 != 0 ? row * m->cols + col : row * m->cols + (m->cols - 1 - col);
            }
            void bdl_matrixSetPixel(LedMatrix *m, int row, int col, uint8_t r, uint8_t g, uint8_t b, float w) {
                if (row >= 0 && row < m->rows && col >= 0 && col < m->cols) {
                    m->next_state[bdl_getIndex(m, row, col)] = (pixel_t){g, r, b, w};
                }
            }
            void bdl_matrixWrite(LedMatrix *m) {
                for (int i = 0; i < m->rows * m->cols; i++) {
                    pixel_t p = m->state[i];
                    pio_sm_put_blocking(m->pio, m->state_machine, bdl_reverse_byte((uint8_t)(p.G * p.W)));
                    pio_sm_put_blocking(m->pio, m->state_machine, bdl_reverse_byte((uint8_t)(p.R * p.W)));
                    pio_sm_put_blocking(m->pio, m->state_machine, bdl_reverse_byte((uint8_t)(p.B * p.W)));
                }
            }
            void bdl_matrixClear(LedMatrix *m) {
                memset(m->next_state, 0, m->rows * m->cols * sizeof(pixel_t));
            }
            bool bdl_matrixUpdateTimer(repeating_timer *t) {
                LedMatrix *m = (LedMatrix *)t->user_data;
                pixel_t *temp = m->state;
                m->state = m->next_state;
                m->next_state = temp;
                memcpy(m->next_state, m->state, m->cols * m->rows * sizeof(pixel_t));  
                bdl_matrixWrite(m);  
                bitdog_matrix_update_ready = true;
                return true;
            }
            void bdl_waitMatrixUpdate() {
                while (!bitdog_matrix_update_ready) {
                    tight_loop_contents();
                }
                bitdog_matrix_update_ready = false;
            }
            void bdl_startMatrixUpdater(LedMatrix *m, double fps) {
                static repeating_timer timer;
                add_repeating_timer_us(1000000.0 / fps, bdl_matrixUpdateTimer, (void*)m, &timer);
            }
            // Bresenham's algorithm.
            void bdl_matrixDrawLine(LedMatrix *m,int row0, int col0,int row1, int col1,uint8_t r, uint8_t g, uint8_t b,float w) {
                int dx = abs(col1 - col0);
                int sx = col0 < col1 ? 1 : -1;
                int dy = -abs(row1 - row0);
                int sy = row0 < row1 ? 1 : -1;
                int err = dx + dy;
                while (true) {
                    bdl_matrixSetPixel(m, row0, col0, r, g, b, w);
                    if (row0 == row1 && col0 == col1)
                    break;
                    int e2 = 2 * err;
                    if (e2 >= dy) {
                        err += dy;
                        col0 += sx;
                    }
                    if (e2 <= dx) {
                        err += dx;
                        row0 += sy;
                    }
                }
            }
            void bdl_hsv_to_rgb(float h, float s, float v, uint8_t *out_r, uint8_t *out_g, uint8_t *out_b) {
                float c = v * s;
                float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
                float m = v - c;
                float r1, g1, b1;
              
                if (h < 60) {
                  r1 = c; g1 = x; b1 = 0;
                } else if (h < 120) {
                  r1 = x; g1 = c; b1 = 0;
                } else if (h < 180) {
                  r1 = 0; g1 = c; b1 = x;
                } else if (h < 240) {
                  r1 = 0; g1 = x; b1 = c;
                } else if (h < 300) {
                  r1 = x; g1 = 0; b1 = c;
                } else {
                  r1 = c; g1 = 0; b1 = x;
                }
              
                *out_r = (uint8_t)round((r1 + m) * 255);
                *out_g = (uint8_t)round((g1 + m) * 255);
                *out_b = (uint8_t)round((b1 + m) * 255);
            }
            #endif
        #endif
    #endif
#endif