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

volatile int adjust = 0;


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
        uint16_t *b = NULL;
        gfx_draw_line(ix, iy, ox, oy, thickness, col, &b);
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
    
            get_rect_point(radians, 120, 160, 109, 149, &ix, &iy);
            get_rect_point(radians, 120, 160, 110, 150, &ox, &oy);
    
            uint16_t *b = NULL;
            gfx_draw_line(ix, iy, ox, oy, 1, col, &b);
        }
    }
}


void draw_borders(void)
{

    uint16_t col = 0xFFFF;
    // uint16_t col = 0x001F;

        draw_minute_markers(col);
        draw_hour_markers(col);

        uint16_t *bu = NULL; uint16_t *bb = NULL; uint16_t *br = NULL; uint16_t *bl = NULL;

        gfx_draw_rect(0, 0, 5,319, col, &bu);
        gfx_draw_rect(239, 0, 234, 319, col, &bb);
        gfx_draw_rect(5, 0, 233, 5, col, &br);
        gfx_draw_rect(5, 314, 233, 319, col, &bl);
    
}


void rect_clock (void *arg)
{
    draw_borders();

    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;

    float angle_per_tick = 6.0f;
    float radians = M_PI / 180.0f;

    float sec_angle = second * angle_per_tick;
    float sec_radians = sec_angle * radians;
    float min_angle = minute * angle_per_tick;
    float min_radians = min_angle * radians;
    float hour_angle = (hour * 30) + (minute * 0.5);
    float hour_radians = hour_angle * radians;
    

    uint16_t col = 0xFFFF;

    bool first = true;
    int spix, spiy, spox, spoy;
    int mpix, mpiy, mpox, mpoy;
    int hpix, hpiy, hpox, hpoy;
    uint16_t *sec_b = NULL; uint16_t *prev_sec_b = NULL;
    uint16_t *min_b = NULL; uint16_t *prev_min_b = NULL;
    uint16_t *hour_b = NULL; uint16_t *prev_hour_b = NULL;


    // adjust 
    while(1)
    {
        if( !first )
        {
            free(sec_b); free(prev_sec_b);
            free(min_b); free(prev_min_b);
            free(hour_b); free(prev_hour_b);
        }

        if ( !first )
        {
            gfx_draw_line( spix, spiy, spox, spoy, 2, 0x0000, &prev_sec_b );
        // gfx_draw_rect(spox, spoy, spox +1, spoy +1, 0x0000, &prev_sec_b);

            gfx_draw_line( mpix, mpiy, mpox, mpoy, 4, 0x0000, &prev_min_b );
            gfx_draw_line( hpix, hpiy, hpox, hpoy, 8, 0x0000, &prev_hour_b );
        }
        else
            first = false;

        sec_angle = second * angle_per_tick;
        sec_radians = sec_angle * radians;

        if (second == 0 || adjust != 0) // adjust
        {
            min_angle = minute * angle_per_tick;
            min_radians = min_angle * radians;
            hour_angle = (hour * 30) + (minute * 0.5);
            hour_radians = hour_angle * radians;
        }

        int sec_ix = 95; int sec_iy = 135; int sec_ox = 98; int sec_oy = 138;
        int min_ix = 85; int min_iy = 125; int min_ox = 94; int min_oy = 134;
        int hour_ix = 70; int hour_iy = 110; int hour_ox = 84; int hour_oy = 124;

        int six, siy, sox, soy;
        int mix, miy, mox, moy;
        int hix, hiy, hox, hoy;

        get_rect_point(sec_radians, 120, 160, sec_ix, sec_iy, &six, &siy);
        get_rect_point(sec_radians, 120, 160, sec_ox, sec_oy, &sox, &soy);
        get_rect_point(min_radians, 120, 160, min_ix, min_iy, &mix, &miy);
        get_rect_point(min_radians, 120, 160, min_ox, min_oy, &mox, &moy);
        get_rect_point(hour_radians, 120, 160, hour_ix, hour_iy, &hix, &hiy);
        get_rect_point(hour_radians, 120, 160, hour_ox, hour_oy, &hox, &hoy);


        gfx_draw_line( six, siy, sox, soy, 2, col, &sec_b );
        // gfx_draw_rect(sox, soy, sox +1, soy +1, col,  &sec_b);
        gfx_draw_line( mix, miy, mox, moy, 4, col, &min_b );
        gfx_draw_line( hix, hiy, hox, hoy, 8, col, &hour_b );



        spix = six; spiy = siy; spox = sox; spoy = soy;
        mpix = mix; mpiy = miy; mpox = mox; mpoy = moy;
        hpix = hix; hpiy = hiy; hpox = hox; hpoy = hoy;

        int delay = 1000;

        if (adjust == 0) // adjust
        {
            delay = 1000;
    
            if (second >= 59 )
            {
                minute = minute >= 60 ? 1 : minute + 1; // 60 to 59
            }
            second = second >= 59 ? 0 : second + 1;
            
            if ( minute == 60 && second >= 59) // 60 to 59
            {
                hour = hour <= 11 ? 0 : hour + 1;
            }
        }
        else
        {
            if (adjust == 1 )
                second = 0;
            if (adjust == 2 )
            {
                minute = minute >= 59 ? 0 : minute + 1;
                delay = 250;
            }
            if (adjust == 3 )
            {
                hour = hour >= 11 ? 0 : hour + 1;
                delay = 350;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(delay));
 
    }


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
                if(tp.y < 100)
                    adjust = 3;
                else if (tp.y > 200)
                    adjust = 1;
                else
                    adjust = 2;
            }
            
        }
        else
            adjust = 0;
    

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


    xTaskCreate(rect_clock, "clock", 4096, NULL, 1, NULL);

}
