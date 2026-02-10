#include <stdio.h>
#include "graphics.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics.h"
#include "touch.h"
#include "esp_log.h"
#include "clock_core.h"
#include "rect_clock.h"

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
