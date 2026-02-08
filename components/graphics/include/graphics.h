#pragma once
#include <stdint.h>
#include "backlight.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_ili9341.h"
#include "driver/gpio.h"
#include <string.h>


void gfx_draw_rect( int x0, int y0,
                    int x1, int y1,
                    uint16_t color,
                    uint16_t **buf
                );

void gfx_draw_line(
    int x0, int y0,
    int x1, int y1,
    int thickness,
    uint16_t color,
    uint16_t **buf
);

void gfx_clear(uint16_t color);

void graphics_init(void);
