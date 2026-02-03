#include <stdio.h>
#include "graphics.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "graphics.h"
#include "touch.h"

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

void app_main(void)
{
    init();
    set_backlight_brightness(80);

    touch_point_t tp;

    if(touch_read(&tp))
    {
        if(tp.x < 120)
        {
            if(tp.y < 160)
            {}
            else if(tp.y > 159)
            {}
        }
        
    }
}
