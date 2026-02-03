#include <stdio.h>
#include "graphics.h"

#define PIN_TFT_CS 7
#define PIN_TFT_DC 8
#define PIN_TFT_RST 9

void graphics_init(void)
{
    backlight_init();
}


#pragma region Remove



static esp_lcd_panel_handle_t panel;

void display_init(void)
{

    esp_lcd_panel_io_handle_t io;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num = PIN_TFT_DC,
        .cs_gpio_num = PIN_TFT_CS,
        .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_cfg, &io);

    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = PIN_TFT_RST,
        .bits_per_pixel = 16,
    };
    esp_lcd_new_panel_ili9341(io, &panel_cfg, &panel);

    esp_lcd_panel_reset(panel);
    esp_lcd_panel_init(panel);
    esp_lcd_panel_disp_on_off(panel, true);
}

void display_fill_color(uint16_t color)
{
    static uint16_t fb[240 * 320];
    for (int i = 0; i < 240 * 320; i++) fb[i] = color;
    esp_lcd_panel_draw_bitmap(panel, 0, 0, 240, 320, fb);
}

void display_draw_icon(int x, int y, int w, int h, const uint16_t *icon)
{
    esp_lcd_panel_draw_bitmap(panel, x, y, x + w, y + h, icon);
}

#pragma endregion //Remove
