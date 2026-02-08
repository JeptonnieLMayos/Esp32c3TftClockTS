#include <stdio.h>
#include "graphics.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics.h"
#include "touch.h"
#include "esp_log.h"
#include <math.h>

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
    
}


// Clock

void get_rect_point(
    float radians,
    int cx, int cy,
    int half_w, int half_h,
    int *out_x, int *out_y)
{
    float dx = cosf(radians);
    float dy = -sinf(radians);   // screen Y axis goes down

    float tx = 1e9f;
    float ty = 1e9f;

    // Vertical sides
    if (dx != 0.0f) {
        float x_edge = dx > 0 ? cx + half_w : cx - half_w;
        tx = (x_edge - cx) / dx;
    }

    // Horizontal sides
    if (dy != 0.0f) {
        float y_edge = dy > 0 ? cy + half_h : cy - half_h;
        ty = (y_edge - cy) / dy;
    }

    float t = tx < ty ? tx : ty;

    *out_x = cx + (int)(dx * t);
    *out_y = cy + (int)(dy * t);
}


void draw_hour_markers(uint16_t col)
{
    for (int i = 0; i < 12; ++i)
    {
        float angle_deg = i * 30.0f;
        float radians   = angle_deg * (M_PI / 180.0f);

        int ix, iy, ox, oy;
        
        int hwi = (i == 0 || i == 6 ) ? 100 : 102;
        int hwo = 110;
        int hhi = ( i == 3 || i == 9 ) ? 140 : 142;
        int hho = 150;




        get_rect_point(radians, 120, 160, hwi, hhi, &ix, &iy);
        get_rect_point(radians, 120, 160, hwo, hho, &ox, &oy);

        int thickness = i == 0 ? 10 : 6;
        gfx_draw_line(ix, iy, ox, oy, thickness, col);
    }
}


void draw_minute_markers(uint16_t col)
{
    for (int i = 0; i < 60; ++i)
    {
        if ( i % 5 != 0)
        {

            float angle_deg = i * 6.0f;
            float radians   = angle_deg * (M_PI / 180.0f);
    
            int ix, iy, ox, oy;
    
            get_rect_point(radians, 120, 160, 105, 145, &ix, &iy);
            get_rect_point(radians, 120, 160, 110, 150, &ox, &oy);
    
            gfx_draw_line(ix, iy, ox, oy, 1, col);
        }
    }
}





void draw_borders(void)
{

    uint16_t col = 0xFFFF;
    // uint16_t col = 0x001F;

        draw_minute_markers(col);
        draw_hour_markers(col);

        gfx_draw_rect(0, 0, 5,319, col);
        gfx_draw_rect(239, 0, 234, 319, col);
        gfx_draw_rect(5, 0, 233, 5, col);
        gfx_draw_rect(5, 314, 233, 319, col);
    
}


void clock (void)
{
    
}


// clock


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
            }
            
        }
    

        int delay_ms = (touch_is_pressed()) ? 50 : 1000;

        
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
    ESP_LOGI("t", "%d %d", tp.x, tp.y);
}

void app_main(void)
{
    init();
    set_backlight_brightness(80);

    xTaskCreate(touch_task, "touch", 4096, NULL, 5, NULL);

    draw_borders();

}
