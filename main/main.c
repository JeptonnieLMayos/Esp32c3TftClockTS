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
#include "input.h"
#include "temphumid.h"

#define PIN_SCLK 4
#define PIN_MISO 5
#define PIN_MOSI 6 

#define PIN_SDA 2
#define PIN_SCL 3

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
    input_init();

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    temphumid_init(I2C_NUM_0, 0x44);


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

static void thtask(void *arg)
{
    temphumid_data_t th;

    while(1)
    {
        temphumid_read(&th);
        ESP_LOGI("th", "%f | %f",th.temperature, th.humidity);
        int delay_ms = 1000;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

static void touch_task(void *arg)
{
    while(1)
    {
        input_update();
        int delay_ms = 100;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void app_main(void)
{
    init();
    set_backlight_brightness(80);

    xTaskCreate(touch_task, "touch", 4096, NULL, 5, NULL);


    xTaskCreate(rect_clock, "clock", 4096, NULL, 1, NULL);

    xTaskCreate(thtask, "th", 4096, NULL, 10, NULL);

}
