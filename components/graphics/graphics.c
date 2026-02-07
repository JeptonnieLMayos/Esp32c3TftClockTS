#include <stdio.h>
#include "graphics.h"
// #include "esp_heap_caps.h" // later

#define PIN_TFT_CS 7
#define PIN_TFT_DC 8
#define PIN_TFT_RST 9

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED   0xF800
#define BLUE 0x07E0 
#define GREEN 0x001F 
#define GRAY  0x8410

static esp_lcd_panel_handle_t panel;

//---

void gfx_clear(uint16_t color)
{
    static uint16_t line[240];
    for (int i = 0; i < 240; i++) line[i] = color;

    for (int y = 0; y < 320; y++) {
        esp_lcd_panel_draw_bitmap(panel, 0, y, 240, y + 1, line);
    }
}


void graphics_init(void)
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

    backlight_init();

    gfx_clear(BLACK);
}

void gfx_draw_rect( int x0, int y0,
                    int x1, int y1,
                    uint16_t color)
{
    int xmin = x0 < x1 ? x0 : x1;
    int xmax = x0 > x1 ? x0 : x1;
    int ymin = y0 < y1 ? y0 : y1;
    int ymax = y0 > y1 ? y0 : y1;



    gfx_buffer_t g;
    g.width  = xmax - xmin + 1;
    g.height = ymax - ymin + 1;
    g.size = g.width * g.height * 2;
    g.buf = malloc(g.size);
    for (int i = 0; i < g.width * g.height; i++)
        g.buf[i] = color;
    
    esp_lcd_panel_draw_bitmap(
        panel,
        xmin, ymin,
        xmin + g.width,
        ymin + g.height,
        g.buf
    );
    
}   

void gfx_draw_line(
    int x0, int y0,
    int x1, int y1,
    int thickness,
    uint16_t color
)
{
    int r = thickness /2;


    int xmin = x0 < x1 ? x0 : x1 - r;
    int xmax = x0 > x1 ? x0 : x1 + r;
    int ymin = y0 < y1 ? y0 : y1 - r;
    int ymax = y0 > y1 ? y0 : y1 + r;

    gfx_buffer_t g;
    g.width  = xmax - xmin + 1;
    g.height = ymax - ymin + 1;
    g.size = g.width * g.height * sizeof(uint16_t);
    g.buf = malloc(g.size);
    for (int i = 0; i < g.width * g.height; i++)
        g.buf[i] = BLACK;

    // Bresenham
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;


    while (1) {
        int bx = x - xmin;
        int by = y - ymin;
        if (dx >= dy) {
            for (int t = -r; t <= r; t++) {
                int yy = by + t;
                if (yy >= 0 && yy < g.height)
                    g.buf[yy * g.width + bx] = color;
            }
        } else {
            for (int t = -r; t <= r; t++) {
                int xx = bx + t;
                if (xx >= 0 && xx < g.width)
                    g.buf[by * g.width + xx] = color;
            }
        }



        if (x == x1 && y == y1) break;
        
        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 <  dx) { err += dx; y += sy; }
    }
    



    esp_lcd_panel_draw_bitmap(
        panel,
        xmin, ymin,
        xmin + g.width,
        ymin + g.height,
        g.buf
    );

    // free(g.buf);
}