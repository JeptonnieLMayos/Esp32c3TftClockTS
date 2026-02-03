#include <stdio.h>
#include "touch.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#define TOUCH_IRQ_GPIO 0
#define TOUCH_CS       20

#define X_MIN 180
#define X_MAX 1850
#define Y_MIN 210
#define Y_MAX 1980


spi_device_handle_t touch_spi;

void touch_init(void)
{
    gpio_config_t irq = {
        .pin_bit_mask = 1ULL << TOUCH_IRQ_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&irq);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 2 * 1000 * 1000,  // 2 MHz
        .mode = 0,
        .spics_io_num = TOUCH_CS,
        .queue_size = 1,
    };

    spi_bus_add_device(SPI2_HOST, &devcfg, &touch_spi);
}

static uint16_t read_cmd(spi_device_handle_t spi, uint8_t cmd)
{
    uint8_t tx[3] = { cmd, 0x00, 0x00 };
    uint8_t rx[3] = { 0 };

    spi_transaction_t t = {
        .length = 24,
        .tx_buffer = tx,
        .rx_buffer = rx,
    };

    spi_device_transmit(spi, &t);

    return ((rx[1] << 8) | rx[2]) >> 4;
}

static int map(int v, int in_min, int in_max, int out_min, int out_max)
{
    if (v < in_min) v = in_min;
    if (v > in_max) v = in_max;
    return (v - in_min) * (out_max - out_min) /
           (in_max - in_min) + out_min;
}


bool touch_read(touch_point_t *p)
{
    if (gpio_get_level(TOUCH_IRQ_GPIO) != 0)
        return false;

    uint16_t raw_x = read_cmd(touch_spi, 0xD0);
    uint16_t raw_y = read_cmd(touch_spi, 0x90);

    p->x = map(raw_x, X_MIN, X_MAX, 0, 239);
    p->y = map(raw_y, Y_MIN, Y_MAX, 0, 319);


    return true;
    
}

bool touch_is_pressed(void)
{
    return gpio_get_level(TOUCH_IRQ_GPIO) == 0;
}