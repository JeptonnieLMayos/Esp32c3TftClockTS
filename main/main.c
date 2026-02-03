#include <stdio.h>
#include "graphics.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics.h"
#include "touch.h"
#include "esp_log.h"

#define PIN_SCLK 4
#define PIN_MISO 5
#define PIN_MOSI 6 

void init(void)
{
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_SCLK,
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1, 
        .max_transfer_sz = 240 * 320 * 2,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    graphics_init();
    touch_init();
    
    display_init(); // remove
}

#pragma region Remove start
// ---------------------------------

#define ICON_W 200
#define ICON_H 200


#include <stdlib.h>
#include <math.h>

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED   0xF800
#define GREEN 0x07E0   // FIXED
#define BLUE  0x001F   // FIXED
#define GRAY  0x8410

uint16_t clock_icon[ICON_W * ICON_H];


static inline void set_px(int x, int y, uint16_t c)
{
    if (x < 0 || y < 0 || x >= ICON_W || y >= ICON_H) return;
    clock_icon[y * ICON_W + x] = c;
}

static void draw_circle(int cx, int cy, int r, uint16_t c)
{
    for (int y = -r; y <= r; y++)
        for (int x = -r; x <= r; x++)
            if (x*x + y*y <= r*r)
                set_px(cx + x, cy + y, c);
}

static void draw_line(int x0, int y0, int x1, int y1, int r, uint16_t c)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        draw_circle(x0, y0, r, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static void draw_hour_markers(int cx, int cy, int face_r)
{
    const int mark_r = face_r - 6;

    for (int i = 0; i < 12; i++) {
        float a = i * (2.0f * M_PI / 12.0f);

        int x = cx + (int)(mark_r * sinf(a));
        int y = cy - (int)(mark_r * cosf(a)); // orientation FIXED

        int r;
        uint16_t color;

        if (i == 0) {
            r = 8; color = WHITE;      // 12
        } else if (i % 3 == 0) {
            r = 5; color = WHITE;    // 3,6,9
        } else {
            r = 3; color = WHITE;     // others
        }

        draw_circle(x, y, r, color);
    }
}

static inline void polar_to_xy(
    int cx, int cy,
    float angle, int r,
    int *x, int *y)
{
    *x = cx + (int)(r * sinf(angle));
    *y = cy + (int)(r * cosf(angle));
}

void build_clock_icon(float hour_angle, float minute_angle, float second_angle)
{
    for (int i = 0; i < ICON_W * ICON_H; i++)
        clock_icon[i] = BLACK;

    int cx = ICON_W / 2;
    int cy = ICON_H / 2;



    draw_circle(cx, cy, 100, WHITE);
    draw_circle(cx, cy, 98, BLACK);
    draw_hour_markers(cx, cy, 96);

    // second hand
    int sx = cx + (int)(88 * sinf(second_angle));
    int sy = cy + (int)(88 * cosf(second_angle)); // FIXED
    draw_line(cx, cy, sx, sy, 1, WHITE);

    // minute hand
    int mx = cx + (int)(68 * sinf(minute_angle));
    int my = cy + (int)(68 * cosf(minute_angle)); // FIXED
    draw_line(cx, cy, mx, my, 2, WHITE);

    // hour hand
    int hx = cx + (int)(48 * sinf(hour_angle));
    int hy = cy + (int)(48 * cosf(hour_angle)); // FIXED
    draw_line(cx, cy, hx, hy, 3, WHITE);



}


typedef enum {
    ADJ_NONE,
    ADJ_HOUR,
    ADJ_MINUTE,
    ADJ_SECONDS
} adjust_mode_t;

adjust_mode_t adjust_mode = ADJ_NONE;
static void ui_task(void *arg)
{
    int seconds = 0;
    int minutes = 0;
    int hours   = 0;


    while (1) {



        /* -------- Time adjustment -------- */
        
        if (adjust_mode == ADJ_HOUR) {
            hours = (hours + 1) % 24;
        }
        else if (adjust_mode == ADJ_MINUTE) {
            minutes = (minutes + 1) % 60;
        }
        else if (adjust_mode == ADJ_SECONDS) {
            seconds = 0;
        }
        else {
            seconds++;
            if (seconds >= 60) {
                seconds = 0;
                minutes++;
                if (minutes >= 60) {
                    minutes = 0;
                    hours = (hours + 1) % 24;
                }
            }
        }

        /* -------- Angle calculation -------- */
        float second_angle = seconds * (2.0f * M_PI / 60.0f) - M_PI;
        float minute_angle = minutes * (2.0f * M_PI / 60.0f) - M_PI;
        float hour_angle = ((hours % 12) + minutes / 60.0f) * (2.0f * M_PI / 12.0f) - M_PI;

        /* -------- Render -------- */
        build_clock_icon(hour_angle, minute_angle, second_angle);

        display_draw_icon(
            (240 - ICON_W) / 2,
            (320 - ICON_H) / 2,
            ICON_W,
            ICON_H,
            clock_icon
        );

        /* -------- Timing -------- */
        int delay_ms = (adjust_mode == ADJ_NONE) ? 1000 : 200;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


//----------------------------------
#pragma endregion //Remove



void set_bl(uint16_t y)
{
    uint8_t br = get_backlight_brightness();

    if( y < 160)
        --br;
    else if( y > 159)
        ++br;

    ESP_LOGI("s", "%d", br);


    set_backlight_brightness(br);

}

static void touch_task(void *arg)
{
    touch_point_t tp;

    while(1)
    {

        if(touch_read(&tp))
        {
            if(tp.x < 120)
            {
                set_bl(tp.y);


            }
            else if (tp.x > 119)
            {
                #pragma region Remove
                if (tp.x > 120)
                {
                    if (tp.y < 100)
                        adjust_mode = ADJ_HOUR;
                    else if (tp.y > 200)
                        adjust_mode = ADJ_SECONDS;
                    else
                        adjust_mode = ADJ_MINUTE;
                }
                #pragma endregion // Remove
            }
            
        }
        else adjust_mode = ADJ_NONE; // remove
    

        int delay_ms = (touch_is_pressed()) ? 50 : 1000;

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void app_main(void)
{
    init();
    set_backlight_brightness(80);

    xTaskCreate(touch_task, "touch", 4096, NULL, 5, NULL);
    display_fill_color(0x0000); // remove
    xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL); // remove



}
